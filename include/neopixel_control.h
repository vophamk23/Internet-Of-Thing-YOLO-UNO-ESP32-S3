#ifndef __NEOPIXEL_CONTROL_H__
#define __NEOPIXEL_CONTROL_H__

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "global.h"

// Tác vụ điều khiển LED NeoPixel theo nhiệt độ và cảnh báo lỗi
void task_neopixel(void *pvParameters);

#endif // __NEOPIXEL_CONTROL_H__
