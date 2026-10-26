# INTERNET VẠN VẬT (IoT) - ỨNG DỤNG TRÊN YOLO:UNO ESP32-S3

Khoa Khoa học và Kỹ thuật Máy tính - Trường Đại học Bách Khoa, ĐHQG-HCM  
Môn học: Internet vạn vật (CO3061 / CO3062)  
Nền tảng phần cứng: Kit Yolo:UNO (ESP32-S3 WROOM-1, Dual-core Xtensa LX7 @ 240 MHz)  
Môi trường phát triển: PlatformIO IDE (Arduino Core for ESP32 / FreeRTOS)  

---

## 1. Tổng quan dự án

Repository này chứa toàn bộ tài liệu thiết kế, mã nguồn mẫu và kiến trúc thực hành cho khóa học Internet vạn vật (IoT). Khóa học hướng tới việc xây dựng hệ sinh thái IoT toàn diện từ thiết bị biên (Edge Devices) đến hạ tầng điện toán đám mây (Cloud IoT Platforms), bao gồm:
- Giao thức truyền thông mạng công nghiệp nhẹ MQTT (Message Queuing Telemetry Transport) và nền tảng quản lý CoreIOT / ThingsBoard.
- Lập trình nhúng điều khiển ngoại vi GPIO, đèn thông minh NeoPixel RGB (WS2812B) và đa nhiệm thời gian thực FreeRTOS.
- Giao tiếp truyền thông nối tiếp I2C với cảm biến môi trường DHT20 và màn hình hiển thị LCD 1602.
- Xây dựng máy chủ nhúng bất đồng bộ (Embedded Asynchronous Web Server) phục vụ RESTful API JSON, giám sát Dashboard và cập nhật firmware từ xa qua mạng (OTA Update).
- Thiết kế và tích hợp hệ thống IoT thực tế trong Đồ án cuối kỳ (Capstone Assignment).

---

## 2. Cấu trúc phân nhánh (Git Branching Strategy)

Mã nguồn của dự án được phân bổ khoa học thành các nhánh tương ứng với từng giai đoạn học tập và phát triển:

| Tên nhánh | Nội dung chuyên biệt | Mục tiêu đào tạo |
| :--- | :--- | :--- |
| `main` | Nhánh gốc chứa tài liệu tổng quan, kiến trúc toàn khóa và đặc tả phần cứng Yolo:UNO. | Tham chiếu chung toàn khóa |
| `lab1_coreiot` (hoặc `lab1`) | Nền tảng IoT Cloud & Giao thức MQTT (CoreIOT, Telemetry, RPC hai chiều, Dashboard). | Điện toán đám mây & MQTT |
| `lab2_gpio_neopixel` (hoặc `lab2`) | Lập trình nhúng IoT & Đa nhiệm FreeRTOS trên ESP32 (GPIO LED, WS2812B, Non-blocking timing). | Lập trình phần cứng & RTOS |
| `lab3_dht20_lcd` (hoặc `lab3`) | Giao tiếp cảm biến I2C & Hiển thị (Cảm biến nhiệt ẩm DHT20, LCD 1602, Bảo an Fail-Safe). | Giao thức I2C & Cảm biến |
| `lab4_webserver` (hoặc `lab4`) | Embedded Web Server & RESTful API (ESPAsyncWebServer, Moving Average, Hysteresis, OTA). | Web Server & Mạng không dây |
| `assignment` | Đồ án Hệ thống IoT hoàn chỉnh (Smart Farm / Smart Home / Giám sát môi trường). | Đồ án tổng hợp (Capstone) |

---

## 3. Đặc tả phần cứng Bo mạch Yolo:UNO

Kit Yolo:UNO là bo mạch phát triển IoT tiêu chuẩn được trang bị vi điều khiển thế hệ mới ESP32-S3:

### 3.1. Thông số kỹ thuật vi điều khiển ESP32-S3
- Lõi xử lý: Dual-core 32-bit Xtensa LX7 hoạt động ở tần số lên đến 240 MHz.
- Tập lệnh hỗ trợ: Vector Instructions phục vụ tăng tốc tính toán AI/DSP.
- Bộ nhớ:
  - ROM: 384 KB (Bootloader và lõi hệ thống).
  - SRAM: 512 KB (Bộ nhớ nội).
  - External SPI Flash: 8 MB.
  - External PSRAM: 8 MB (Octal SPI).
- Kết nối không dây:
  - Wi-Fi: 802.11 b/g/n (2.4 GHz) hỗ trợ chế độ Station (STA) và SoftAP.
  - Bluetooth: Bluetooth 5.0 Low Energy (BLE) và Bluetooth Mesh.

### 3.2. Bảng ánh xạ chân ngoại vi trên Yolo:UNO (Pinout Mapping)

| Ngoại vi / Tín hiệu | Chân GPIO ESP32-S3 | Chức năng & Định dạng logic |
| :--- | :--- | :--- |
| Onboard User LED | `GPIO 13` | Digital Output, Active-High |
| Onboard NeoPixel RGB | `GPIO 6` | Single-wire 800 kHz protocol (WS2812B) |
| I2C Serial Data (SDA) | `GPIO 11` | Giao tiếp I2C (DHT20, LCD 1602, RTC) |
| I2C Serial Clock (SCL) | `GPIO 12` | Xung nhịp I2C (100 kHz standard / 400 kHz fast) |
| Cổng Console Debug | USB-C (Native CDC / UART0) | Giám sát Serial Monitor (Baudrate: 115200) |

---

## 4. Yêu cầu môi trường và Cài đặt

### 4.1. Công cụ phát triển (IDE & Toolchain)
- Trình soạn thảo mã nguồn: Visual Studio Code.
- Môi trường phát triển nhúng: Extension **PlatformIO IDE**.
- Framework biên dịch: `framework = arduino` với `platform = espressif32`.
- Trình thông dịch Python: Python 3.9+ (phục vụ chạy script MQTT kiểm thử cho Lab 1).

### 4.2. File cấu hình `platformio.ini` tiêu chuẩn
Mọi dự án trong các lab sử dụng file cấu hình mẫu sau:

```ini
[env:yolo_uno]
platform = espressif32
board = esp32-s3-devkitc-1
framework = arduino
monitor_speed = 115200
upload_speed = 921600

build_flags = 
    -DARDUINO_USB_MODE=1
    -DARDUINO_USB_CDC_ON_BOOT=1

lib_deps = 
    adafruit/Adafruit NeoPixel @ ^1.12.0
    robtillaart/DHT20 @ ^0.3.1
    marcoschwartz/LiquidCrystal_I2C @ ^1.1.4
    bblanchon/ArduinoJson @ ^6.21.4
    me-no-dev/ESPAsyncWebServer @ ^3.6.0
    me-no-dev/AsyncTCP @ ^3.3.2
    knolleary/PubSubClient @ ^2.8
    ayushsharma82/ElegantOTA @ ^3.1.6
```

---

## 5. Hướng dẫn sử dụng Repository

### 5.1. Clone mã nguồn
```bash
git clone https://github.com/vophamk23/INTERNET-OF-THINGS.git
cd INTERNET-OF-THINGS
```

### 5.2. Chuyển đổi giữa các nhánh
Để làm việc với từng bài thực hành cụ thể, sử dụng lệnh checkout của Git:

```bash
# Xem danh sách toàn bộ các nhánh
git branch -a

# Chuyển sang Lab 1: Nền tảng Cloud CoreIOT & MQTT
git checkout lab1_coreiot    # hoặc: git checkout lab1

# Chuyển sang Lab 2: Lập trình nhúng GPIO & NeoPixel
git checkout lab2_gpio_neopixel    # hoặc: git checkout lab2

# Chuyển sang Lab 3: Cảm biến I2C DHT20 & LCD 1602
git checkout lab3_dht20_lcd    # hoặc: git checkout lab3

# Chuyển sang Lab 4: Embedded Web Server & RESTful API
git checkout lab4_webserver    # hoặc: git checkout lab4

# Chuyển sang Assignment: Đồ án hệ thống IoT cuối kỳ
git checkout assignment
```

### 5.3. Quy trình biên dịch và nạp chương trình với PlatformIO
Từ thư mục gốc của từng dự án:

```bash
# Biên dịch mã nguồn
pio run

# Nạp firmware vào bo mạch Yolo:UNO
pio run --target upload

# Mở cửa sổ Serial Monitor để theo dõi nhật ký hệ thống
pio device monitor
```

---

## 6. Danh mục tài liệu tham khảo

1. ESP32-S3 Technical Reference Manual: Espressif Systems.
2. PlatformIO Documentation: https://docs.platformio.org/
3. CoreIOT / ThingsBoard IoT Platform Documentation: https://thingsboard.io/docs/
4. FreeRTOS Real-time Kernel Documentation: https://www.freertos.org/
5. Giáo trình Thực hành Internet vạn vật - Khoa KH&KT Máy tính, ĐH Bách Khoa TP.HCM.
