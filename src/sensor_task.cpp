#include "sensor_task.h"
#include "global.h"

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
  int sample_count = 0;

  while (1) {
    // 1. Giả lập dữ liệu cảm biến môi trường (hoặc đọc từ DHT20)
    glob_temperature = 28.0f + (random(0, 300) / 100.0f); // 28.00 .. 30.99 °C
    glob_humidity = 60.0f + (random(0, 500) / 100.0f);    // 60.00 .. 64.99 %

    // glob_temperature = dht20.getTemperature();
    // glob_humidity = dht20.getHumidity();

    // 2. Ghi mẫu vào bộ đệm tròn 10 phần tử
    addSample(glob_temperature, glob_humidity);
    sample_count++;

    // 3. In log đối chiếu giữa giá trị thô (Raw) và trung bình trượt (Avg 10)
    float t_avg = 0.0f, h_avg = 0.0f;
    if (getAverage(&t_avg, &h_avg)) {
      Serial.printf("[SAMPLE #%02d] Raw: T=%.2f C, H=%.2f %% | Avg(10): T=%.2f C, H=%.2f %%\n",
                    sample_count, glob_temperature, glob_humidity, t_avg, h_avg);
    }

    // 4. Chu kỳ lấy mẫu 2 giây (tạo cửa sổ trượt 20 giây cho 10 mẫu)
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}
