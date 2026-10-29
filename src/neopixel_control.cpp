#include "neopixel_control.h"

static Adafruit_NeoPixel strip(LED_COUNT, NEO_PIN, NEO_GRB + NEO_KHZ800);

// Task điều khiển NeoPixel
void task_neopixel(void *pvParameters) {
  strip.begin();
  strip.clear();
  strip.show();

  while (1) {

    // Kiểm tra trạng thái lỗi cảm biến (Bài 3)
    if (glob_sensor_error || (glob_temperature == 0.0 && glob_humidity == 0.0)) {

      // Khi có lỗi: NeoPixel nhấp nháy màu VÀNG cảnh báo
      strip.setPixelColor(0, strip.Color(255, 200, 0)); // Bật VÀNG
      strip.show();
      vTaskDelay(500);

      strip.setPixelColor(0, strip.Color(0, 0, 0)); // Tắt (ĐEN)
      strip.show();
      vTaskDelay(500);

    } else {

      // 2. Trạng thái bình thường: Đổi màu theo giá trị nhiệt độ (Bài 1)
      float temp = glob_temperature;

      if (temp < 20.0) {
        // Màu XANH DƯƠNG nếu TEMP < 20°C
        strip.setPixelColor(0, strip.Color(0, 0, 255));
      } else if (temp < 28.0) {
        // Màu XANH LÁ nếu 20°C <= TEMP < 28°C
        strip.setPixelColor(0, strip.Color(0, 255, 0));
      } else {
        // Màu ĐỎ nếu TEMP >= 28°C
        strip.setPixelColor(0, strip.Color(255, 0, 0));
      }

      strip.show();

      // Kiểm tra và cập nhật lại màu sắc sau mỗi 1 giây
      vTaskDelay(1000);
    }
  }
}
