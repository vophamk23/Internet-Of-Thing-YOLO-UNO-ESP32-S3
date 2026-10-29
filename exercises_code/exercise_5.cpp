#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

#define LED_PIN GPIO_NUM_48 // LED on-board YoloUNO (GPIO 48)
#define NEO_PIN 45          // NeoPixel data pin (GPIO 45)
#define LED_COUNT 1         // 1 LED NeoPixel

// Cấu trúc pha đèn giao thông (Màu sắc và thời gian sáng)
struct TrafficPhase {
  uint8_t r, g, b;
  uint32_t duration_ms;
};

// Chuỗi trạng thái đèn giao thông: ĐỎ 7s -> VÀNG 2s -> XANH 5s
const TrafficPhase trafficPhases[3] = {
    {255, 0, 0, 7000},   // Đèn ĐỎ: 7 giây
    {255, 180, 0, 2000}, // Đèn VÀNG (Amber): 2 giây
    {0, 255, 0, 5000}    // Đèn XANH: 5 giây
};

// Tác vụ 1: Nhịp tim hệ thống (SÁNG 1 giây / TẮT 1 giây)
void task_led_blinky(void *pvParameters) {
  pinMode(LED_PIN, OUTPUT);

  while (1) {
    digitalWrite(LED_PIN, HIGH); // Heartbeat SÁNG 1s
    vTaskDelay(1000);

    digitalWrite(LED_PIN, LOW);  // Heartbeat TẮT 1s
    vTaskDelay(1000);
  }
}

// Tác vụ 2: Đèn giao thông tuần tự theo chu kỳ
void task_traffic_light(void *pvParameters) {
  Adafruit_NeoPixel strip(LED_COUNT, NEO_PIN, NEO_GRB + NEO_KHZ800);
  strip.begin();
  strip.clear();
  strip.show();

  int phase = 0;

  while (1) {
    TrafficPhase p = trafficPhases[phase];
    strip.setPixelColor(0, strip.Color(p.r, p.g, p.b));
    strip.show();

    vTaskDelay(p.duration_ms); // Giữ màu theo thời gian quy định của từng pha

    // Chuyển sang pha tiếp theo trong chu kỳ (Đỏ -> Vàng -> Xanh -> Đỏ...)
    phase = (phase + 1) % 3;
  }
}

void setup() {
  Serial.begin(115200);

  xTaskCreate(task_led_blinky,      // Hàm tác vụ
              "Task_LED_Heartbeat", // Tên tác vụ
              2048,                 // Stack depth
              NULL,                 // Tham số
              1,                    // Độ ưu tiên
              NULL                  // Task handle
  );
  xTaskCreate(task_traffic_light,   // Hàm tác vụ
              "Task_Traffic_Light", // Tên tác vụ
              2048,                 // Stack depth
              NULL,                 // Tham số
              1,                    // Độ ưu tiên
              NULL                  // Task handle
  );
}

void loop() {}
