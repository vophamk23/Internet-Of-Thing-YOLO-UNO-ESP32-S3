// ============================================================================
// CHỌN BÀI TẬP CẦN NẠP TẠI ĐÂY (Thay số: 1, 2, 3, 4 hoặc 5)
// ============================================================================
#define EXERCISE 1

#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

#define LED_PIN GPIO_NUM_48 // LED on-board YoloUNO (GPIO 48)
#define NEO_PIN 45          // NeoPixel data pin (GPIO 45)
#define LED_COUNT 1         // 1 LED NeoPixel

// ============================================================================
// BÀI TẬP 1: Nhấp nháy luân phiên đơn giản (SÁNG 1s, TẮT 2s)
// ============================================================================
#if (EXERCISE == 1)

void task_led_blinky(void *pvParameters) {
  pinMode(LED_PIN, OUTPUT);
  while (1) {
    digitalWrite(LED_PIN, HIGH);
    vTaskDelay(100); // Sáng 1 giây

    digitalWrite(LED_PIN, LOW);
    vTaskDelay(200); // Tắt 2 giây
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
}

void loop() {}

// ============================================================================
// BÀI TẬP 2: Phối hợp LED và NeoPixel (Đồng bộ qua biến led_state)
// ============================================================================
#elif (EXERCISE == 2)

volatile bool led_state = false;

void task_led_blinky(void *pvParameters) {
  pinMode(LED_PIN, OUTPUT);
  while (1) {
    digitalWrite(LED_PIN, HIGH);
    led_state = true; // LED SÁNG
    vTaskDelay(2000);

    digitalWrite(LED_PIN, LOW);
    led_state = false; // LED TẮT
    vTaskDelay(2000);
  }
}

void task_neo_blinky(void *pvParameters) {
  Adafruit_NeoPixel strip(LED_COUNT, NEO_PIN, NEO_GRB + NEO_KHZ800);
  strip.begin();
  strip.clear();
  strip.show();

  while (1) {
    if (led_state) {
      strip.setPixelColor(0, strip.Color(255, 0, 0)); // Bật ĐỎ khi LED SÁNG
    } else {
      strip.setPixelColor(0, strip.Color(0, 0, 0)); // TẮT khi LED TẮT
    }
    strip.show();
    vTaskDelay(50);
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

// ============================================================================
// BÀI TẬP 3: Các mẫu nhấp nháy độc lập (LED 1.5s/0.5s - NeoPixel Đỏ/Lá/Dương)
// ============================================================================
#elif (EXERCISE == 3)

void task_led_blinky(void *pvParameters) {
  pinMode(LED_PIN, OUTPUT);
  while (1) {
    digitalWrite(LED_PIN, HIGH);
    vTaskDelay(1500); // Sáng 1.5s

    digitalWrite(LED_PIN, LOW);
    vTaskDelay(500); // Tắt 0.5s
  }
}

void task_neo_blinky(void *pvParameters) {
  Adafruit_NeoPixel strip(LED_COUNT, NEO_PIN, NEO_GRB + NEO_KHZ800);
  strip.begin();
  strip.clear();
  strip.show();

  while (1) {
    strip.setPixelColor(0, strip.Color(255, 0, 0)); // ĐỎ 1s
    strip.show();
    vTaskDelay(1000);

    strip.setPixelColor(0, strip.Color(0, 255, 0)); // XANH LÁ 2s
    strip.show();
    vTaskDelay(2000);

    strip.setPixelColor(0, strip.Color(0, 0, 255)); // XANH DƯƠNG 2s
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

// ============================================================================
// BÀI TẬP 4: Chuỗi màu NeoPixel nâng cao (Cầu vồng 5 màu, mỗi màu 3s)
// ============================================================================
#elif (EXERCISE == 4)

// Cấu trúc RGB Color
struct RGBColor {
  uint8_t r, g, b;
};

// Mảng màu cầu vồng
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

// Tác vụ 2: Chuyển 5 màu, mỗi màu giữ 3 giây (3000 ms)
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

    vTaskDelay(3000); // 3 giây

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
  xTaskCreate(task_neo_rainbow,   // Hàm tác vụ
              "Task_NEO_Rainbow", // Tên tác vụ
              2048,               // Stack depth
              NULL,               // Tham số
              1,                  // Độ ưu tiên
              NULL                // Task handle
  );
}

void loop() {}

// ============================================================================
// BÀI TẬP 5: Mô phỏng hệ thống đèn giao thông (ĐỎ 7s -> VÀNG 2s -> XANH 5s)
// ============================================================================
#elif (EXERCISE == 5)

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

    digitalWrite(LED_PIN, LOW); // Heartbeat TẮT 1s
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

#endif
