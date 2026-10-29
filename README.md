# 📡 LAB 3: GIAO TIẾP NGOẠI VI I2C VÀ CẢNH BÁO ĐA TẦNG TRÊN ESP32
### Cảm biến nhiệt - ẩm DHT20, Màn hình LCD 1602 & Cảnh báo NeoPixel RGB

> **Trường Đại học Bách Khoa - ĐHQG-HCM**  
> **Khoa Khoa học và Kỹ thuật Máy tính**  
> **Môn học:** Internet vạn vật (CO3061 / CO3062)  
> **Sinh viên thực hiện:** Võ Phạm (MSSV: 2313946)  
> **Nhánh Git:** `lab3_dht20_lcd` | **Nền tảng:** Yolo:UNO (ESP32-S3 Dual-Core @ 240MHz)

---

## 📖 Mục lục
- [1. Giới thiệu & Mục tiêu](#1-giới-thiệu--mục-tiêu)
- [2. Cấu trúc thư mục dự án](#2-cấu-trúc-thư-mục-dự-án)
- [3. Sơ đồ kết nối phần cứng & Địa chỉ I2C](#3-sơ-đồ-kết-nối-phần-cứng--địa-chỉ-i2c)
- [4. Nội dung bài thực hành](#4-nội-dung-bài-thực-hành)
  - [Bài 1: Giám sát nhiệt độ & NeoPixel RGB](#bài-1-giám-sát-nhiệt-độ--neopixel-rgb)
  - [Bài 2: Điều biến tần số nhấp nháy LED theo độ ẩm](#bài-2-điều-biến-tần-số-nhấp-nháy-led-theo-độ-ẩm)
  - [Bài 3: Máy trạng thái hữu hạn xử lý sự cố (Fail-Safe FSM)](#bài-3-máy-trạng-thái-hữu-hạn-xử-lý-sự-cố-fail-safe-fsm)
  - [Bài 4 & 5: Hiển thị trạng thái HMI trên LCD 1602](#bài-4--5-hiển-thị-trạng-thái-hmi-trên-lcd-1602)
- [5. Hướng dẫn biên dịch & Nạp mã nguồn](#5-hướng-dẫn-biên-dịch--nạp-mã-nguồn)
- [6. Lưu ý kỹ thuật quan trọng](#6-lưu-ý-kỹ-thuật-quan-trọng)

---

## 🎯 1. Giới thiệu & Mục tiêu

Bài thí nghiệm Lab 3 tập trung vào kỹ thuật lập trình giao tiếp thời gian thực qua giao thức **I2C (Inter-Integrated Circuit)** trên vi điều khiển **ESP32-S3**, kết hợp mô hình kiến trúc phân tầng dạng module (*modular architecture*):

1. **Nguyên lý I2C:** Vận hành bus nối tiếp 2 dây (`SDA` / `SCL`), nhận diện các thiết bị thông qua địa chỉ 7-bit (`0x38` cho DHT20, `0x27` cho LCD 1602).
2. **Thu thập dữ liệu chính xác:** Đo đạc nhiệt độ (°C) và độ ẩm tương đối (%RH) từ cảm biến kỹ thuật số DHT20 kèm giải thuật kiểm tra Checksum (CRC).
3. **Cảnh báo thị giác đa tầng:**
   - Đèn thông minh NeoPixel (WS2812B) phản ánh 3 mức nhiệt độ môi trường.
   - User LED nhấp nháy với tần số tỷ lệ theo độ ẩm không khí.
   - Màn hình LCD 1602 hiển thị thông số thời gian thực và trạng thái hoạt động.
4. **Độ tin cậy công nghiệp (Fail-Safe Mechanism):** Thiết kế máy trạng thái FSM nhận biết lỗi mất cáp cảm biến để kích hoạt chế độ báo động khẩn cấp và tự phục hồi khi kết nối ổn định.

---

## 📁 2. Cấu trúc thư mục dự án

```text
YoloUNO_PlatformIO-DHT20_LCD/
├── platformio.ini              # Cấu hình nạp code & thư viện PlatformIO
├── boards/                     # File định nghĩa bo mạch Yolo:UNO
│   └── yolo_uno.json
├── include/                    # Header files khai báo module
│   ├── global.h                # Khai báo biến toàn cục & cờ đồng bộ
│   ├── temp_humi_monitor.h     # Giao tiếp cảm biến DHT20
│   ├── neopixel_control.h      # Điều khiển đèn NeoPixel RGB
│   └── led_control.h           # Điều biến xung nhịp User LED
├── src/                        # Mã nguồn thực thi (C++)
│   ├── main.cpp                # Vòng lặp chính khởi tạo hệ thống
│   ├── global.cpp              # Định nghĩa biến toàn cục
│   ├── temp_humi_monitor.cpp   # Xử lý đọc & phân tích dữ liệu I2C
│   ├── neopixel_control.cpp    # Hiển thị màu sắc theo dải nhiệt độ
│   └── led_control.cpp         # Điều khiển chớp tắt LED phi chặn
├── lib/                        # Thư viện ngoài (DHT20, LiquidCrystal_I2C,...)
├── Report_IOT_Lab3/            # Mã nguồn LaTeX bài báo cáo
├── LAB3_2313946.pdf            # Báo cáo thí nghiệm định dạng PDF
└── README.md                   # Tài liệu hướng dẫn kỹ thuật Lab 3
```

---

## 🔌 3. Sơ đồ kết nối phần cứng & Địa chỉ I2C

### 3.1. Phân bố chân ngoại vi trên Kit Yolo:UNO

| Ngoại vi / Tín hiệu | Chân ESP32-S3 | Chế độ | Chức năng kỹ thuật |
| :--- | :--- | :--- | :--- |
| **I2C SDA** | `GPIO 11` | Bidirectional | Đường truyền dữ liệu cảm biến & màn hình |
| **I2C SCL** | `GPIO 12` | Output Clock | Xung nhịp đồng bộ bus I2C (100 kHz) |
| **NeoPixel RGB** | `GPIO 6` | Output (1-Wire) | Đèn LED WS2812B báo dải nhiệt độ |
| **User LED** | `GPIO 13` | Digital Output | LED báo trạng thái nhấp nháy theo độ ẩm |
| **Serial Console** | `USB-C CDC` | UART | Giám sát dữ liệu qua terminal (115200 bps) |

### 3.2. Bảng địa chỉ thiết bị trên Bus I2C

| Thiết bị ngoại vi | Chip điều khiển | Địa chỉ I2C (Hex) | Ghi chú |
| :--- | :--- | :--- | :--- |
| **Cảm biến DHT20** | Aosong DHT20 | `0x38` | Cố định của nhà sản xuất |
| **Màn hình LCD 1602** | PCF8574 Backpack | `0x27` / `0x3F` | Bus mở rộng điều khiển 4-bit LCD |

---

## 📊 4. Nội dung bài thực hành

### 🌡️ Bài 1: Giám sát nhiệt độ & NeoPixel RGB
Thu thập giá trị nhiệt độ từ DHT20 theo chu kỳ 1000 ms và phân định trạng thái thông qua 3 dải màu cảnh báo trực quan:

| Dải nhiệt độ | Đánh giá trạng thái | Màu sắc NeoPixel | Mã màu RGB |
| :--- | :--- | :--- | :--- |
| **< 25.0 °C** | Mát mẻ (*Cool*) | 🔵 Xanh dương (*Blue*) | `(0, 0, 255)` |
| **25.0 °C – 32.0 °C** | Tiêu chuẩn (*Comfort*) | 🟢 Xanh lá (*Green*) | `(0, 255, 0)` |
| **> 32.0 °C** | Nóng / Cảnh báo (*Hot*) | 🔴 Đỏ (*Red*) | `(255, 0, 0)` |

---

### 💧 Bài 2: Điều biến tần số nhấp nháy LED theo độ ẩm
Sử dụng hàm định thời phi chặn `millis()` để thay đổi tốc độ chớp của User LED (GPIO 13) tương ứng với độ ẩm môi trường:

| Dải độ ẩm tương đối | Đánh giá | Chu kỳ nhấp nháy | Thời gian Sáng / Tắt |
| :--- | :--- | :--- | :--- |
| **< 50.0% RH** | Khô hạn (*Dry*) | **2000 ms** (Chậm) | 1000 ms ON / 1000 ms OFF |
| **50.0% – 75.0% RH** | Tiêu chuẩn (*Normal*) | **1000 ms** (Vừa) | 500 ms ON / 500 ms OFF |
| **> 75.0% RH** | Ẩm ướt (*Humid*) | **400 ms** (Nhanh) | 200 ms ON / 200 ms OFF |

---

### 🛡️ Bài 3: Máy trạng thái hữu hạn xử lý sự cố (Fail-Safe FSM)
Cơ chế bảo an tự động phản ứng khi xảy ra đứt cáp hoặc mất tín hiệu từ cảm biến DHT20:

- **Trạng thái `NORMAL`:** Khi cảm biến phản hồi mã `DHT20_OK` (0), hệ thống hoạt động bình thường theo Bài 1 và 2.
- **Trạng thái `FAIL_SAFE`:** Khi phát hiện mất tín hiệu I2C hoặc checksum sai:
  - NeoPixel lập tức chuyển sang chế độ chớp tắt màu **VÀNG / ĐỎ**.
  - User LED nhấp nháy tần số cao (**100 ms**) báo hiệu lỗi phần cứng.
  - Xuất thông điệp khẩn cấp ra cổng Serial: `[ERROR] DHT20 Disconnected! Entering Fail-Safe mode`.
  - Tự động khôi phục lại trạng thái bình thường ngay khi gắn lại cảm biến mà không cần Reset kit.

---

### 📟 Bài 4 & 5: Hiển thị trạng thái HMI trên LCD 1602
Giao diện hiển thị trực quan thông số tại trạm đo mà không cần kết nối máy tính:

- **Chế độ chuẩn:**
  ```text
  +----------------+
  |Temp: 28.5C [OK]|
  |Humi: 65.2% [OK]|
  +----------------+
  ```
- **Chế độ sự cố (Fail-Safe):**
  ```text
  +----------------+
  |Temp: --.-C [ERR]|
  |  SENSOR ERROR! |
  +----------------+
  ```

---

## 🚀 5. Hướng dẫn biên dịch & Nạp mã nguồn

Khởi chạy từ thư mục gốc của dự án với PlatformIO Core CLI:

```bash
# 1. Kiểm tra và biên dịch mã nguồn
pio run

# 2. Nạp chương trình vào bo mạch ESP32-S3 Yolo:UNO
pio run --target upload

# 3. Mở Serial Monitor để giám sát dữ liệu thời gian thực
pio device monitor -b 115200
```

> **Mẹo:** Trong VS Code, bạn có thể nhấn trực tiếp biểu tượng dấu tích `✔` (Build) và mũi tên `➔` (Upload) trên thanh công cụ PlatformIO phía dưới màn hình.

---

## ⚙️ 6. Lưu ý kỹ thuật quan trọng

1. **Điện trở kéo lên (Pull-up Resistors):** Chuẩn giao tiếp I2C hoạt động ở dạng cực thu hở (*Open-Drain*), yêu cầu điện trở kéo lên 3.3V trên cả đường `SDA` và `SCL` (thường từ 4.7kΩ đến 10kΩ). Bo mạch Yolo:UNO và module DHT20 đã tích hợp sẵn linh kiện này.
2. **Thời gian khởi động cảm biến:** Sau khi cấp nguồn, cảm biến DHT20 cần độ trễ tối thiểu **100 ms** để ổn định điện áp trước khi phát lệnh `Wire.begin()` và `dht20.begin()`.
3. **Quét bus I2C:** Trường hợp LCD hoặc cảm biến không phản hồi, nên nạp sketch `I2C Scanner` để kiểm tra chính xác địa chỉ thiết bị thực tế đang có mặt trên đường truyền.
