#ifndef __LED_CONTROL_H__
#define __LED_CONTROL_H__

#include <Arduino.h>
#include "global.h"

// Tác vụ điều khiển LED đơn nhấp nháy theo độ ẩm và cảnh báo lỗi
void task_led(void *pvParameters);

#endif // __LED_CONTROL_H__
