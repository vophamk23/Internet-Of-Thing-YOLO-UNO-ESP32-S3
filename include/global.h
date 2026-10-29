#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// Định nghĩa các chân phần cứng YoloUNO (ESP32-S3)
#define LED_PIN GPIO_NUM_48 // LED đơn on-board (GPIO 48)
#define NEO_PIN 45          // LED NeoPixel RGB on-board (GPIO 45)
#define LED_COUNT 1         // 1 hạt LED NeoPixel
#define SDA_PIN 11          // I2C SDA (Cổng ĐỎ)
#define SCL_PIN 12          // I2C SCL (Cổng ĐỎ)
#define LCD_ADDR 33         // Địa chỉ I2C LCD 16x2 (thập phân 33 = 0x21)

// Khai báo các biến toàn cục chia sẻ giữa các tác vụ (Inter-Task Communication)
extern volatile float glob_temperature;
extern volatile float glob_humidity;
extern volatile bool glob_sensor_error;

#endif // __GLOBAL_H__
