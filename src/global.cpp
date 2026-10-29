#include "global.h"

// =============================================================
// Định nghĩa biến toàn cục lưu số liệu cảm biến (sensor_task ghi vào)
// =============================================================
float glob_temperature = 0.0f;
float glob_humidity = 0.0f;

// Cấu hình Wi-Fi Access Point (Tên nhóm & mật khẩu tối thiểu 8 ký tự)
String ssid = "ESP32-TEAM01";
String password = "Lab4_2313946";

// Thông tin Wi-Fi router nhà nhận từ form cài đặt (/settings)
String wifi_ssid = "";
String wifi_password = "";

// Cờ báo trạng thái mạng và các công cụ đồng bộ hóa FreeRTOS
bool isWifiConnected = false;
SemaphoreHandle_t xBinarySemaphoreInternet = NULL; // Báo hiệu khi có Internet
SemaphoreHandle_t xSensorMutex = NULL;             // Bảo vệ bộ nhớ đệm cảm biến

// =============================================================
// Hàm đánh giá mức nhiệt độ (Bài tập 2 & Bài tập 5)
// Trả về chuỗi: "normal", "caution", "warning", "danger"
// =============================================================
const char *tempLevel(float t) {
  if (t >= T_DANGER)
    return "danger"; // >= 40 °C: Nguy hiểm
  if (t >= T_WARNING)
    return "warning"; // >= 35 °C: Cảnh báo
  if (t >= T_CAUTION)
    return "caution"; // >= 30 °C: Chú ý
  return "normal";    // < 30 °C: Bình thường
}

// =============================================================
// Hàm đánh giá mức độ ẩm kèm dải trễ 1% Hysteresis (Bài tập 3)
// Chống hiện tượng chập chờn/nhấp nháy trạng thái quanh ngưỡng biên
// =============================================================
enum HumState { STATE_DRY, STATE_OK, STATE_HUMID, STATE_DANGER };
static HumState current_hum_state = STATE_OK;
static bool is_hum_initialized = false;

const char *humLevel(float h) {
  // 1. Khởi tạo đúng trạng thái thực tế ngay lần đọc đầu tiên
  if (!is_hum_initialized) {
    if (h >= H_DANGER)
      current_hum_state = STATE_DANGER;
    else if (h >= H_HUMID)
      current_hum_state = STATE_HUMID;
    else if (h < H_DRY)
      current_hum_state = STATE_DRY;
    else
      current_hum_state = STATE_OK;
    is_hum_initialized = true;
  }

  HumState prev_state = current_hum_state;

  // 2. Chuyển đổi trạng thái với dải trễ Hysteresis 1% (Cách B chuẩn xác: >=
  // 30% luôn là OK)
  switch (current_hum_state) {
  case STATE_DRY:
    // Đang ở DRY: Chạm mốc >= 30.0% là lập tức trở về mức OK bình thường
    if (h >= H_DRY) {
      if (h >= H_DANGER)
        current_hum_state = STATE_DANGER;
      else if (h >= H_HUMID)
        current_hum_state = STATE_HUMID;
      else
        current_hum_state = STATE_OK;
    }
    break;

  case STATE_OK:
    // Đang OK: Phải tụt hẳn dưới 29.0% (30 - 1) mới báo Khô; vượt >= 60.0% báo
    // Ẩm
    if (h < H_DRY - H_HYST) {
      current_hum_state = STATE_DRY;
    } else if (h >= H_DANGER) {
      current_hum_state = STATE_DANGER;
    } else if (h >= H_HUMID) {
      current_hum_state = STATE_HUMID;
    }
    break;

  case STATE_HUMID:
    // Đang ẨM: Vượt >= 70.0% báo Nguy hiểm; tụt dưới < 59.0% (60 - 1) mới quay
    // về OK
    if (h >= H_DANGER) {
      current_hum_state = STATE_DANGER;
    } else if (h < H_HUMID - H_HYST) {
      if (h < H_DRY - H_HYST)
        current_hum_state = STATE_DRY;
      else
        current_hum_state = STATE_OK;
    }
    break;

  case STATE_DANGER:
    // Đang NGUY HIỂM: Phải tụt dưới < 69.0% (70 - 1) mới hạ mức
    if (h < H_DANGER - H_HYST) {
      if (h < H_DRY - H_HYST)
        current_hum_state = STATE_DRY;
      else if (h < H_HUMID)
        current_hum_state = STATE_OK; // < 60% là OK chuẩn xác
      else
        current_hum_state = STATE_HUMID; // 60% .. 68.9% là ẨM
    }
    break;
  }

  // 3. Cơ chế chốt log: CHỈ in ra Serial đúng định dạng đề bài khi có sự đổi
  // mức
  if (current_hum_state != prev_state) {
    auto getStateName = [](HumState s) -> const char * {
      switch (s) {
      case STATE_DRY:
        return "dry";
      case STATE_OK:
        return "ok";
      case STATE_HUMID:
        return "humid";
      case STATE_DANGER:
        return "danger";
      default:
        return "ok";
      }
    };
    Serial.printf("[HUM] level %s -> %s (%.1f %%)\n", getStateName(prev_state),
                  getStateName(current_hum_state), h);
  }

  // 4. Trả về tên mức cho Web Server và MQTT
  switch (current_hum_state) {
  case STATE_DRY:
    return "dry";
  case STATE_OK:
    return "ok";
  case STATE_HUMID:
    return "humid";
  case STATE_DANGER:
    return "danger";
  default:
    return "ok";
  }
}
