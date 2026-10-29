# 🌐 LAB 4: XÂY DỰNG EMBEDDED WEB SERVER VÀ KIẾN TRÚC RESTFUL API TRÊN ESP32
### Bất đồng bộ AsyncTCP, Lọc trung bình trượt, Thuật toán trễ Hysteresis & Cập nhật OTA

> **Trường Đại học Bách Khoa - ĐHQG-HCM**  
> **Khoa Khoa học và Kỹ thuật Máy tính**  
> **Môn học:** Internet vạn vật (CO3061 / CO3062)  
> **Sinh viên thực hiện:** Võ Phạm (MSSV: 2313946)  
> **Nhánh Git:** `lab4_webserver` | **Nền tảng:** Yolo:UNO (ESP32-S3 Dual-Core @ 240MHz) & ESP32 Dev

---

## 📖 Mục lục
- [1. Giới thiệu & Mục tiêu](#1-giới-thiệu--mục-tiêu)
- [2. Cấu trúc thư mục dự án](#2-cấu-trúc-thư-mục-dự-án)
- [3. Phân bổ kiến trúc đa nhiệm FreeRTOS (Dual-Core)](#3-phân-bổ-kiến-trúc-đa-nhiệm-freertos-dual-core)
- [4. Đặc tả RESTful API & Giao thức HTTP](#4-đặc-tả-restful-api--giao-thức-http)
- [5. Nội dung chi tiết các bài thực hành](#5-nội-dung-chi-tiết-các-bài-thực-hành)
  - [Bài 0: Khởi tạo hệ thống & Điều khiển LED qua Fetch API](#bài-0-khởi-tạo-hệ-thống--điều-khiển-led-qua-fetch-api)
  - [Bài 1: Bộ lọc trung bình trượt (Moving Average 10 mẫu)](#bài-1-bộ-lọc-trung-bình-trượt-moving-average-10-mẫu)
  - [Bài 2: Phân loại dải nhiệt độ Client-Side bằng JavaScript](#bài-2-phân-loại-dải-nhiệt-độ-client-side-bằng-javascript)
  - [Bài 3: Thuật toán trễ (1% Hysteresis) chống rung trạng thái độ ẩm](#bài-3-thuật-toán-trễ-1-hysteresis-chống-rung-trạng-thái-độ-ẩm)
  - [Bài 4 & 5: Cập nhật Firmware OTA & Dashboard trực quan](#bài-4--5-cập-nhật-firmware-ota--dashboard-trực-quan)
- [6. Hướng dẫn biên dịch & Nạp mã nguồn](#6-hướng-dẫn-biên-dịch--nạp-mã-nguồn)
- [7. Quy tắc kỹ thuật & Lưu ý cốt lõi](#7-quy-tắc-kỹ-thuật--lưu-ý-cốt-lõi)

---

## 🎯 1. Giới thiệu & Mục tiêu

Bài thí nghiệm Lab 4 tập trung vào kỹ thuật phát triển **máy chủ nhúng bất đồng bộ (Embedded Asynchronous Web Server)** và thiết kế kiến trúc **RESTful API** tối ưu trên vi điều khiển **ESP32-S3**:

1. **Làm chủ hạ tầng Wi-Fi nhúng:**
   - Hoạt động ở chế độ **Wi-Fi Station (STA)** kết nối vào mạng LAN nội bộ, nhận địa chỉ IP động qua giao thức DHCP.
   - Sẵn sàng chuyển đổi chế độ **SoftAP** phục vụ cấu hình trạm đo tại hiện trường.
2. **Web Server bất đồng bộ phi chặn (Non-blocking):**
   - Triển khai `ESPAsyncWebServer` kết hợp `AsyncTCP` xử lý đa yêu cầu HTTP đồng thời mà không làm nghẽn chu trình lấy mẫu cảm biến.
   - Phục vụ toàn bộ giao diện HTML5/CSS/JavaScript lưu trữ trực tiếp trong bộ nhớ Flash (**PROGMEM**).
3. **Xử lý tín hiệu & Giải thuật lọc số:**
   - **Bộ lọc trung bình trượt (Moving Average - 10 mẫu):** Triệt tiêu các xung nhiễu đột biến từ cảm biến DHT20.
   - **Thuật toán trễ (1.0% Hysteresis):** Loại bỏ hiện tượng dao động bật/tắt liên tục (*Chattering / Flip-flop*) tại các vùng biên độ ẩm.
   - **Mô hình tính toán phân tán (Client-Side Rendering):** Đẩy việc tính toán hiển thị màu sắc về trình duyệt người dùng để giải phóng tài nguyên CPU cho vi điều khiển.
4. **Bảo trì từ xa qua OTA:** Tích hợp công cụ nâng cấp firmware qua sóng Wi-Fi (*Over-The-Air Update*) bằng `ElegantOTA`.

---

## 📁 2. Cấu trúc thư mục dự án

```text
YoloUNO_PlatformIO-ESP32_WebServer/
├── platformio.ini              # Cấu hình biên dịch (esp32_thuong & yolo_uno)
├── boards/                     # File định nghĩa phần cứng bo mạch Yolo:UNO
│   └── yolo_uno.json
├── include/                    # Header files các module chức năng
│   ├── global.h                # Định nghĩa cấu trúc dữ liệu, Mutex & biến toàn cục
│   ├── sensor_task.h           # Tác vụ đọc cảm biến DHT20 & bộ lọc số
│   ├── internet_task.h         # Tác vụ kết nối Wi-Fi & dịch vụ OTA
│   ├── mainserver.h            # Định nghĩa các Route HTTP REST API
│   └── web_pages.h             # Mã nguồn giao diện Web Dashboard (PROGMEM)
├── src/                        # Mã nguồn C++ thực thi
│   ├── main.cpp                # Khởi tạo tác vụ đa nhiệm FreeRTOS
│   ├── global.cpp              # Khởi tạo tài nguyên Mutex chia sẻ
│   ├── sensor_task.cpp         # Hiện thực giải thuật lọc Moving Average & Hysteresis
│   ├── internet_task.cpp       # Quản trị mạng Wi-Fi và cập nhật ElegantOTA
│   └── mainserver.cpp          # Bộ định tuyến Web Server & xử lý JSON API
├── Report_IOT_Lab4/            # Báo cáo thí nghiệm và câu trả lời lý thuyết
│   ├── main.pdf                # Báo cáo kết quả thí nghiệm định dạng PDF
│   ├── cau_tra_loi_lab4.txt    # Tổng hợp câu trả lời lý thuyết & trắc nghiệm
│   ├── main.tex                # Mã nguồn LaTeX bài báo cáo
│   ├── bkthesis.sty            # Template luận văn ĐHBK
│   ├── content/                # Nội dung phân tích các bài tập
│   └── image/                  # Hình ảnh giao diện Web & đồ thị thực nghiệm
└── README.md                   # Tài liệu hướng dẫn kỹ thuật Lab 4
```

---

## 🔄 3. Phân bổ kiến trúc đa nhiệm FreeRTOS (Dual-Core)

Hệ thống phân chia tài nguyên xử lý trên hai nhân vi điều khiển độc lập nhằm đảm bảo tính phản hồi thời gian thực:

```text
+--------------------------------------------------------------------------+
|                       ESP32-S3 CORE 0 & CORE 1                           |
+------------------------------------+-------------------------------------+
|      TASK 1: sensor_task           |       TASK 2: internet_task         |
|  - Đọc I2C DHT20 mỗi 1 giây        |  - Duy trì kết nối Wi-Fi Station    |
|  - Tính Moving Average (10 mẫu)    |  - ESPAsyncWebServer phục vụ HTTP   |
|  - Tính toán Hysteresis 1%         |  - Phục vụ REST API & Web Dashboard |
|  - Khóa Mutex đồng bộ dữ liệu      |  - ElegantOTA Firmware Update       |
+------------------------------------+-------------------------------------+
                                  ▲
                                  │ (Protected by Mutex)
                                  ▼
                     [ Shared System State Data ]
```

---

## 🔌 4. Đặc tả RESTful API & Giao thức HTTP

| Phương thức | Đường dẫn URI | Kiểu dữ liệu | Chức năng kỹ thuật |
| :--- | :--- | :--- | :--- |
| **`GET`** | `/` | `text/html` | Tải giao diện Web Dashboard (HTML5/CSS3/JS từ Flash) |
| **`GET`** | `/api/sensors` | `application/json` | Truy xuất số liệu nhiệt độ, độ ẩm sau khi đã lọc |
| **`POST`** | `/api/led` | `application/json` | Điều khiển bật/tắt User LED hoặc đổi màu NeoPixel |
| **`GET`** | `/api/status` | `application/json` | Kiểm tra trạng thái: Wi-Fi RSSI, Bộ nhớ Heap, Uptime |
| **`GET/POST`** | `/update` | `text/html` / Binary | Giao diện nạp firmware không dây qua OTA |

**Cấu trúc dữ liệu JSON phản hồi tại `/api/sensors`:**
```json
{
  "temperature": 29.45,
  "humidity": 68.20,
  "temp_status": "COMFORT",
  "humi_status": "NORMAL",
  "samples_count": 10
}
```

---

## 📊 5. Nội dung chi tiết các bài thực hành

### 🛠️ Bài 0: Khởi tạo hệ thống & Điều khiển LED qua Fetch API
- **Mục tiêu:** Thiết lập kết nối Wi-Fi thành công, cấu hình Web Server và phục vụ trang Web từ bộ nhớ `PROGMEM`.
- **Thực thi:**
  - Hoàn thiện các Checkpoint kiểm tra (D1 đến D6) đảm bảo stack mạng hoạt động tin cậy.
  - Xây dựng API `POST /api/led`: Người dùng click nút gạt trên Dashboard, trình duyệt dùng `fetch()` gửi lệnh JSON mà không cần tải lại toàn bộ trang Web.

---

### 📈 Bài 1: Bộ lọc trung bình trượt (Moving Average 10 mẫu)
- **Mục tiêu:** Loại bỏ hoàn toàn các xung nhiễu ngẫu nhiên từ cảm biến bằng bộ đệm vòng (*Circular Buffer*).
- **Công thức tính toán:**
  ```text
  X_avg = (1 / N) * ∑ X[k - i]    (với N = 10 mẫu)
  ```
- **Thực thi:**
  - Lớp `SensorMovingAvg` duy trì mảng trượt 10 phần tử cho cả Nhiệt độ và Độ ẩm.
  - Khi đã thu thập đủ 10 mẫu đầu tiên, đường dữ liệu xuất ra trở nên mịn màng và có độ tin cậy cao.
- **Kiểm chứng:** So sánh tín hiệu cảm biến thô (*Raw*) và tín hiệu đã lọc (*Filtered*) trên Serial Monitor và Dashboard.

---

### 🎨 Bài 2: Phân loại dải nhiệt độ Client-Side bằng JavaScript
- **Mục tiêu:** Giảm tải tính toán cho CPU nhúng bằng cách chuyển logic hiển thị sang phía Client (*Edge Computing*).
- **Thực thi:**
  - Vi điều khiển chỉ có nhiệm vụ phục vụ dữ liệu thô dạng JSON qua API `/api/sensors`.
  - Mã JavaScript trên trình duyệt định kỳ thăm dò dữ liệu và tự động đổi màu thẻ thông tin:
    - 🔵 **Mát mẻ (*Cool*):** `< 25.0 °C` (Màu xanh dương)
    - 🟡 **Lý tưởng (*Comfort*):** `25.0 °C – 32.0 °C` (Màu vàng/xanh lá)
    - 🔴 **Nóng (*Hot*):** `> 32.0 °C` (Màu đỏ cảnh báo)

---

### 🛡️ Bài 3: Thuật toán trễ (1% Hysteresis) chống rung trạng thái độ ẩm
- **Mục tiêu:** Khắc phục triệt để hiện tượng bật/tắt liên tục (*Chattering*) gây hại rơ-le hoặc quạt khi độ ẩm dao động quanh ngưỡng biên.
- **Nguyên lý Deadband 1.0%:**
  - Giả định ngưỡng chuyển giữa **NORMAL** và **HUMID** là **75.0%**:
    - Chuyển từ NORMAL → HUMID khi: `Độ ẩm >= 75.0%`.
    - Chuyển từ HUMID → NORMAL chỉ khi: `Độ ẩm < 74.0%` (tức `75.0% - 1.0%`).
- **Kiểm chứng:** Khi giá trị độ ẩm dao động nhẹ tại `74.8%`, hệ thống giữ nguyên trạng thái trước đó, không bị rung lật trạng thái.

---

### 🚀 Bài 4 & 5: Cập nhật Firmware OTA & Dashboard trực quan
- **Mục tiêu:** Quản trị thiết bị IoT hiện trường mà không cần dùng cáp nạp USB.
- **Thực thi:**
  - Tích hợp thư viện `ElegantOTA` trên luồng `internet_task`.
  - Mở trình duyệt tại địa chỉ `http://<ESP32_IP>/update`, kéo thả file `firmware.bin` vừa biên dịch để nâng cấp thiết bị trong vòng vài giây.

---

## 💻 6. Hướng dẫn biên dịch & Nạp mã nguồn

Dự án hỗ trợ sẵn cấu hình cho cả kit **Yolo:UNO** (mặc định) và bo mạch **ESP32 Dev thông thường**:

```bash
# 1. Biên dịch dự án (chọn môi trường yolo_uno hoặc esp32_thuong)
pio run -e yolo_uno

# 2. Nạp qua cổng USB-C
pio run -e yolo_uno --target upload

# 3. Theo dõi địa chỉ IP được cấp phát
pio device monitor -b 115200

# 4. Mở trình duyệt Web truy cập vào: http://<DIA_CHI_IP_ESP32>
```

---

## ⚙️ 7. Quy tắc kỹ thuật & Lưu ý cốt lõi

1. **Khóa Mutex bảo vệ vùng nhớ dùng chung:** Khi `sensor_task` cập nhật biến trạng thái và `internet_task` đọc dữ liệu để đóng gói JSON, bắt buộc phải dùng `portMUX_TYPE` hoặc `FreeRTOS SemaphoreMutex` để ngăn ngừa xung đột dữ liệu (*Race Condition*).
2. **Quản lý bộ nhớ Heap hiệu quả:** Máy chủ bất đồng bộ sử dụng nhiều bộ đệm động; hạn chế tối đa việc sử dụng chuỗi đối tượng `String` của Arduino nhằm tránh phân mảnh bộ nhớ (*Heap Fragmentation*). Khuyến nghị dùng `snprintf` và mảng ký tự cố định.
3. **An toàn khi nâng cấp OTA:** Trong quá trình nạp firmware qua mạng, tuyệt đối không ngắt nguồn điện để tránh làm hỏng phân vùng nạp chương trình (*Bootloader Panic*).
