# 🌐 INTERNET VẠN VẬT (IoT) - YOLO:UNO ESP32-S3
### Hệ thống Thực hành IoT: Từ Thiết bị Biên đến Điện toán Đám mây

> **Trường Đại học Bách Khoa - ĐHQG-HCM**  
> **Khoa Khoa học và Kỹ thuật Máy tính**  
> **Môn học:** Internet vạn vật (CO3061 / CO3062)  
> **Sinh viên thực hiện:** Võ Phạm (MSSV: 2313946)  
> **Nền tảng phần cứng:** Kit Yolo:UNO (ESP32-S3 WROOM-1, Dual-Core Xtensa LX7 @ 240MHz)  
> **Môi trường phát triển:** PlatformIO IDE (Framework Arduino for ESP32 & FreeRTOS)

---

## 📖 Mục lục
- [1. Tổng quan dự án](#1-tổng-quan-dự-án)
- [2. Cấu trúc phân nhánh Git (Branching Strategy)](#2-cấu-trúc-phân-nhánh-git-branching-strategy)
- [3. Tóm tắt nội dung các bài thí nghiệm](#3-tóm-tắt-nội-dung-các-bài-thí-nghiệm)
  - [Lab 1: Nền tảng IoT Cloud & Giao thức MQTT](#-lab-1-nền-tảng-iot-cloud--giao-thức-mqtt)
  - [Lab 2: Lập trình nhúng & Đa nhiệm FreeRTOS](#-lab-2-lập-trình-nhúng--đa-nhiệm-freertos)
  - [Lab 3: Giao tiếp I2C & Cảnh báo đa tầng](#-lab-3-giao-tiếp-i2c--cảnh-báo-đa-tầng)
  - [Lab 4: Embedded Web Server & RESTful API](#-lab-4-embedded-web-server--restful-api)
  - [Assignment: Đồ án hệ thống IoT tổng hợp](#-assignment-đồ-án-hệ-thống-iot-tổng-hợp)
- [4. Đặc tả phần cứng Bo mạch Yolo:UNO](#4-đặc-tả-phần-cứng-bo-mạch-yolouno)
- [5. Hướng dẫn cài đặt & Khởi chạy](#5-hướng-dẫn-cài-đặt--khởi-chạy)
- [6. Tài liệu tham khảo](#6-tài-liệu-tham-khảo)

---

## 🎯 1. Tổng quan dự án

Repository này lưu trữ toàn bộ mã nguồn, cấu hình phần cứng và báo cáo thực nghiệm cho môn học **Internet vạn vật (IoT)** tại Trường ĐH Bách Khoa - ĐHQG-HCM. Dự án được xây dựng theo mô hình kiến trúc phân tầng chuẩn công nghiệp:

- **Tầng Thiết bị biên (Edge Perception Layer):** Thu thập dữ liệu từ cảm biến nhiệt độ, độ ẩm kỹ thuật số (DHT20), xử lý tín hiệu số (Bộ lọc trung bình trượt, thuật toán trễ Hysteresis) và điều khiển cơ cấu chấp hành (LED đơn, NeoPixel WS2812B RGB, màn hình LCD 1602).
- **Tầng Mạng & Giao thức (Network Layer):** Vận hành mạng Wi-Fi (chế độ kép Station & SoftAP), giao thức truyền thông nhẹ MQTT (Publish/Subscribe) và máy chủ HTTP bất đồng bộ (AsyncTCP).
- **Tầng Ứng dụng & Đám mây (Cloud & Application Layer):** Tích hợp nền tảng đám mây CoreIOT (ThingsBoard), Web Dashboard trực quan hóa dữ liệu chuỗi thời gian, điều khiển từ xa hai chiều (Two-Way RPC) và cập nhật phần mềm không dây (OTA Update).

---

## 🌿 2. Cấu trúc phân nhánh Git (Branching Strategy)

Mỗi bài thí nghiệm được quản lý độc lập trên một nhánh Git chuyên biệt, hoàn chỉnh từ mã nguồn đến tài liệu báo cáo:

| Nhánh Git | Nội dung kỹ thuật | Nền tảng & Giao thức | Báo cáo hoàn chỉnh |
| :--- | :--- | :--- | :---: |
| **`main`** | Tổng quan hệ thống, tài liệu tham chiếu toàn khóa & đặc tả Yolo:UNO | Markdown / Docs | — |
| [**`lab1_coreiot`**](https://github.com/vophamk23/Internet-Of-Thing-YOLO-UNO-ESP32-S3/tree/lab1_coreiot) | IoT Cloud CoreIOT, MQTT Broker, Telemetry Streaming, Two-Way RPC | Python 3 / MQTT / Cloud | PDF & LaTeX |
| [**`lab2_gpio_neopixel`**](https://github.com/vophamk23/Internet-Of-Thing-YOLO-UNO-ESP32-S3/tree/lab2_gpio_neopixel) | GPIO Driver, NeoPixel WS2812B RGB, Đa nhiệm FreeRTOS, Non-blocking | ESP32-S3 / FreeRTOS / PlatformIO | PDF & LaTeX |
| [**`lab3_dht20_lcd`**](https://github.com/vophamk23/Internet-Of-Thing-YOLO-UNO-ESP32-S3/tree/lab3_dht20_lcd) | Giao thức I2C, Cảm biến DHT20, Màn hình LCD 1602, Cơ chế Fail-Safe | ESP32-S3 / I2C Bus / PlatformIO | PDF & LaTeX |
| [**`lab4_webserver`**](https://github.com/vophamk23/Internet-Of-Thing-YOLO-UNO-ESP32-S3/tree/lab4_webserver) | Embedded Web Server, RESTful API, Moving Average, Hysteresis, OTA | ESP32-S3 / AsyncWebServer / OTA | PDF & LaTeX |
| [**`assignment`**](https://github.com/vophamk23/Internet-Of-Thing-YOLO-UNO-ESP32-S3/tree/assignment) | Đồ án tích hợp hệ thống IoT hoàn chỉnh cuối khóa | ESP32-S3 / Full-stack IoT | Đồ án cuối kỳ |

---

## 📚 3. Tóm tắt nội dung các bài thí nghiệm

### ☁️ Lab 1: Nền tảng IoT Cloud & Giao thức MQTT
- **Trọng tâm:** Kết nối thiết bị giả lập Python với máy chủ `app.coreiot.io:1883` qua giao thức MQTT.
- **Tính năng chính:**
  - Xác thực bảo mật bằng Device Access Token riêng biệt.
  - Gửi Telemetry chu kỳ 3 giây/lần với định dạng JSON (nhiệt độ, độ ẩm, ánh sáng).
  - Điều khiển hai chiều thời gian thực thông qua cơ chế Remote Procedure Call (RPC).
  - Thiết kế Visualization Dashboard (Gauges, Cards, Time-series Charts) và xuất bản Public URL.

---

### ⚡ Lab 2: Lập trình nhúng & Đa nhiệm FreeRTOS
- **Trọng tâm:** Xóa bỏ hoàn toàn hàm dừng `delay()` gây nghẽn CPU, chuyển sang đa nhiệm thực sự với FreeRTOS.
- **Tính năng chính:**
  - Nhấp nháy LED đơn bất đối xứng (1s ON / 2s OFF) bằng `vTaskDelay()`.
  - Phối hợp nhịp nhàng giữa LED13 và đèn thông minh NeoPixel qua biến dùng chung `volatile`.
  - Vận hành song song 2 tác vụ độc lập chạy lệch pha mà không bị giật nhịp.
  - Máy trạng thái tuần tự hiển thị chuỗi 5 dải màu RGB 24-bit (3 giây/màu).

---

### 📡 Lab 3: Giao tiếp I2C & Cảnh báo đa tầng
- **Trọng tâm:** Truyền thông nối tiếp đồng bộ I2C 2 dây (`SDA` / `SCL`) với cảm biến DHT20 và màn hình LCD 1602.
- **Tính năng chính:**
  - Thu thập thông số môi trường chính xác, kiểm tra mã sai số Checksum (CRC).
  - Phân tầng chỉ thị cảnh báo: NeoPixel đổi màu theo dải nhiệt độ, LED đơn điều biến tốc độ chớp theo độ ẩm.
  - Hiển thị trực quan giao diện người - máy (HMI) trên LCD 1602 qua chip PCF8574.
  - Cơ chế bảo an công nghiệp (*Fail-Safe FSM*): Tự động phát hiện đứt cáp I2C, kích hoạt báo động khẩn cấp và tự phục hồi khi có tín hiệu.

---

### 🌐 Lab 4: Embedded Web Server & RESTful API
- **Trọng tâm:** Xây dựng máy chủ Web bất đồng bộ phục vụ REST API JSON trực tiếp trên vi điều khiển.
- **Tính năng chính:**
  - Chế độ mạng kép `WIFI_AP_STA`: Vừa phát SoftAP để cấu hình, vừa làm Station kết nối Router.
  - Bộ lọc trung bình trượt (*Moving Average 10 mẫu*) triệt tiêu xung nhiễu đột biến của cảm biến.
  - Thuật toán trễ (*1.0% Hysteresis*) chống rung lật trạng thái độ ẩm.
  - Cơ chế đồng bộ dữ liệu Dual-Core an toàn giữa `sensor_task` và `internet_task` bằng FreeRTOS Mutex.
  - Tích hợp công cụ nâng cấp phần mềm qua mạng không dây (*Over-The-Air - ElegantOTA*).

---

### 🚀 Assignment: Đồ án hệ thống IoT tổng hợp
- Tích hợp trọn vẹn chuỗi công nghệ từ vi điều khiển ESP32-S3 đến máy chủ đám mây, xây dựng giải pháp IoT hoàn chỉnh đáp ứng tiêu chuẩn đồ án môn học.

---

## 🔌 4. Đặc tả phần cứng Bo mạch Yolo:UNO

Kit **Yolo:UNO** là bo mạch phát triển IoT tiêu chuẩn trang bị vi điều khiển hiệu năng cao **ESP32-S3 WROOM-1**:

### 4.1. Thông số kỹ thuật vi điều khiển ESP32-S3
- **Lõi xử lý:** Dual-Core 32-bit Xtensa LX7 hoạt động ở xung nhịp tối đa **240 MHz**.
- **Tập lệnh mở rộng:** Vector Instructions hỗ trợ tăng tốc xử lý tính toán số học và AI/DSP.
- **Bộ nhớ:**
  - **SRAM nội:** 512 KB (320 KB khả dụng cho ứng dụng người dùng).
  - **Flash SPI ngoài:** 8 MB (QIO mode tốc độ cao).
  - **ROM:** 384 KB lưu trữ Bootloader và nhân hệ thống.
- **Kết nối không dây:** Wi-Fi 802.11 b/g/n (2.4 GHz) & Bluetooth 5.0 Low Energy (BLE) / Bluetooth Mesh.

### 4.2. Bảng phân bố chân ngoại vi chuẩn (Pinout Mapping)

| Ngoại vi / Tín hiệu | Chân GPIO ESP32-S3 | Chế độ hoạt động | Chức năng kỹ thuật |
| :--- | :--- | :--- | :--- |
| **Onboard User LED** | `GPIO 13` | Digital Output | LED đơn báo hiệu trạng thái hệ thống |
| **Onboard NeoPixel RGB** | `GPIO 6` | Single-wire (800 kHz) | IC WS2812B tích hợp LED 5050 RGB |
| **I2C SDA (Data)** | `GPIO 11` | Bidirectional Open-Drain | Đường truyền dữ liệu cảm biến DHT20 & LCD |
| **I2C SCL (Clock)** | `GPIO 12` | Output Clock | Xung nhịp đồng bộ I2C (100 kHz) |
| **Nút nhấn BOOT** | `GPIO 0` | Input Pull-up | Chuyển chế độ nạp hoặc reset cấu hình mạng |
| **Cổng nạp & Console** | USB-C | Native USB CDC / UART | Nạp code và theo dõi nhật ký (115200 bps) |

---

## 💻 5. Hướng dẫn cài đặt & Khởi chạy

### 5.1. Sao chép kho mã nguồn
```bash
# Clone repository về máy tính
git clone https://github.com/vophamk23/Internet-Of-Thing-YOLO-UNO-ESP32-S3.git
cd Internet-Of-Thing-YOLO-UNO-ESP32-S3
```

### 5.2. Chuyển đổi giữa các nhánh bài thực hành
Mỗi bài lab được đóng gói hoàn chỉnh trên nhánh tương ứng:

```bash
# Liệt kê tất cả các nhánh
git branch -a

# Làm việc với Lab 1: Cloud CoreIOT & MQTT
git checkout lab1_coreiot

# Làm việc với Lab 2: GPIO & NeoPixel FreeRTOS
git checkout lab2_gpio_neopixel

# Làm việc với Lab 3: Cảm biến I2C DHT20 & LCD 1602
git checkout lab3_dht20_lcd

# Làm việc với Lab 4: Embedded Web Server & RESTful API
git checkout lab4_webserver

# Làm việc với Đồ án cuối kỳ
git checkout assignment
```

### 5.3. Quy trình biên dịch và nạp với PlatformIO
Từ thư mục của bài lab tương ứng (Lab 2, Lab 3, Lab 4):

```bash
# 1. Biên dịch mã nguồn cho bo mạch Yolo:UNO
pio run

# 2. Nạp firmware qua cổng USB-C
pio run --target upload

# 3. Mở Serial Monitor để giám sát nhật ký hoạt động
pio device monitor -b 115200
```

---

## 📖 6. Tài liệu tham khảo

1. **Espressif Systems:** [ESP32-S3 Technical Reference Manual](https://www.espressif.com/en/products/socs/esp32s3)
2. **PlatformIO:** [PlatformIO IDE Documentation](https://docs.platformio.org/)
3. **CoreIOT / ThingsBoard:** [ThingsBoard IoT Platform Documentation](https://thingsboard.io/docs/)
4. **FreeRTOS:** [FreeRTOS Real-time Kernel Reference Manual](https://www.freertos.org/)
5. **Khoa KH&KT Máy tính - ĐH Bách Khoa TP.HCM:** *Giáo trình Thực hành Internet vạn vật (CO3061 / CO3062)*.
