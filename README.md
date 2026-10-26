# LAB 4: XÂY DỰNG EMBEDDED WEB SERVER VÀ KIẾN TRÚC RESTFUL API TRÊN ESP32

Khoa Khoa học và Kỹ thuật Máy tính - Trường Đại học Bách Khoa, ĐHQG-HCM  
Môn học: Internet vạn vật (CO3061 / CO3062)  
Nhánh Git: `lab4_webserver` (hoặc `lab4`)  
Nền tảng: Kit Yolo:UNO (ESP32-S3 WROOM-1, Dual-core Xtensa LX7 @ 240 MHz)  
Môi trường: PlatformIO IDE (Framework Arduino for ESP32 & FreeRTOS)  

---

## 1. Mục tiêu thí nghiệm

1. Nắm vững kiến trúc mạng Wi-Fi trên hệ vi điều khiển ESP32-S3:
   - Chế độ **Wi-Fi Station (STA):** Kết nối trực tiếp vào mạng cục bộ (LAN) hoặc Router gia đình để nhận địa chỉ IP động qua DHCP.
   - Chế độ **SoftAP (Access Point):** Bo mạch tự phát sóng Wi-Fi để phục vụ cấu hình mạng ban đầu.
2. Xây dựng máy chủ Web bất đồng bộ (Asynchronous HTTP Web Server với `ESPAsyncWebServer` và `AsyncTCP`):
   - Xử lý đa luồng kết nối đồng thời mà không làm nghẽn hoặc gián đoạn các tác vụ đọc cảm biến FreeRTOS.
   - Thiết kế hệ thống API RESTful chuẩn JSON sử dụng thư viện `ArduinoJson`.
3. Nâng cao độ tin cậy dữ liệu và tối ưu hóa thuật toán nhúng:
   - **Bộ lọc trung bình trượt (Moving Average Filter 10 mẫu):** Làm mịn dữ liệu, khử nhiễu ngẫu nhiên từ cảm biến nhiệt độ và độ ẩm.
   - **Thuật toán trễ Hysteresis 1%:** Ngăn ngừa hiện tượng rung dao động chuyển trạng thái (Chattering / Flip-flop) tại các ngưỡng biên độ ẩm.
   - **Kiến trúc Client-Side Processing:** Phân bổ tính toán giữa ESP32 và trình duyệt người dùng (JavaScript) để giảm tải CPU cho thiết bị nhúng.
4. Tích hợp tính năng nâng cấp phần mềm qua mạng không dây (Over-The-Air Update - ElegantOTA).

---

## 2. Phân bổ kiến trúc đa nhiệm FreeRTOS (Multitasking Architecture)

Hệ thống phân chia tài nguyên thành 2 tác vụ chính hoạt động song song trên 2 nhân vi điều khiển:

```
+--------------------------------------------------------------------------+
|                       ESP32-S3 CORE 0 & CORE 1                           |
+------------------------------------+-------------------------------------+
|      TASK 1: sensor_task           |       TASK 2: internet_task         |
|  - Đọc I2C DHT20 mỗi 1 giây        |  - Duy trì kết nối Wi-Fi            |
|  - Tính Moving Average (10 mẫu)    |  - ESPAsyncWebServer phục vụ HTTP   |
|  - Tính toán Hysteresis 1%         |  - Phục vụ REST API & Web Dashboard |
|  - Đồng bộ qua Mutex/Global state  |  - ElegantOTA Firmware Update       |
+------------------------------------+-------------------------------------+
```

---

## 3. Đặc tả các Route HTTP và Giao tiếp Web Server

| Phương thức | Route URI | Định dạng trả về | Chức năng kỹ thuật |
| :--- | :--- | :--- | :--- |
| `GET` | `/` | `text/html` | Phục vụ giao diện Web Dashboard HTML5/CSS/JS |
| `GET` | `/api/sensors` | `application/json` | Trả về số liệu đo cảm biến sau khi qua bộ lọc |
| `POST` | `/api/led` | `application/json` | Nhận lệnh bật/tắt User LED hoặc đổi màu NeoPixel |
| `GET` | `/api/status` | `application/json` | Thông tin hệ thống: Wi-Fi RSSI, Heap memory, Uptime |
| `GET/POST`| `/update` | `text/html` / Binary | Giao diện và cổng nạp firmware OTA qua trình duyệt |

**Định dạng JSON mẫu tại Route `/api/sensors`:**
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

## 4. Danh mục các bài tập thực hành và Mô tả chi tiết

### Bài 0: Các Checkpoint thực hành cơ bản và Điều khiển LED
- **Mục tiêu:** Khởi tạo hệ thống, kết nối Wi-Fi thành công và phục vụ trang Web tĩnh từ bộ nhớ Flash PROGMEM.
- **Thực thi:**
  - Hoàn thiện các điểm kiểm tra D1 đến D6 để đảm bảo stack Web Server hoạt động ổn định.
  - Xây dựng Route `POST /api/led` để người dùng có thể bật tắt LED trên board thông qua nút nhấn trên giao diện Web mà không bị tải lại trang (sử dụng Fetch API).

### Bài 1: Bộ lọc trung bình trượt (Moving Average 10 mẫu) trên ESP32
- **Mục tiêu:** Loại bỏ nhiễu đột biến của cảm biến DHT20 bằng bộ đệm vòng (Circular Buffer 10 phần tử).
- **Thuật toán:**
  $$\bar{X}_k = \frac{1}{N} \sum_{i=0}^{N-1} X_{k-i} \quad (N = 10)$$
- **Thực thi:**
  - Khai báo lớp `SensorMovingAvg` quản lý mảng trượt 10 phần tử.
  - Khi đủ 10 mẫu đầu tiên, giá trị trung bình trở nên cực kỳ ổn định, loại bỏ hoàn toàn các xung nhiễu đột biến.
- **Kiểm chứng:** So sánh giá trị thô và giá trị lọc qua Serial Monitor và Dashboard; đường đồ thị trở nên mịn màng và tin cậy.

### Bài 2: Phân loại ngưỡng nhiệt độ động trên trình duyệt (Client-Side)
- **Mục tiêu:** Áp dụng triết lý tính toán biên (Edge Computing): Chuyển việc định dạng và đổi màu sắc về cho trình duyệt xử lý thay vì bắt vi điều khiển phải sinh mã HTML động.
- **Thực thi:**
  - Mã JavaScript trên trình duyệt gọi `fetch('/api/sensors')` định kỳ mỗi 1000 ms.
  - Dựa trên giá trị nhiệt độ nhận được, JavaScript tự động đổi màu Card giao diện (Xanh: < 25°C, Vàng: 25-32°C, Đỏ: > 32°C).

### Bài 3: Mức độ ẩm tính trên ESP32 với Thuật toán trễ (1% Hysteresis)
- **Mục tiêu:** Chống hiện tượng chập chờn chuyển trạng thái độ ẩm khi giá trị đo dao động sát vùng biên.
- **Nguyên lý Hysteresis:**
  - Giả sử ngưỡng chuyển giữa NORMAL và HUMID là 75%:
    - Chuyển từ NORMAL lên HUMID chỉ khi: `Độ_ẩm >= 75.0%`.
    - Chuyển từ HUMID về NORMAL chỉ khi: `Độ_ẩm < (75.0% - 1.0%) = 74.0%`.
  - Vùng chết 1.0% (Deadband) ngăn chặn việc máy bơm/quạt bật tắt liên tục gây hỏng thiết bị chấp hành.
- **Kiểm chứng:** Khi phả nhẹ vào cảm biến ở mức 74.8%, trạng thái hệ thống vẫn giữ nguyên tính ổn định của trạng thái trước đó.

### Bài 4 & 5: Tích hợp OTA Firmware Update và Dashboard trực quan
- **Mục tiêu:** Hoàn thiện hệ thống nhúng với khả năng nâng cấp phần mềm từ xa không cần cắm cáp nạp USB.
- **Thực thi:**
  - Tích hợp thư viện `ElegantOTA` vào `internet_task`.
  - Truy cập vào `http://<ESP32_IP>/update`, kéo thả file `firmware.bin` để nạp chương trình mới qua Wi-Fi trong vòng vài giây.

---

## 5. Hướng dẫn Biên dịch và Nạp chương trình

```bash
# Biên dịch mã nguồn
pio run

# Nạp vào kit Yolo:UNO lần đầu qua USB
pio run --target upload

# Giám sát địa chỉ IP được cấp bởi Router
pio device monitor -b 115200

# Truy cập giao diện bằng trình duyệt tại: http://<IP_DUOC_CAP>
```

---

## 6. Quy tắc kỹ thuật và Lưu ý cốt lõi

1. **Mutex bảo vệ biến toàn cục:** Khi `sensor_task` cập nhật biến độ ẩm/nhiệt độ và `internet_task` đọc các biến này để tạo chuỗi JSON, bắt buộc phải sử dụng `portMUX_TYPE` hoặc `FreeRTOS SemaphoreMutex` để đảm bảo an toàn luồng (Thread-safe).
2. **Bộ nhớ Heap trên ESP32:** Máy chủ `ESPAsyncWebServer` cấp phát động bộ đệm trong Heap; tránh sử dụng quá nhiều biến `String` kiểu Arduino để tránh phân mảnh bộ nhớ (Memory Fragmentation). Khuyến nghị dùng `char[]` hoặc `snprintf()`.
