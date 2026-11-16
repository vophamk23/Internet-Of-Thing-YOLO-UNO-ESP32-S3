#include "sensor_task.h"
#include "global.h"
#include <DHT20.h>
#include <WiFi.h>
#include <Wire.h>

#define SDA_PIN 11
#define SCL_PIN 12

static DHT20 dht20;

// Bộ nhớ đệm tròn (Circular buffer) 10 phần tử
static float temp_buf[AVG_N];
static float hum_buf[AVG_N];
static int buf_idx = 0;
static int buf_count = 0;

// Thêm mẫu cảm biến mới vào bộ đệm tròn (Bảo vệ bằng Mutex)
void addSample(float t, float h) {
  if (xSensorMutex == NULL)
    return;

  if (xSemaphoreTake(xSensorMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
    temp_buf[buf_idx] = t;
    hum_buf[buf_idx] = h;

    buf_idx = (buf_idx + 1) % AVG_N;
    if (buf_count < AVG_N) {
      buf_count++;
    }

    xSemaphoreGive(xSensorMutex);
  }
}

// Tính trung bình trượt của các mẫu hiện có (Bảo vệ bằng Mutex)
bool getAverage(float *t_avg, float *h_avg) {
  if (xSensorMutex == NULL)
    return false;

  if (xSemaphoreTake(xSensorMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
    if (buf_count == 0) {
      xSemaphoreGive(xSensorMutex);
      return false;
    }

    float sum_t = 0.0f;
    float sum_h = 0.0f;
    for (int i = 0; i < buf_count; i++) {
      sum_t += temp_buf[i];
      sum_h += hum_buf[i];
    }

    *t_avg = sum_t / buf_count;
    *h_avg = sum_h / buf_count;

    xSemaphoreGive(xSensorMutex);
    return true;
  }
  return false;
}

// =============================================================
// Task: Thu thập dữ liệu cảm biến định kỳ & Lọc trung bình trượt (Bài tập 1)
// Chức năng: Đọc mẫu mỗi 2 giây, ghi vào circular buffer được khóa bằng Mutex
// =============================================================
void sensor_task(void *pvParameters) {
  Wire.begin(SDA_PIN, SCL_PIN);
  dht20.begin();
  int sample_count = 0;

  while (1) {
    // 1. Đọc cảm biến thực tế DHT20 (tự động fallback giả lập nếu chưa cắm)
    if (dht20.read() == 0) {
      glob_temperature = dht20.getTemperature();
      glob_humidity = dht20.getHumidity();
    } else {
      // Dữ liệu mô phỏng theo dải môi trường phòng thí nghiệm
      glob_temperature = 28.0f + (random(0, 300) / 100.0f); // 28.00 .. 30.99 °C
      glob_humidity = 60.0f + (random(0, 500) / 100.0f);    // 60.00 .. 64.99 %
    }

    // 2. Ghi mẫu vào bộ đệm tròn 10 phần tử
    addSample(glob_temperature, glob_humidity);
    sample_count++;

    // 3. In log đối chiếu giữa giá trị thô (Raw) và trung bình trượt (Avg 10)
    float t_avg = 0.0f, h_avg = 0.0f;
    if (getAverage(&t_avg, &h_avg)) {
        Serial.printf("[SAMPLE #%02d] Raw: T=%.2f C, H=%.2f %% | Avg(10): T=%.2f C, H=%.2f %%\n",
                      sample_count, glob_temperature, glob_humidity, t_avg, h_avg);
    }

    // Hiển thị rõ ràng tên Wi-Fi AP, Mật khẩu, IP và API Token ngay cả khi chưa kết nối Internet
    if (isAPMode) {
        if (sample_count <= 2 || sample_count % 5 == 0 || Serial.available()) {
            while (Serial.available()) Serial.read(); // Xóa bộ đệm phím
            Serial.printf(" >> [AP MODE] Wi-Fi: \"%s\" | Pass: \"%s\" | IP: http://192.168.4.1 | CoreIOT Token: %s\n",
                          ssid.c_str(), password.c_str(), COREIOT_TOKEN);
        }
    } else {
        if (sample_count % 5 == 0 || Serial.available()) {
            while (Serial.available()) Serial.read();
            Serial.printf(" >> [STA MODE] IP: http://%s | Broker: %s | CoreIOT Token: %s\n",
                          WiFi.localIP().toString().c_str(), COREIOT_HOST, COREIOT_TOKEN);
        }
    }

      // 4. Chu kỳ lấy mẫu 2 giây (tạo cửa sổ trượt 20 giây cho 10 mẫu)
      vTaskDelay(pdMS_TO_TICKS(2000));
    }
  }
