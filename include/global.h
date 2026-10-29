#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <Arduino.h>

// Giá trị cảm biến tức thời
extern float glob_temperature;
extern float glob_humidity;

// Cấu hình Wi-Fi SoftAP của ESP32
extern String ssid;
extern String password;

// Cấu hình Wi-Fi router nhận từ form cài đặt
extern String wifi_ssid;
extern String wifi_password;

// Trạng thái mạng và công cụ đồng bộ hóa FreeRTOS
extern bool isWifiConnected;
extern SemaphoreHandle_t xBinarySemaphoreInternet; // Báo Internet sẵn sàng (Bài tập 5)
extern SemaphoreHandle_t xSensorMutex;             // Khóa bảo vệ bộ đệm cảm biến (Bài tập 1)

// Ngưỡng nhiệt độ (Bài tập 2)
#define T_CAUTION   30.0f
#define T_WARNING   35.0f
#define T_DANGER    40.0f

// Ngưỡng độ ẩm & dải trễ Hysteresis 1% (Bài tập 3)
#define H_DRY       30.0f
#define H_HUMID     60.0f
#define H_DANGER    70.0f
#define H_HYST       1.0f

const char* tempLevel(float t);
const char* humLevel(float h);

#endif // __GLOBAL_H__
