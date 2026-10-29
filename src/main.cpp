// ============================================================================
// CHỌN BÀI TẬP LAB 3 CẦN NẠP TẠI ĐÂY (Thay số: 1, 2, 3, 4 hoặc 5)
// ============================================================================
#define EXERCISE 1

#include <Arduino.h>

#include "global.h"
#include "temp_humi_monitor.h"
#include "neopixel_control.h"
#include "led_control.h"

void setup() {

  // Khởi tạo cổng truyền thông Serial Monitor
  Serial.begin(115200);

#if (EXERCISE == 1)
  // ==========================================================================
  // BÀI TẬP 1: Cảm biến DHT20 + NeoPixel chỉ thị nhiệt độ
  // ==========================================================================

  xTaskCreate(
      task_temp_humi_monitor, // Hàm tác vụ
      "Temp_Humi_Monitor",    // Tên tác vụ
      4096,                   // Stack depth (bytes)
      NULL,                   // Tham số truyền vào
      2,                      // Độ ưu tiên (Priority)
      NULL                    // Task handle
  );

  xTaskCreate(
      task_neopixel,          // Hàm tác vụ
      "Task_NeoPixel",        // Tên tác vụ
      2048,                   // Stack depth (bytes)
      NULL,                   // Tham số truyền vào
      1,                      // Độ ưu tiên (Priority)
      NULL                    // Task handle
  );

#elif (EXERCISE == 2 || EXERCISE == 3)
  // ==========================================================================
  // BÀI TẬP 2 & 3: Cảm biến + NeoPixel nhiệt độ + LED nhấp nháy độ ẩm / Báo lỗi
  // ==========================================================================

  xTaskCreate(
      task_temp_humi_monitor, // Hàm tác vụ
      "Temp_Humi_Monitor",    // Tên tác vụ
      4096,                   // Stack depth (bytes)
      NULL,                   // Tham số truyền vào
      2,                      // Độ ưu tiên (Priority)
      NULL                    // Task handle
  );

  xTaskCreate(
      task_neopixel,          // Hàm tác vụ
      "Task_NeoPixel",        // Tên tác vụ
      2048,                   // Stack depth (bytes)
      NULL,                   // Tham số truyền vào
      1,                      // Độ ưu tiên (Priority)
      NULL                    // Task handle
  );

  xTaskCreate(
      task_led,               // Hàm tác vụ
      "Task_LED_Control",     // Tên tác vụ
      2048,                   // Stack depth (bytes)
      NULL,                   // Tham số truyền vào
      1,                      // Độ ưu tiên (Priority)
      NULL                    // Task handle
  );

#elif (EXERCISE == 4)
  // ==========================================================================
  // BÀI TẬP 4: Bộ lọc trung bình (Average Filter) + NeoPixel + LED
  // ==========================================================================

  xTaskCreate(
      task_temp_humi_filter,  // Hàm tác vụ
      "Temp_Humi_Filter",     // Tên tác vụ
      4096,                   // Stack depth (bytes)
      NULL,                   // Tham số truyền vào
      2,                      // Độ ưu tiên (Priority)
      NULL                    // Task handle
  );

  xTaskCreate(
      task_neopixel,          // Hàm tác vụ
      "Task_NeoPixel",        // Tên tác vụ
      2048,                   // Stack depth (bytes)
      NULL,                   // Tham số truyền vào
      1,                      // Độ ưu tiên (Priority)
      NULL                    // Task handle
  );

  xTaskCreate(
      task_led,               // Hàm tác vụ
      "Task_LED_Control",     // Tên tác vụ
      2048,                   // Stack depth (bytes)
      NULL,                   // Tham số truyền vào
      1,                      // Độ ưu tiên (Priority)
      NULL                    // Task handle
  );

#elif (EXERCISE == 5)
  // ==========================================================================
  // BÀI TẬP 5: Bộ lọc nâng cao (Median + EMA) + NeoPixel + LED
  // ==========================================================================

  xTaskCreate(
      task_advanced_filter,   // Hàm tác vụ
      "Task_Adv_Filter",      // Tên tác vụ
      4096,                   // Stack depth (bytes)
      NULL,                   // Tham số truyền vào
      2,                      // Độ ưu tiên (Priority)
      NULL                    // Task handle
  );

  xTaskCreate(
      task_neopixel,          // Hàm tác vụ
      "Task_NeoPixel",        // Tên tác vụ
      2048,                   // Stack depth (bytes)
      NULL,                   // Tham số truyền vào
      1,                      // Độ ưu tiên (Priority)
      NULL                    // Task handle
  );

  xTaskCreate(
      task_led,               // Hàm tác vụ
      "Task_LED_Control",     // Tên tác vụ
      2048,                   // Stack depth (bytes)
      NULL,                   // Tham số truyền vào
      1,                      // Độ ưu tiên (Priority)
      NULL                    // Task handle
  );

#endif
}

void loop() {
  // FreeRTOS quản lý tất cả các tác vụ đa nhiệm, loop() để trống
}
