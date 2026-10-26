# LAB 3: GIAO TIẾP CẢM BIẾN I2C VÀ HIỂN THỊ TRÊN ESP32 (DHT20 & LCD 1602)

Khoa Khoa học và Kỹ thuật Máy tính - Trường Đại học Bách Khoa, ĐHQG-HCM  
Môn học: Internet vạn vật (CO3061 / CO3062)  
Nhánh Git: `lab3_dht20_lcd` (hoặc `lab3`)  
Nền tảng: Kit Yolo:UNO (ESP32-S3 WROOM-1, Dual-core Xtensa LX7 @ 240 MHz)  
Môi trường: PlatformIO IDE (Framework Arduino for ESP32 & FreeRTOS)  

---

## 1. Mục tiêu thí nghiệm

1. Nắm vững nguyên lý truyền thông nối tiếp đồng bộ I2C (Inter-Integrated Circuit):
   - Cơ chế 2 dây: Dây dữ liệu nối tiếp (SDA) và Dây xung nhịp (SCL) với điện trở kéo lên (Pull-up resistors).
   - Định địa chỉ thiết bị 7-bit trên bus: Cảm biến DHT20 (`0x38`), Màn hình LCD 1602 I2C qua chip PCF8574 (`0x27` hoặc `0x3F`).
   - Khung truyền I2C: Tín hiệu START, địa chỉ thiết bị kèm bit R/W, bit xác nhận ACK/NACK, các byte dữ liệu và tín hiệu STOP.
2. Thu thập thông số môi trường thời gian thực từ cảm biến kỹ thuật số DHT20:
   - Đo nhiệt độ chính xác (°C) và độ ẩm tương đối (RH%).
   - Tính toán mã kiểm tra lỗi (Checksum verification) để loại bỏ các giá trị đọc lỗi hoặc nhiễu đường truyền.
3. Hiển thị trực quan và cảnh báo đa tầng:
   - Chỉ thị dải nhiệt độ bằng màu sắc trên đèn NeoPixel RGB.
   - Điều biến tần số chớp tắt của LED đơn theo mức độ ẩm môi trường.
   - Hiển thị thông số thời gian thực lên màn hình ký tự LCD 1602.
4. Xây dựng máy trạng thái hữu hạn (FSM) xử lý sự cố bảo an hệ thống (Fail-Safe Mechanism) khi mất kết nối cảm biến đột ngột.

---

## 2. Phân bố chân phần cứng và Địa chỉ I2C (Hardware Configuration)

### 2.1. Bảng đấu nối ngoại vi trên Yolo:UNO
Bo mạch Yolo:UNO định nghĩa các chân I2C mặc định cho ESP32-S3:

| Ngoại vi / Tín hiệu | Chân GPIO ESP32-S3 | Chức năng kỹ thuật |
| :--- | :--- | :--- |
| I2C Serial Data (SDA) | `GPIO 11` | Đường truyền dữ liệu hai chiều I2C |
| I2C Serial Clock (SCL) | `GPIO 12` | Đường truyền xung nhịp đồng bộ (100 kHz) |
| Onboard NeoPixel RGB | `GPIO 6` | Chỉ thị màu sắc cảnh báo nhiệt độ |
| Onboard User LED | `GPIO 13` | Chỉ thị nhịp nhấp nháy theo độ ẩm |
| Console UART | `USB-C CDC` | Theo dõi nhật ký Serial (115200 bps) |

### 2.2. Bảng tra cứu địa chỉ I2C (I2C Address Map)
- **Cảm biến DHT20:** Địa chỉ cố định `0x38`.
- **Màn hình LCD 1602 (I2C Backpack PCF8574):** Địa chỉ tiêu chuẩn `0x27` (hoặc `0x3F` tùy lô sản xuất phần cứng).

---

## 3. Danh mục các bài tập thực hành và Mô tả chi tiết

### Bài 1: Đọc cảm biến DHT20 và NeoPixel chỉ thị dải giá trị nhiệt độ
- **Mục tiêu:** Thu thập dữ liệu nhiệt độ và chuyển đổi thành tín hiệu cảnh báo thị giác.
- **Quy chuẩn phân dải nhiệt độ và mã màu NeoPixel:**

| Dải nhiệt độ đo được | Trạng thái môi trường | Màu sắc NeoPixel | Mã màu RGB |
| :--- | :--- | :--- | :--- |
| Nhiệt độ < 25.0°C | Mát mẻ (Cool) | XANH DƯƠNG (Blue) | `(0, 0, 255)` |
| 25.0°C <= Nhiệt độ <= 32.0°C | Lý tưởng (Comfort) | XANH LÁ (Green) | `(0, 255, 0)` |
| Nhiệt độ > 32.0°C | Nóng / Cảnh báo (Hot) | ĐỎ (Red) | `(255, 0, 0)` |

- **Yêu cầu kỹ thuật:** Chu kỳ lấy mẫu cảm biến là 1000 ms. Dữ liệu được in định kỳ ra Serial Monitor để đối chiếu với màu đèn thực tế trên bo mạch.

### Bài 2: Điều biến tần số nhấp nháy LED theo độ ẩm môi trường
- **Mục tiêu:** Chuyển đổi giá trị độ ẩm thành tần số nhấp nháy của User LED13.
- **Quy chuẩn chu kỳ và tần số nhấp nháy:**

| Dải độ ẩm môi trường (RH) | Trạng thái độ ẩm | Chu kỳ nhấp nháy LED13 | Thời gian Sáng / Tắt |
| :--- | :--- | :--- | :--- |
| Độ ẩm < 50.0% | Khô hạn (Dry) | **2000 ms (Chậm)** | Sáng 1000 ms / Tắt 1000 ms |
| 50.0% <= Độ ẩm <= 75.0% | Lý tưởng (Normal) | **1000 ms (Vừa)** | Sáng 500 ms / Tắt 500 ms |
| Độ ẩm > 75.0% | Ẩm ướt (Humid) | **400 ms (Nhanh)** | Sáng 200 ms / Tắt 200 ms |

- **Kiểm chứng:** Khi phả hơi ẩm vào cảm biến, độ ẩm vượt 75%, LED lập tức tăng tốc độ chớp tắt rõ rệt mà không làm gián đoạn chu trình kiểm tra nhiệt độ của NeoPixel ở Bài 1.

### Bài 3: Xử lý sự cố ngắt kết nối cảm biến và cơ chế bảo an (Fail-Safe FSM)
- **Mục tiêu:** Xây dựng tính năng tin cậy công nghiệp, tự động phát hiện và xử lý khi cảm biến bị hỏng hoặc tuột cáp I2C.
- **Thiết kế Máy trạng thái hữu hạn (FSM):**
  - **Trạng thái NORMAL (Bình thường):** Cảm biến phản hồi mã thành công `DHT20_OK` (0). Hệ thống vận hành bình thường như Bài 1 và Bài 2.
  - **Trạng thái FAIL_SAFE (Sự cố):** Khi lệnh đọc cảm biến trả về mã lỗi (mất kết nối hoặc lỗi truyền tin I2C):
    - Đèn NeoPixel lập tức chuyển sang chế độ **chớp tắt liên tục màu VÀNG / ĐỎ** cảnh báo nguy hiểm.
    - User LED13 nhấp nháy tần số cực nhanh (100 ms) báo hiệu lỗi phần cứng.
    - Xuất thông điệp cảnh báo khẩn cấp `[ERROR] DHT20 Disconnected! Entering Fail-Safe mode` ra cổng Serial.
    - Khi cắm cảm biến trở lại, FSM tự động khôi phục về trạng thái NORMAL mà không cần khởi động lại vi điều khiển.

### Bài 4 & 5: Hiển thị thông số lên Màn hình LCD 1602 I2C
- **Mục tiêu:** Tích hợp màn hình LCD 1602 để tạo giao diện người - máy (HMI) độc lập tại hiện trường.
- **Định dạng hiển thị:**
  - Dòng 1: `Temp: xx.x C [STATUS]`
  - Dòng 2: `Humi: yy.y % [STATUS]`
  - Khi ở chế độ Fail-Safe: Dòng 2 hiển thị thông báo `SENSOR ERROR!` để nhân viên vận hành nhận diện chính xác sự cố.

---

## 4. Hướng dẫn Biên dịch và Nạp chương trình

Từ thư mục gốc của nhánh trên PlatformIO:

```bash
# Biên dịch chương trình
pio run

# Nạp vào kit Yolo:UNO
pio run --target upload

# Giám sát Serial Monitor
pio device monitor -b 115200
```

---

## 5. Quy tắc kỹ thuật và Lưu ý về Bus I2C

1. **Trở kéo lên (Pull-up Resistors):** Dây SDA và SCL bắt buộc phải có điện trở kéo lên nguồn 3.3V (thường từ 4.7kΩ đến 10kΩ). Trên kit Yolo:UNO và module DHT20 đã tích hợp sẵn trở kéo.
2. **Kiểm tra kết nối bằng I2C Scanner:** Nếu không đọc được dữ liệu, cần nạp chương trình I2C Scanner để xác nhận đúng địa chỉ `0x38` của DHT20 và `0x27` của LCD trên bus trước khi nạp chương trình chính.
3. **Thời gian khởi động DHT20:** Sau khi cấp nguồn, cảm biến DHT20 cần tối thiểu 100 ms để ổn định điện áp trước khi thực hiện lệnh khởi tạo `Wire.begin()` và `dht20.begin()`.
