#include "internet_task.h"

#include <PubSubClient.h>
#include <WiFi.h>

#include "global.h"
#include "sensor_task.h"
#include "mainserver.h"

// Con trỏ toàn cục để gửi phản hồi RPC trong hàm Callback
static PubSubClient* g_mqttClient = NULL;

// =============================================================
// Hàm Callback: Xử lý lệnh điều khiển ngược từ CoreIOT Cloud (RPC Downlink)
// Cho phép bật/tắt LED1, LED2 trực tiếp từ Dashboard Cloud qua MQTT
// =============================================================
void mqttCallback(char* topic, byte* payload, unsigned int length) {
    char message[256];
    if (length >= sizeof(message)) length = sizeof(message) - 1;
    memcpy(message, payload, length);
    message[length] = '\0';

    Serial.println("\n----------------- [COREIOT RPC NHAN LENH NGUOC] -----------------");
    Serial.printf("  [!] Topic : %s\n", topic);
    Serial.printf("  [!] Lenh  : %s\n", message);

    String topicStr = String(topic);
    // Nhận RPC Request dạng: v1/devices/me/rpc/request/{requestId}
    if (topicStr.startsWith("v1/devices/me/rpc/request/")) {
        String requestId = topicStr.substring(26);

        // 1. Xử lý lệnh điều khiển LED 1
        if (strstr(message, "setLed1") || strstr(message, "setValue") || strstr(message, "led1")) {
            if (strstr(message, "true") || strstr(message, "1") || strstr(message, "on") || strstr(message, "ON")) {
                led1_state = true;
            } else if (strstr(message, "false") || strstr(message, "0") || strstr(message, "off") || strstr(message, "OFF")) {
                led1_state = false;
            }
            digitalWrite(LED1_PIN, led1_state ? HIGH : LOW);
            Serial.printf("  [+] Thuc thi: LED1 -> %s (GPIO %d)\n", led1_state ? "ON" : "OFF", LED1_PIN);
        }

        // 2. Xử lý lệnh điều khiển LED 2
        if (strstr(message, "setLed2") || strstr(message, "led2")) {
            if (strstr(message, "true") || strstr(message, "1") || strstr(message, "on") || strstr(message, "ON")) {
                led2_state = true;
            } else if (strstr(message, "false") || strstr(message, "0") || strstr(message, "off") || strstr(message, "OFF")) {
                led2_state = false;
            }
            digitalWrite(LED2_PIN, led2_state ? HIGH : LOW);
            Serial.printf("  [+] Thuc thi: LED2 -> %s (GPIO %d)\n", led2_state ? "ON" : "OFF", LED2_PIN);
        }

        // 3. Gửi phản hồi (RPC Response) ngược lại CoreIOT xác nhận lệnh thành công
        if (g_mqttClient != NULL && g_mqttClient->connected()) {
            String respTopic = "v1/devices/me/rpc/response/" + requestId;
            String respPayload = "{\"led1\":\"" + String(led1_state ? "ON" : "OFF") + 
                                 "\",\"led2\":\"" + String(led2_state ? "ON" : "OFF") + "\"}";
            g_mqttClient->publish(respTopic.c_str(), respPayload.c_str());
            Serial.printf("  [+] Phan hoi Cloud: %s -> %s\n", respTopic.c_str(), respPayload.c_str());
        }
    }
    Serial.println("-----------------------------------------------------------------\n");
}

// =============================================================
// Task: Truyền thông Đám mây CoreIOT & Báo động nguy hiểm (Bài tập 5)
// Chức năng: Đợi cờ Internet, gửi Telemetry 10s/lần, chốt báo động tức thời
// =============================================================
void internet_task(void* pvParameters) {
    WiFiClient espClient;
    PubSubClient mqttClient(espClient);
    mqttClient.setServer(COREIOT_HOST, COREIOT_PORT);
    mqttClient.setCallback(mqttCallback);
    g_mqttClient = &mqttClient;

    bool prev_danger = false;
    uint32_t last_telemetry_ms = 0;
    uint32_t reconnect_delay_ms = 2000;

    while (1) {
        // 1. Chờ cờ Internet từ Web Server (ngủ hoàn toàn, không tốn CPU)
        if (xSemaphoreTake(xBinarySemaphoreInternet, portMAX_DELAY) == pdTRUE) {
            Serial.println("[NET] Internet da san sang -> Bat dau ket noi MQTT CoreIOT");

            // Khởi tạo lại các biến trạng thái
            prev_danger = false;
            last_telemetry_ms = millis() - 10000;  // Gửi ngay lập tức gói tin đầu tiên
            reconnect_delay_ms = 2000;

            // 2. Vòng lặp duy trì khi còn kết nối Wi-Fi Router
            while (isWifiConnected && WiFi.status() == WL_CONNECTED) {
                // Tự động kết nối lại MQTT nếu bị rớt mạng
                if (!mqttClient.connected()) {
                    Serial.print("[MQTT] Dang ket noi Broker... ");

                    if (mqttClient.connect("YoloUNO_ESP32S3", COREIOT_TOKEN, "")) {
                        Serial.println("Thanh cong!");
                        // Đăng ký nhận lệnh điều khiển ngược từ CoreIOT Cloud (RPC Downlink)
                        mqttClient.subscribe("v1/devices/me/rpc/request/+");
                        Serial.println("[MQTT] Da dang ky topic nhan lenh nguoc: v1/devices/me/rpc/request/+");
                        reconnect_delay_ms = 2000;
                    } else {
                        Serial.printf("That bai (rc=%d). Thu lai sau %lu ms\n", mqttClient.state(), reconnect_delay_ms);
                        vTaskDelay(pdMS_TO_TICKS(reconnect_delay_ms));
                        reconnect_delay_ms = (reconnect_delay_ms * 2 > 30000) ? 30000 : (reconnect_delay_ms * 2);
                        continue;
                    }
                }

                mqttClient.loop();

                // 3. Gửi Telemetry định kỳ mỗi 10 giây lên topic CoreIOT
                if (millis() - last_telemetry_ms >= 10000) {
                    last_telemetry_ms = millis();

                    float t_avg = 0.0f, h_avg = 0.0f;
                    if (getAverage(&t_avg, &h_avg)) {
                        const char* t_lvl = tempLevel(t_avg);
                        const char* h_lvl = humLevel(h_avg);

                        // Đóng gói JSON Telemetry chuẩn CoreIOT rõ ràng, trực quan, dễ nhìn
                        char payload[256];
                        snprintf(payload, sizeof(payload),
                                 "{\n"
                                 "  \"temperature\": %.2f,\n"
                                 "  \"humidity\": %.2f,\n"
                                 "  \"temp_level\": \"%s\",\n"
                                 "  \"hum_level\": \"%s\"\n"
                                 "}",
                                 t_avg, h_avg, t_lvl, h_lvl);

                        // Gửi (Publish) lên topic Telemetry của CoreIOT
                        mqttClient.publish("v1/devices/me/telemetry", payload);

                        // In log ra Serial Monitor định dạng trực quan, cực kỳ dễ chụp báo cáo
                        Serial.println("\n------------------ [COREIOT TELEMETRY PUBLISH] ------------------");
                        Serial.printf("  [+] Topic MQTT : v1/devices/me/telemetry\n");
                        Serial.printf("  [+] Nhiet do TB: %.2f C  [Muc: %s]\n", t_avg, t_lvl);
                        Serial.printf("  [+] Do am TB   : %.2f %%  [Muc: %s]\n", h_avg, h_lvl);
                        Serial.println(" [+] Payload JSON:");
                        Serial.println(payload);
                        Serial.println("-----------------------------------------------------------------\n");

                        // 4. Cơ chế chốt cảnh báo khẩn cấp (Danger Alarm Latch) khi vượt ngưỡng
                        bool is_danger = (strcmp(t_lvl, "danger") == 0 || strcmp(h_lvl, "danger") == 0);
                        if (is_danger && !prev_danger) {
                            char alarm_payload[256];
                            snprintf(alarm_payload, sizeof(alarm_payload),
                                     "{\n"
                                     "  \"alarm\": \"HIGH_THRESHOLD_DANGER\",\n"
                                     "  \"temp\": %.2f,\n"
                                     "  \"hum\": %.2f,\n"
                                     "  \"msg\": \"Vuot nguong nguy hiem!\"\n"
                                     "}",
                                     t_avg, h_avg);
                            mqttClient.publish("v1/devices/me/telemetry", alarm_payload);

                            Serial.println("\n**************** [CANH BAO NGUY HIEM - COREIOT ALARM] ****************");
                            Serial.printf("  [!] Trang thai: PHAT HIEN VUOT NGUONG NGUY HIEM!\n");
                            Serial.printf("  [!] Nhiet do  : %.2f C | Do am: %.2f %%\n", t_avg, h_avg);
                            Serial.println(" [!] Goi tin Alarm JSON:");
                            Serial.println(alarm_payload);
                            Serial.println("**********************************************************************\n");
                        }
                        prev_danger = is_danger;
                    }
                }

                vTaskDelay(pdMS_TO_TICKS(100));  // Nhường CPU cho các task khác
            }

            // 5. Ngắt kết nối MQTT khi mất Wi-Fi hoặc khi người dùng bấm nút BOOT
            Serial.println("[NET] Ngat ket noi Wi-Fi -> Dong MQTT an toan");
            if (mqttClient.connected()) { mqttClient.disconnect(); }
        }
    }
}
