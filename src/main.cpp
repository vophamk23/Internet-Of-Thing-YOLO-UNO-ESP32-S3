#include "global.h"
#include "mainserver.h"
#include "sensor_task.h"
#include "internet_task.h"

// =============================================================
// Hàm Setup: Khởi tạo hệ thống & Kích hoạt các Task FreeRTOS
// Chức năng: Tạo Semaphore, Mutex và khởi chạy 3 tiến trình song song
// =============================================================
void setup() {

    // Khởi tạo cổng Serial để theo dõi log hệ thống
    Serial.begin(115200);
    vTaskDelay(pdMS_TO_TICKS(500));
    Serial.println("\n--- ESP32-S3 YoloUNO WebServer Firmware Starting ---");

    // 1. Tạo Semaphore đồng bộ hóa Internet và Mutex bảo vệ cảm biến
    xBinarySemaphoreInternet = xSemaphoreCreateBinary();
    xSensorMutex = xSemaphoreCreateMutex();

    if (xBinarySemaphoreInternet == NULL || xSensorMutex == NULL) {
        Serial.println("[ERROR] Khoi tao Semaphore hoac Mutex that bai!");
        while (1) {
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }

    // 2. Kích hoạt 3 Task FreeRTOS độc lập

    // Task 1: Xử lý Web Server nội bộ (Dashboard & Cấu hình Wi-Fi)
    xTaskCreate(main_server_task, // Hàm tác vụ
                "server",         // Tên tác vụ
                8192,             // Stack depth (bytes)
                NULL,             // Tham số truyền vào
                2,                // Độ ưu tiên (Priority)
                NULL              // Task handle
    );

    // Task 2: Giao tiếp MQTT với Cloud CoreIOT khi có mạng Internet (Bài tập 5)
    xTaskCreate(internet_task,    // Hàm tác vụ
                "internet",       // Tên tác vụ
                4096,             // Stack depth (bytes)
                NULL,             // Tham số truyền vào
                1,                // Độ ưu tiên (Priority)
                NULL              // Task handle
    );

    // Task 3: Thu thập dữ liệu cảm biến định kỳ và lọc trung bình trượt (Bài tập 1)
    xTaskCreate(sensor_task,      // Hàm tác vụ
                "sensor",         // Tên tác vụ
                4096,             // Stack depth (bytes)
                NULL,             // Tham số truyền vào
                1,                // Độ ưu tiên (Priority)
                NULL              // Task handle
    );

}

// =============================================================
// Hàm Loop: Nhàn rỗi (FreeRTOS tự động điều phối 3 Task trên)
// =============================================================
void loop() { 

    vTaskDelay(pdMS_TO_TICKS(1000)); 

}
