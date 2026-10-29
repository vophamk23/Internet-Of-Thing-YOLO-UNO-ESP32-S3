#pragma once
#include <Arduino.h>

#define AVG_N 10

// Quản lý bộ đệm trượt 10 mẫu có Mutex bảo vệ (Bài tập 1)
void addSample(float t, float h);
bool getAverage(float *t_avg, float *h_avg);

// Task thu thập dữ liệu cảm biến định kỳ 2 giây
void sensor_task(void *pvParameters);
