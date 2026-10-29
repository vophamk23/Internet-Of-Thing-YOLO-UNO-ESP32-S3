#include "global.h"

// ============================================================================
// ĐỊNH NGHĨA CÁC BIẾN TOÀN CỤC CHIA SẺ GIỮA CÁC TÁC VỤ (FREE RTOS)
// ============================================================================

// Biến lưu trữ nhiệt độ đo được từ cảm biến DHT20 (Đơn vị: độ C)
volatile float glob_temperature = 0.0;

// Biến lưu trữ độ ẩm không khí đo được từ cảm biến DHT20 (Đơn vị: %)
volatile float glob_humidity = 0.0;

// Cờ báo trạng thái lỗi cảm biến:
//   - true: Cảm biến bị mất kết nối, lỗi đọc I2C, hoặc trả về giá trị không hợp lệ
//   - false: Cảm biến hoạt động bình thường
volatile bool glob_sensor_error = false;
