#include "global.h"
#include "internet_task.h"
#include "mainserver.h"
#include "sensor_task.h"


// =============================================================
// Hàm Setup: Khởi tạo hệ thống & Kích hoạt các Task FreeRTOS
// Chức năng: Tạo Semaphore, Mutex và khởi chạy 3 tiến trình song song
// =============================================================
void setup() {

  // Khởi tạo cổng Serial để theo dõi log hệ thống
  Serial.begin(115200);
  // Chờ Serial Monitor trên máy tính kết nối (tối đa 2.5s để không bị trôi log
  // ban đầu)
  unsigned long start_wait = millis();
  while (!Serial && (millis() - start_wait < 2500)) {
    delay(50);
  }
  delay(200);

  Serial.println("\n==========================================================="
                 "===========");
  Serial.println(
      "  YOLOUNO ESP32-S3 LAB 4 - PHAM CONG VO (MSSV: 2313946 - LOP: L01)");
  Serial.println(
      "======================================================================");
  Serial.println(
      " [1] CHE DO KHOI TAO: Wi-Fi Access Point (Chua can ket noi Internet)");
  Serial.printf("     >> Ten Wi-Fi AP (SSID) : %s\n", ssid.c_str());
  Serial.printf("     >> Mat khau Wi-Fi AP   : %s\n", password.c_str());
  Serial.println("     >> Dia chi Web Server  : http://192.168.4.1");
  Serial.println(" [2] THONG TIN KET NOI COREIOT CLOUD (MQTT):");
  Serial.printf("     >> Broker Host         : %s (Port %d)\n", COREIOT_HOST,
                COREIOT_PORT);
  Serial.println("     >> Topic Telemetry     : v1/devices/me/telemetry");
  Serial.printf("     >> Device API Token    : %s\n", COREIOT_TOKEN);
  Serial.println("============================================================="
                 "=========\n");

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
  xTaskCreate(internet_task, // Hàm tác vụ
              "internet",    // Tên tác vụ
              4096,          // Stack depth (bytes)
              NULL,          // Tham số truyền vào
              1,             // Độ ưu tiên (Priority)
              NULL           // Task handle
  );

  // Task 3: Thu thập dữ liệu cảm biến định kỳ và lọc trung bình trượt (Bài tập
  // 1)
  xTaskCreate(sensor_task, // Hàm tác vụ
              "sensor",    // Tên tác vụ
              4096,        // Stack depth (bytes)
              NULL,        // Tham số truyền vào
              1,           // Độ ưu tiên (Priority)
              NULL         // Task handle
  );
}

// =============================================================
// Hàm Loop: Nhàn rỗi (FreeRTOS tự động điều phối 3 Task trên)
// =============================================================
void loop() { vTaskDelay(pdMS_TO_TICKS(1000)); }
