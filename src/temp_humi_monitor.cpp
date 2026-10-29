#include "temp_humi_monitor.h"

// Khởi tạo các đối tượng điều khiển ngoại vi I2C cục bộ trong mô-đun
static DHT20 dht20;
static LiquidCrystal_I2C lcd(LCD_ADDR, 16, 2);

// ============================================================================
// TÁC VỤ 1: Đọc cảm biến tiêu chuẩn & Cập nhật LCD (Dùng cho Bài 1, 2, 3)
// ============================================================================
void task_temp_humi_monitor(void *pvParameters) {
  Wire.begin(SDA_PIN, SCL_PIN);
  dht20.begin();

  lcd.begin();
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("DHT20 & LCD 16x2");
  vTaskDelay(1000);
  lcd.clear();

  // Vòng lặp giám sát định kỳ
  while (1) {
    int status = dht20.read();
    float t = dht20.getTemperature();
    float h = dht20.getHumidity();

    // Kiểm tra điều kiện lỗi:
    //    - status != 0: Lỗi giao tiếp I2C
    //    - isnan(t) hoặc isnan(h): Giá trị trả về không phải số hợp lệ
    //    - (t == 0.0 && h == 0.0): Mô phỏng ngắt kết nối theo yêu cầu Bài 3
    if (status != 0 || isnan(t) || isnan(h) || (t == 0.0 && h == 0.0)) {

      glob_sensor_error = true;
      glob_temperature = 0.0;
      glob_humidity = 0.0;
      Serial.println("[CANH BAO] Cam bien DHT20 bi loi hoac mat ket noi!");

      // Báo lỗi trực quan trên màn hình LCD
      lcd.setCursor(0, 0);
      lcd.print("! SENSOR ERROR !");
      lcd.setCursor(0, 1);
      lcd.print("Check DHT20 I2C ");

    } else {

      glob_sensor_error = false;

      // Cập nhật giá trị vào biến dùng chung để các tác vụ khác sử dụng
      glob_temperature = t;
      glob_humidity = h;

      // In kết quả ra cổng Serial Monitor
      Serial.print("[DHT20] Nhiet do: ");
      Serial.print(glob_temperature, 1);
      Serial.print(" *C | Do am: ");
      Serial.print(glob_humidity, 1);
      Serial.println(" %");

      // Cập nhật dòng 1 trên LCD: Hiển thị Nhiệt độ
      lcd.setCursor(0, 0);
      lcd.print("Temp: ");
      lcd.print(glob_temperature, 1);
      lcd.print((char)223); // Ký tự độ ° trong bảng mã LCD
      lcd.print("C   ");

      // Cập nhật dòng 2 trên LCD: Hiển thị Độ ẩm
      lcd.setCursor(0, 1);
      lcd.print("Humi: ");
      lcd.print(glob_humidity, 1);
      lcd.print("%   ");
    }

    // Nhường CPU cho các tác vụ khác trong 5 giây (Chu kỳ lấy mẫu)
    vTaskDelay(5000);
  }
}

// ============================================================================
// TÁC VỤ 2: Bộ lọc trung bình (Moving Average Filter) - Dùng cho Bài 4
// ============================================================================
void task_temp_humi_filter(void *pvParameters) {

  Wire.begin(SDA_PIN, SCL_PIN);
  dht20.begin();

  lcd.begin();
  lcd.backlight();
  lcd.clear();

  // Định nghĩa các tham số của bộ lọc trung bình
  const int NUM_SAMPLES = 5;            // Số mẫu thu thập trong 1 đợt
  const float THRESHOLD_TEMP_VAR = 1.5; // Ngưỡng biến thiên nhiệt độ: 1.5°C
  const float THRESHOLD_HUMI_VAR = 3.0; // Ngưỡng biến thiên độ ẩm: 3.0%

  while (1) {

    float temp_samples[NUM_SAMPLES];
    float humi_samples[NUM_SAMPLES];
    int valid_count = 0;

    // 1. Thu thập 5 mẫu liên tiếp, mỗi mẫu cách nhau 100ms
    for (int i = 0; i < NUM_SAMPLES; i++) {

      if (dht20.read() == 0) {
        temp_samples[valid_count] = dht20.getTemperature();
        humi_samples[valid_count] = dht20.getHumidity();
        valid_count++;
      }

      vTaskDelay(100); // Khoảng cách giữa 2 lần lấy mẫu
    }

    // 2. Khi đã thu thập đủ 5 mẫu hợp lệ, tiến hành xử lý thống kê
    if (valid_count == NUM_SAMPLES) {

      float min_t = temp_samples[0], max_t = temp_samples[0], sum_t = 0;
      float min_h = humi_samples[0], max_h = humi_samples[0], sum_h = 0;

      for (int i = 0; i < NUM_SAMPLES; i++) {

        // Tìm giá trị min/max nhiệt độ
        if (temp_samples[i] < min_t)
          min_t = temp_samples[i];
        if (temp_samples[i] > max_t)
          max_t = temp_samples[i];
        sum_t += temp_samples[i];

        // Tìm giá trị min/max độ ẩm
        if (humi_samples[i] < min_h)
          min_h = humi_samples[i];
        if (humi_samples[i] > max_h)
          max_h = humi_samples[i];
        sum_h += humi_samples[i];
      }

      // Tính độ biến thiên đỉnh-đỉnh (Peak-to-Peak: Delta = Max - Min)
      float delta_temp = max_t - min_t;
      float delta_humi = max_h - min_h;

      // 3. So sánh với ngưỡng cho phép:
      //    Nếu độ biến thiên nhỏ hơn ngưỡng -> Tín hiệu ổn định, tính giá trị
      //    trung bình
      if (delta_temp < THRESHOLD_TEMP_VAR && delta_humi < THRESHOLD_HUMI_VAR) {

        glob_sensor_error = false;
        glob_temperature = sum_t / NUM_SAMPLES;
        glob_humidity = sum_h / NUM_SAMPLES;

        Serial.print("[FILTER OK] T_avg: ");
        Serial.print(glob_temperature, 2);
        Serial.print(" *C | H_avg: ");
        Serial.print(glob_humidity, 2);
        Serial.println(" %");

        // Hiển thị giá trị trung bình đã lọc lên màn hình LCD
        lcd.setCursor(0, 0);
        lcd.print("AvgT: ");
        lcd.print(glob_temperature, 1);
        lcd.print((char)223);
        lcd.print("C   ");

        lcd.setCursor(0, 1);
        lcd.print("AvgH: ");
        lcd.print(glob_humidity, 1);
        lcd.print("%   ");

      } else {

        // Nếu biến thiên quá lớn -> Nhiễu xung hoặc rung động, loại bỏ mẫu này
        Serial.print("[NOISE REJECTED] Delta T: ");
        Serial.print(delta_temp, 2);
        Serial.print(" | Delta H: ");
        Serial.println(delta_humi, 2);
      }

    } else {

      glob_sensor_error = true;
    }

    // Nghỉ 4.5 giây để tổng chu kỳ đọc xấp xỉ 5 giây
    vTaskDelay(4500);
  }
}

// ============================================================================
// TÁC VỤ 3: Bộ lọc nâng cao (Median Filter + EMA) - Dùng cho Bài 5
// ============================================================================

// Hàm tính giá trị trung vị (Median) của mảng 5 phần tử
static float compute_median_5(float arr[5]) {

  float sorted[5];
  for (int i = 0; i < 5; i++)
    sorted[i] = arr[i];

  // Sắp xếp nổi bọt (Bubble Sort) đơn giản
  for (int i = 0; i < 4; i++) {
    for (int j = i + 1; j < 5; j++) {
      if (sorted[i] > sorted[j]) {
        float temp = sorted[i];
        sorted[i] = sorted[j];
        sorted[j] = temp;
      }
    }
  }

  // Trả về phần tử nằm chính giữa (phần tử thứ 3, index = 2)
  return sorted[2];
}

void task_advanced_filter(void *pvParameters) {

  Wire.begin(SDA_PIN, SCL_PIN);
  dht20.begin();

  lcd.begin();
  lcd.backlight();
  lcd.clear();

  // Tham số hệ số làm mượt mũ (Exponential Moving Average)
  const float EMA_ALPHA = 0.3f; // 0 < alpha <= 1 (alpha càng nhỏ càng mượt)
  float ema_temp = 0.0;
  float ema_humi = 0.0;
  bool is_first_run = true;

  while (1) {

    float raw_temps[5];
    float raw_humis[5];
    int valid_count = 0;

    // 1. Thu thập 5 mẫu cách nhau 100ms
    for (int i = 0; i < 5; i++) {

      if (dht20.read() == 0) {
        raw_temps[valid_count] = dht20.getTemperature();
        raw_humis[valid_count] = dht20.getHumidity();
        valid_count++;
      }

      vTaskDelay(100);
    }

    if (valid_count == 5) {

      // TẦNG 1: Lọc trung vị (Median Filter)
      // Loại bỏ 100% mẫu dị biệt (Spike noise) ở 2 đầu cực trị
      float med_temp = compute_median_5(raw_temps);
      float med_humi = compute_median_5(raw_humis);

      // TẦNG 2: Lọc làm mượt hàm mũ (EMA): y[n] = alpha * x[n] + (1 - alpha) *
      // y[n-1]
      if (is_first_run) {

        ema_temp = med_temp;
        ema_humi = med_humi;
        is_first_run = false;

      } else {

        ema_temp = EMA_ALPHA * med_temp + (1.0f - EMA_ALPHA) * ema_temp;
        ema_humi = EMA_ALPHA * med_humi + (1.0f - EMA_ALPHA) * ema_humi;
      }

      glob_sensor_error = false;
      glob_temperature = ema_temp;
      glob_humidity = ema_humi;

      Serial.print("[MEDIAN+EMA] Out_T: ");
      Serial.print(glob_temperature, 2);
      Serial.print(" *C | Out_H: ");
      Serial.print(glob_humidity, 2);
      Serial.println(" %");

      // Cập nhật LCD
      lcd.setCursor(0, 0);
      lcd.print("AdvT: ");
      lcd.print(glob_temperature, 1);
      lcd.print((char)223);
      lcd.print("C   ");

      lcd.setCursor(0, 1);
      lcd.print("AdvH: ");
      lcd.print(glob_humidity, 1);
      lcd.print("%   ");

    } else {

      glob_sensor_error = true;
    }

    // Nghỉ 4.5 giây
    vTaskDelay(4500);
  }
}
