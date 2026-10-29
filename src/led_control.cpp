#include "led_control.h"

// Tạo một task để điều khiển LED (Bài 2 & 3)
void task_led(void *pvParameters) {
  pinMode(LED_PIN, OUTPUT);

  while (1) {
    if (glob_sensor_error || (glob_temperature == 0.0 && glob_humidity == 0.0)) {

      // Khi cảm biến lỗi (Bài 3): LED sáng liên tục (Solid ON)
      digitalWrite(LED_PIN, HIGH);
      vTaskDelay(200);

    } else {
      // Khi bình thường: Tốc độ nhấp nháy phản ánh giá trị độ ẩm
      float humi = glob_humidity;
      uint32_t half_period_ms = 500; // Mặc định 1s

      if (humi < 40.0) {
        // Độ ẩm thấp (<40%): Nhấp nháy mỗi 2s (Sáng 1s, Tắt 1s)
        half_period_ms = 1000;
      } else if (humi <= 60.0) {
        // Độ ẩm bình thường (40%–60%): Nhấp nháy mỗi 1s (Sáng 0.5s, Tắt 0.5s)
        half_period_ms = 500;
      } else {
        // Độ ẩm cao (>60%): Nhấp nháy nhanh mỗi 0.5s (Sáng 0.25s, Tắt 0.25s)
        half_period_ms = 250;
      }

      digitalWrite(LED_PIN, HIGH);
      vTaskDelay(half_period_ms);

      digitalWrite(LED_PIN, LOW);
      vTaskDelay(half_period_ms);
    }
  }
}
