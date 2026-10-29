#include "mainserver.h"
#include "web_pages.h"
#include "global.h"
#include "sensor_task.h"

WebServer server(80);

bool isAPMode = true;
bool connecting = false;
uint32_t connect_start_ms = 0;

bool led1_state = false;
bool led2_state = false;

// -------------------------------------------------------------
// Route Handlers (Các hàm xử lý yêu cầu HTTP)
// -------------------------------------------------------------

// Phục vụ trang Dashboard từ bộ nhớ Flash PROGMEM
void handleRoot() {
    server.send_P(200, "text/html", PAGE_MAIN);
}

// Phục vụ trang Cài đặt Wi-Fi từ bộ nhớ Flash PROGMEM
void handleSettings() {
    server.send_P(200, "text/html", PAGE_SETTINGS);
}

// Endpoint trả về dữ liệu cảm biến (Bài tập 1 & Bài tập 3)
void handleSensors() {
    float t_avg = 0.0f, h_avg = 0.0f;

    // Lấy giá trị trung bình trượt 10 mẫu (Bài tập 1)
    if (!getAverage(&t_avg, &h_avg)) {
        t_avg = glob_temperature;
        h_avg = glob_humidity;
    }

    // Đánh giá mức độ ẩm có Hysteresis 1% (Bài tập 3)
    const char* h_level = humLevel(h_avg);

    char json[128];
    snprintf(json, sizeof(json), 
             "{\"temp\":%.2f,\"hum\":%.2f,\"hum_level\":\"%s\"}", 
             t_avg, h_avg, h_level);
    
    server.send(200, "application/json", json);
}

// Điều khiển bật tắt LED1 và LED2 (Checkpoint B6)
void handleToggle() {
    if (server.hasArg("led")) {
        String led = server.arg("led");

        if (led == "1") {
            led1_state = !led1_state;
            digitalWrite(LED1_PIN, led1_state ? HIGH : LOW);
            Serial.printf("[WEB ACTION] Nhan lenh tu Web: LED1 -> %s (GPIO %d)\n", led1_state ? "ON" : "OFF", LED1_PIN);
        } else if (led == "2") {
            led2_state = !led2_state;
            digitalWrite(LED2_PIN, led2_state ? HIGH : LOW);
            Serial.printf("[WEB ACTION] Nhan lenh tu Web: LED2 -> %s (GPIO %d)\n", led2_state ? "ON" : "OFF", LED2_PIN);
        }
    }

    String json = "{\"led1\":\"" + String(led1_state ? "ON" : "OFF") + 
                  "\",\"led2\":\"" + String(led2_state ? "ON" : "OFF") + "\"}";
    server.send(200, "application/json", json);
}

// Nhận thông tin cấu hình Wi-Fi từ form
void handleConnect() {
    if (server.hasArg("ssid")) {
        wifi_ssid = server.arg("ssid");
        wifi_password = server.hasArg("pass") ? server.arg("pass") : "";
        
        server.send(200, "text/plain", "Connecting....");

        connect_start_ms = millis();
        connecting = true;
        connectToWiFi();
    } else {
        server.send(400, "text/plain", "Missing SSID");
    }
}

// Endpoint thăm dò trạng thái kết nối bất đồng bộ (Bài tập 4)
void handleStatus() {
    String json = "{";

    if (isWifiConnected) {
        json += "\"state\":\"CONNECTED\",\"ip\":\"" + WiFi.localIP().toString() + "\"";
    } else if (connecting) {
        json += "\"state\":\"CONNECTING\",\"ip\":\"\"";
    } else {
        json += "\"state\":\"FAILED\",\"ip\":\"\"";
    }

    json += "}";
    server.send(200, "application/json", json);
}

// -------------------------------------------------------------
// Vòng đời quản lý mạng & Task Web Server
// -------------------------------------------------------------

void startAP() {
    WiFi.mode(WIFI_AP);

    if (!WiFi.softAP(ssid.c_str(), password.c_str())) {
        Serial.println("softAP failed (password >= 8?)");
    }

    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());

    isAPMode = true;
    connecting = false;
    isWifiConnected = false;
}

// Kết nối Wi-Fi với chế độ kép để giữ AP hoạt động (Bài tập 4)
void connectToWiFi() {
    WiFi.mode(WIFI_AP_STA);

    if (wifi_password.isEmpty()) {
        WiFi.begin(wifi_ssid.c_str());
    } else {
        WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());
    }

    Serial.print("Connecting to: ");
    Serial.println(wifi_ssid);
}

void setupServer() {
    server.on("/", HTTP_GET, handleRoot);
    server.on("/sensors", HTTP_GET, handleSensors);
    server.on("/toggle", HTTP_GET, handleToggle);
    server.on("/settings", HTTP_GET, handleSettings);
    server.on("/connect", HTTP_GET, handleConnect);
    server.on("/status", HTTP_GET, handleStatus);
    server.begin();

    Serial.println("HTTP Web Server started on port 80");
}

void main_server_task(void *pvParameters) {
    pinMode(BOOT_PIN, INPUT_PULLUP);
    pinMode(LED1_PIN, OUTPUT);
    pinMode(LED2_PIN, OUTPUT);

    digitalWrite(LED1_PIN, LOW);
    digitalWrite(LED2_PIN, LOW);

    startAP();
    setupServer();

    while (1) {
        server.handleClient();

        // Nhấn nút BOOT quay về AP mode (Checkpoint D6)
        if (digitalRead(BOOT_PIN) == LOW) {
            vTaskDelay(pdMS_TO_TICKS(100));
            if (digitalRead(BOOT_PIN) == LOW && !isAPMode) {
                Serial.println("[BOOT] Pressed -> Returning to AP Mode");
                startAP();
            }
        }

        // Giám sát quá trình kết nối Wi-Fi (Timeout 10 giây)
        if (connecting) {
            if (WiFi.status() == WL_CONNECTED) {
                Serial.print("STA IP address: ");
                Serial.println(WiFi.localIP());

                isWifiConnected = true;
                xSemaphoreGive(xBinarySemaphoreInternet); // Báo Internet sẵn sàng (Bài tập 5)

                isAPMode = false;
                connecting = false;
            } else if (millis() - connect_start_ms > 10000) {
                Serial.println("WiFi connect failed! Back to AP.");
                startAP();
            }
        }

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
