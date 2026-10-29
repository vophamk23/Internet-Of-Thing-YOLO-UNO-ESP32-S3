#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

#define LED_PIN GPIO_NUM_48 // LED on-board YoloUNO (GPIO 48)
#define NEO_PIN 45          // NeoPixel data pin (GPIO 45)
#define LED_COUNT 1         // 1 LED NeoPixel

// Biến chia sẻ trạng thái đồng bộ giữa 2 tác vụ
volatile bool led_state = false;

// Tác vụ 1: Điều khiển LED đơn (SÁNG 2s, TẮT 2s) và cập nhật biến led_state
void task_led_blinky(void *pvParameters) {
  pinMode(LED_PIN, OUTPUT);
  while (1) {
    digitalWrite(LED_PIN, HIGH);
    led_state = true; // Báo trạng thái LED đang SÁNG
    vTaskDelay(2000);

    digitalWrite(LED_PIN, LOW);
    led_state = false; // Báo trạng thái LED đang TẮT
    vTaskDelay(2000);
  }
}

// Tác vụ 2: Điều khiển NeoPixel theo biến led_state (ĐỎ khi LED SÁNG, TẮT khi LED TẮT)
void task_neo_blinky(void *pvParameters) {
  Adafruit_NeoPixel strip(LED_COUNT, NEO_PIN, NEO_GRB + NEO_KHZ800);
  strip.begin();
  strip.clear();
  strip.show();

  while (1) {
    if (led_state) {
      strip.setPixelColor(0, strip.Color(255, 0, 0)); // Bật ĐỎ khi LED SÁNG
    } else {
      strip.setPixelColor(0, strip.Color(0, 0, 0));   // TẮT khi LED TẮT
    }
    strip.show();
    vTaskDelay(50); // Kiểm tra lại sau 50ms
  }
}

void setup() {
  Serial.begin(115200);

  xTaskCreate(task_led_blinky,   // Hàm tác vụ
              "Task_LED_Blinky", // Tên tác vụ
              2048,              // Stack depth
              NULL,              // Tham số
              1,                 // Độ ưu tiên
              NULL               // Task handle
  );
  xTaskCreate(task_neo_blinky,   // Hàm tác vụ
              "Task_NEO_Blinky", // Tên tác vụ
              2048,              // Stack depth
              NULL,              // Tham số
              1,                 // Độ ưu tiên
              NULL               // Task handle
  );
}

void loop() {}
