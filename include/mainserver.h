#pragma once
#include <Arduino.h>
#include <WebServer.h>
#include <WiFi.h>

// =============================================================
// Cấu hình chân linh hoạt tự động theo loại chip:
// - Bo YoloUNO (ESP32-S3): LED1 = GPIO 48, LED2 = GPIO 41, BOOT = GPIO 0
// - Bo ESP32 thường:       LED1 = GPIO 2,  LED2 = GPIO 4,  BOOT = GPIO 0
// =============================================================
#if defined(CONFIG_IDF_TARGET_ESP32S3)
#ifndef LED1_PIN
#define LED1_PIN 48 // YoloUNO LED1
#endif
#ifndef LED2_PIN
#define LED2_PIN 41 // YoloUNO LED2
#endif
#else
#ifndef LED1_PIN
#define LED1_PIN 2 // ESP32 thường LED1 (On-board LED)
#endif
#ifndef LED2_PIN
#define LED2_PIN 4 // ESP32 thường LED2
#endif
#endif

#ifndef BOOT_PIN
#define BOOT_PIN 0 // Nút nhấn BOOT
#endif

extern WebServer server;
extern bool isAPMode;
extern bool connecting;
extern uint32_t connect_start_ms;
extern bool led1_state;
extern bool led2_state;

void setupServer();
void startAP();
void connectToWiFi();
void main_server_task(void *pvParameters);
