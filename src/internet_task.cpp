#include "internet_task.h"
#include "global.h"
#include "sensor_task.h"
#include <WiFi.h>
#include <PubSubClient.h>

// Thông tin máy chủ MQTT CoreIOT (ThingsBoard)
const char* COREIOT_HOST  = "app.coreiot.io";
const int   COREIOT_PORT  = 1883;
const char* COREIOT_TOKEN = "ACCESS_TOKEN";     // Thay bằng Access Token của bạn

// =============================================================
// Task: Truyền thông Đám mây CoreIOT & Báo động nguy hiểm (Bài tập 5)
// Chức năng: Đợi cờ Internet, gửi Telemetry 10s/lần, chốt báo động tức thời
// =============================================================
void internet_task(void *pvParameters) {
    WiFiClient espClient;
    PubSubClient mqttClient(espClient);
    mqttClient.setServer(COREIOT_HOST, COREIOT_PORT);

    bool prev_danger = false;
    uint32_t last_telemetry_ms = 0;
    uint32_t reconnect_delay_ms = 2000;

    while (1) {
        // 1. Chờ cờ Internet từ Web Server (ngủ hoàn toàn, không tốn CPU)
        if (xSemaphoreTake(xBinarySemaphoreInternet, portMAX_DELAY) == pdTRUE) {
            Serial.println("[NET] Internet da san sang -> Bat dau ket noi MQTT CoreIOT");

            // Khởi tạo lại các biến trạng thái
            prev_danger = false;
            last_telemetry_ms = millis() - 10000;   // Gửi ngay lập tức gói tin đầu tiên
            reconnect_delay_ms = 2000;

            // 2. Vòng lặp duy trì khi còn kết nối Wi-Fi Router
            while (isWifiConnected && WiFi.status() == WL_CONNECTED) {

                // Tự động kết nối lại MQTT nếu bị rớt mạng
                if (!mqttClient.connected()) {
                    Serial.print("[MQTT] Dang ket noi Broker... ");

                    if (mqttClient.connect("YoloUNO_ESP32S3", COREIOT_TOKEN, "")) {
                        Serial.println("Thanh cong!");
                        reconnect_delay_ms = 2000;
                    } else {
                        Serial.printf("That bai (rc=%d). Thu lai sau %lu ms\n", 
                                      mqttClient.state(), reconnect_delay_ms);
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

                        // Đóng gói và publish Telemetry
                        char payload[160];
                        snprintf(payload, sizeof(payload),
                            "{\"temperature\":%.2f,\"humidity\":%.2f,\"temp_level\":\"%s\",\"hum_level\":\"%s\"}",
                            t_avg, h_avg, t_lvl, h_lvl);
                        mqttClient.publish("v1/devices/me/telemetry", payload);
                        Serial.printf("[COREIOT] Telemetry: %s\n", payload);

                        // 4. Cơ chế chốt cảnh báo khẩn cấp (Danger Alarm Latch) khi vượt ngưỡng
                        bool is_danger = (strcmp(t_lvl, "danger") == 0 || strcmp(h_lvl, "danger") == 0);
                        if (is_danger && !prev_danger) {
                            char alarm_payload[160];
                            snprintf(alarm_payload, sizeof(alarm_payload),
                                "{\"alarm\":\"HIGH_THRESHOLD_DANGER\",\"temp\":%.2f,\"hum\":%.2f,\"msg\":\"Vuot nguong nguy hiem!\"}",
                                t_avg, h_avg);
                            mqttClient.publish("v1/devices/me/telemetry", alarm_payload);
                            Serial.printf("[COREIOT ALARM] %s\n", alarm_payload);
                        }
                        prev_danger = is_danger;
                    }
                }

                vTaskDelay(pdMS_TO_TICKS(100)); // Nhường CPU cho các task khác
            }

            // 5. Ngắt kết nối MQTT khi mất Wi-Fi hoặc khi người dùng bấm nút BOOT
            Serial.println("[NET] Ngat ket noi Wi-Fi -> Dong MQTT an toan");
            if (mqttClient.connected()) {
                mqttClient.disconnect();
            }
        }
    }
}
