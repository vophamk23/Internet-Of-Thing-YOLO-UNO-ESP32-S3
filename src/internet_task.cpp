#include "internet_task.h"

#include <PubSubClient.h>
#include <WiFi.h>

#include "global.h"
#include "sensor_task.h"

// =============================================================
// Task: Truyền thông Đám mây CoreIOT & Báo động nguy hiểm (Bài tập 5)
// Chức năng: Đợi cờ Internet, gửi Telemetry 10s/lần, chốt báo động tức thời
// =============================================================
void internet_task(void* pvParameters) {
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
            last_telemetry_ms = millis() - 10000;  // Gửi ngay lập tức gói tin đầu tiên
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
                        Serial.printf("That bai (rc=%d). Thu lai sau %lu ms\n", mqttClient.state(), reconnect_delay_ms);
                        vTaskDelay(pdMS_TO_TICKS(reconnect_delay_ms));
                        // Tăng thời gian chờ theo hàm mũ (Exponential Back-off, tối đa 30s)
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

                        // In log ra Serial Monitor định dạng trực quan, dễ theo dõi
                        Serial.println("\n------------------ [COREIOT TELEMETRY PUBLISH] ------------------");
                        Serial.printf("  [+] Topic MQTT : v1/devices/me/telemetry\n");
                        Serial.printf("  [+] Nhiet do TB: %.2f C  [Muc: %s]\n", t_avg, t_lvl);
                        Serial.printf("  [+] Do am TB   : %.2f %%  [Muc: %s]\n", h_avg, h_lvl);
                        Serial.println("  [+] Payload JSON:");
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
                            Serial.println("  [!] Goi tin Alarm JSON:");
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
            if (mqttClient.connected()) {
                mqttClient.disconnect();
            }
        }
    }
}
