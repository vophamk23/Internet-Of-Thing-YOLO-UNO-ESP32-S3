#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

#define LED_PIN GPIO_NUM_48 // LED on-board YoloUNO (GPIO 48)
#define NEO_PIN 45          // NeoPixel data pin (GPIO 45)
#define LED_COUNT 1         // 1 LED NeoPixel

// Cấu trúc RGB Color
struct RGBColor {
  uint8_t r, g, b;
};

// Mảng 5 màu cầu vồng
const RGBColor rainbowColors[5] = {
    {255, 0, 0},   // ĐỎ
    {255, 255, 0}, // VÀNG
    {0, 255, 0},   // XANH LÁ
    {0, 255, 255}, // XANH NGỌC (Cyan)
    {0, 0, 255}    // XANH DƯƠNG
};

// Tác vụ 1: LED đơn nhấp nháy SÁNG 2s, TẮT 2s
void task_led_blinky(void *pvParameters) {
  pinMode(LED_PIN, OUTPUT);
  while (1) {
    digitalWrite(LED_PIN, HIGH);
    vTaskDelay(2000);

    digitalWrite(LED_PIN, LOW);
    vTaskDelay(2000);
  }
}

// Tác vụ 2: NeoPixel chuyển 5 màu cầu vồng, mỗi màu giữ 3 giây (3000 ms)
void task_neo_rainbow(void *pvParameters) {
  Adafruit_NeoPixel strip(LED_COUNT, NEO_PIN, NEO_GRB + NEO_KHZ800);
  strip.begin();
  strip.clear();
  strip.show();

  int colorIndex = 0;

  while (1) {
    RGBColor c = rainbowColors[colorIndex];
    strip.setPixelColor(0, strip.Color(c.r, c.g, c.b));
    strip.show();

    vTaskDelay(3000); // Giữ 3 giây

    // Chuyển sang màu tiếp theo
    colorIndex = (colorIndex + 1) % 5;
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
  xTaskCreate(task_neo_rainbow,  // Hàm tác vụ
              "Task_NEO_Rainbow", // Tên tác vụ
              2048,              // Stack depth
              NULL,              // Tham số
              1,                 // Độ ưu tiên
              NULL               // Task handle
  );
}

void loop() {}
