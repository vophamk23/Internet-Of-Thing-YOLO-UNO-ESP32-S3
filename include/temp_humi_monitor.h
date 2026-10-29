#ifndef __TEMP_HUMI_MONITOR_H__
#define __TEMP_HUMI_MONITOR_H__

#include <Arduino.h>
#include <Wire.h>
#include "DHT20.h"
#include "LiquidCrystal_I2C.h"
#include "global.h"

// Khai báo các hàm tác vụ cảm biến cho từng bài tập
void task_temp_humi_monitor(void *pvParameters);  // Cho Bài 1, 2, 3
void task_temp_humi_filter(void *pvParameters);   // Cho Bài 4 (Average Filter)
void task_advanced_filter(void *pvParameters);    // Cho Bài 5 (Median + EMA Filter)

#endif // __TEMP_HUMI_MONITOR_H__
