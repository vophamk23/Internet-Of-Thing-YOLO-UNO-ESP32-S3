#include <Arduino.h>

#define LED_PIN GPIO_NUM_48 // LED on-board YoloUNO (GPIO 48)

// Tác vụ 1: Nhấp nháy LED đơn (SÁNG 1 giây, TẮT 2 giây)
void task_led_blinky(void *pvParameters) {
  pinMode(LED_PIN, OUTPUT);
  while (1) {
    digitalWrite(LED_PIN, HIGH);
    vTaskDelay(1000); // Sáng 1 giây

    digitalWrite(LED_PIN, LOW);
    vTaskDelay(2000); // Tắt 2 giây
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
