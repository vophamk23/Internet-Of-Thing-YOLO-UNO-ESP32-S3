#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

#define LED_PIN GPIO_NUM_48 // LED on-board YoloUNO (GPIO 48)
#define NEO_PIN 45          // NeoPixel data pin (GPIO 45)
#define LED_COUNT 1         // 1 LED NeoPixel

// Tác vụ 1: Nhấp nháy LED độc lập (SÁNG 1.5s, TẮT 0.5s)
void task_led_blinky(void *pvParameters) {
  pinMode(LED_PIN, OUTPUT);
  while (1) {
    digitalWrite(LED_PIN, HIGH);
    vTaskDelay(1500); // Sáng 1.5s

    digitalWrite(LED_PIN, LOW);
    vTaskDelay(500);  // Tắt 0.5s
  }
}

// Tác vụ 2: NeoPixel chuyển màu độc lập (ĐỎ 1s -> XANH LÁ 2s -> XANH DƯƠNG 2s)
void task_neo_blinky(void *pvParameters) {
  Adafruit_NeoPixel strip(LED_COUNT, NEO_PIN, NEO_GRB + NEO_KHZ800);
  strip.begin();
  strip.clear();
  strip.show();

  while (1) {
    strip.setPixelColor(0, strip.Color(255, 0, 0)); // ĐỎ 1 giây
    strip.show();
    vTaskDelay(1000);

    strip.setPixelColor(0, strip.Color(0, 255, 0)); // XANH LÁ 2 giây
    strip.show();
    vTaskDelay(2000);

    strip.setPixelColor(0, strip.Color(0, 0, 255)); // XANH DƯƠNG 2 giây
    strip.show();
    vTaskDelay(2000);
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
