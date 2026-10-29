# ⚡ LAB 2: LẬP TRÌNH NHÚNG IOT VÀ ĐA NHIỆM FREERTOS TRÊN ESP32
### Ngoại vi GPIO, Đèn LED WS2812B NeoPixel RGB & Định thời phi chặn

> **Trường Đại học Bách Khoa - ĐHQG-HCM**  
> **Khoa Khoa học và Kỹ thuật Máy tính**  
> **Môn học:** Internet vạn vật (CO3061 / CO3062)  
> **Sinh viên thực hiện:** Võ Phạm (MSSV: 2313946)  
> **Nhánh Git:** `lab2_gpio_neopixel` | **Nền tảng:** Kit Yolo:UNO (ESP32-S3 Dual-Core @ 240MHz)

---

## 📖 Mục lục
- [1. Giới thiệu & Mục tiêu](#1-giới-thiệu--mục-tiêu)
- [2. Cấu trúc thư mục dự án](#2-cấu-trúc-thư-mục-dự-án)
- [3. Sơ đồ kết nối phần cứng](#3-sơ-đồ-kết-nối-phần-cứng)
- [4. Nội dung chi tiết các bài thực hành](#4-nội-dung-chi-tiết-các-bài-thực-hành)
  - [Bài 1: Nhấp nháy luân phiên bất đối xứng](#bài-1-nhấp-nháy-luân-phiên-bất-đối-xứng)
  - [Bài 2: Đồng bộ LED và NeoPixel qua biến dùng chung](#bài-2-đồng-bộ-led-và-neopixel-qua-biến-dùng-chung)
  - [Bài 3: Đa nhiệm FreeRTOS với các mẫu chớp độc lập](#bài-3-đa-nhiệm-freertos-với-các-mẫu-chớp-độc-lập)
  - [Bài 4: Chuỗi màu NeoPixel nâng cao](#bài-4-chuỗi-màu-neopixel-nâng-cao)
  - [Bài 5: Máy trạng thái chuyển đổi chế độ hoạt động](#bài-5-máy-trạng-thái-chuyển-đổi-chế-độ-hoạt-động)
- [5. Hướng dẫn biên dịch & Nạp mã nguồn](#5-hướng-dẫn-biên-dịch--nạp-mã-nguồn)
- [6. Quy tắc kỹ thuật & Lưu ý quan trọng](#6-quy-tắc-kỹ-thuật--lưu-ý-quan-trọng)

---

## 🎯 1. Giới thiệu & Mục tiêu

Bài thực hành Lab 2 trang bị nền tảng lập trình hệ thống nhúng chuyên sâu trên vi điều khiển **ESP32-S3**, xóa bỏ cách lập trình tuần tự truyền thống để chuyển sang hệ điều hành thời gian thực **FreeRTOS**:

1. **Làm chủ ngoại vi GPIO:**
   - Cấu hình các chế độ chân số: `OUTPUT`, `INPUT`, `INPUT_PULLUP`.
   - Giao tiếp 1 dây tốc độ cao (*Single-wire NZR Protocol @ 800 kHz*) điều khiển dải đèn thông minh NeoPixel WS2812B RGB.
2. **Kỹ thuật định thời phi chặn (Non-blocking):** Thay thế triệt để hàm `delay()` bằng hàm `millis()` nhằm tối ưu hóa chu trình CPU.
3. **Đa nhiệm thực sự với FreeRTOS:**
   - Khởi tạo tác vụ độc lập bằng `xTaskCreate()` hoặc `xTaskCreatePinnedToCore()`.
   - Tối ưu hóa bộ lập lịch bằng cách đưa tác vụ sang trạng thái *Blocked* với `vTaskDelay()`.
   - Đồng bộ hóa dữ liệu an toàn giữa các luồng bằng biến chia sẻ có định từ `volatile`.

---

## 📁 2. Cấu trúc thư mục dự án

```text
YoloUNO_PlatformIO-LED_NEO_Blinky/
├── platformio.ini              # Cấu hình biên dịch & nạp code PlatformIO
├── boards/                     # File định nghĩa bo mạch Yolo:UNO
│   └── yolo_uno.json
├── src/                        # Mã nguồn ứng dụng chính
│   └── main.cpp                # Chương trình thực thi tích hợp
├── exercises_code/             # Mã nguồn độc lập từng bài tập
│   ├── exercise_1.cpp          # Bài 1: Nháy LED bất đối xứng
│   ├── exercise_2.cpp          # Bài 2: Đồng bộ LED & NeoPixel
│   ├── exercise_3.cpp          # Bài 3: 2 Tasks FreeRTOS độc lập
│   ├── exercise_4.cpp          # Bài 4: Chuỗi 5 màu NeoPixel tuần tự
│   └── exercise_5.cpp          # Bài 5: FSM chuyển đổi chế độ hoạt động
├── lib/                        # Thư viện ngoài hỗ trợ ngoại vi
├── Report_IOT_Lab2/            # Mã nguồn LaTeX bài báo cáo
├── LAB2_2313946.pdf            # Báo cáo thí nghiệm định dạng PDF
├── LAB2_Exercises_VI.docx      # Báo cáo thí nghiệm định dạng Word
└── README.md                   # Tài liệu hướng dẫn kỹ thuật Lab 2
```

---

## 🔌 3. Sơ đồ kết nối phần cứng

| Ngoại vi / Tín hiệu | Chân ESP32-S3 | Định dạng tín hiệu | Giải thích kỹ thuật |
| :--- | :--- | :--- | :--- |
| **Onboard User LED** | `GPIO 13` | Digital Output, Active-High | LED đơn tích hợp sẵn trên kit Yolo:UNO |
| **NeoPixel RGB** | `GPIO 6` | Single-wire Data In (DIN) | IC WS2812B tích hợp LED RGB 5050 |
| **Serial Console** | `USB-C CDC` | Serial UART (115200 bps) | Xuất nhật ký gỡ lỗi hệ thống |

---

## 📊 4. Nội dung chi tiết các bài thực hành

### ⏱️ Bài 1: Nhấp nháy luân phiên bất đối xứng
- **Mục tiêu:** Điều khiển LED đơn với chu kỳ sáng/tắt bất đối xứng mà không chiếm dụng CPU.
- **Yêu cầu thực hiện:**
  - Khởi tạo FreeRTOS Task riêng biệt: `Task_LED_Blink`.
  - Chu kỳ nháy của LED13:
    - Thời gian **SÁNG (ON):** **1000 ms** (1 giây).
    - Thời gian **TẮT (OFF):** **2000 ms** (2 giây).
    - Tổng chu kỳ lặp: **3000 ms** (3 giây).
  - Sử dụng lệnh `vTaskDelay(pdMS_TO_TICKS(1000))` để nhường CPU cho các tiến trình khác.
- **Kiểm chứng:** LED13 chớp nhịp nhàng theo đúng tỉ lệ 1s/2s, nhật ký Serial không bị nghẽn.

---

### 🔄 Bài 2: Đồng bộ LED và NeoPixel qua biến dùng chung
- **Mục tiêu:** Đồng bộ trạng thái tức thời giữa hai ngoại vi độc lập bằng biến toàn cục.
- **Yêu cầu thực hiện:**
  - Khai báo cờ trạng thái đồng bộ: `volatile bool g_led_state = false;`.
  - **Master Task (LED13):** Đảo trạng thái LED13 với chu kỳ 2s SÁNG / 2s TẮT, đồng thời cập nhật `g_led_state`.
  - **Slave Task (NeoPixel):** Định kỳ kiểm tra cờ `g_led_state`:
    - Khi `g_led_state == true`: Bật NeoPixel màu 🟢 **XANH LÁ** (`0, 255, 0`).
    - Khi `g_led_state == false`: Tắt NeoPixel (`0, 0, 0`).
- **Kiểm chứng:** Đèn NeoPixel bật/tắt đồng pha hoàn hảo với nhịp nhấp nháy của LED13.

---

### 🔀 Bài 3: Đa nhiệm FreeRTOS với các mẫu chớp độc lập
- **Mục tiêu:** Chứng minh sức mạnh phân chia thời gian (*Time-slicing*) của hệ điều hành thời gian thực.
- **Yêu cầu thực hiện:**
  - Khởi tạo 2 Task FreeRTOS chạy song song hoàn toàn độc lập:
    - **Task 1 (LED13):** Chu kỳ **2000 ms** (Sáng 1500 ms / Tắt 500 ms).
    - **Task 2 (NeoPixel):** Chu kỳ **3000 ms** (Sáng màu 🔴 **ĐỎ** 1000 ms / Tắt 2000 ms).
  - Hai tác vụ không phụ thuộc vào nhau; sự chênh lệch nhịp diễn ra tự nhiên nhờ bộ lập lịch Preemptive Scheduler.
- **Kiểm chứng:** Quan sát thực nghiệm thấy chu kỳ chớp của 2 đèn lệch pha trơn tru, không có hiện tượng giật hay đứng hệ thống.

---

### 🌈 Bài 4: Chuỗi màu NeoPixel nâng cao
- **Mục tiêu:** Làm chủ kỹ thuật phối màu RGB 24-bit thông qua máy trạng thái tuần tự.
- **Quy chuẩn 5 màu sắc:** Mỗi màu được giữ sáng liên tục đúng **3000 ms (3 giây)** trước khi chuyển tiếp:

| Thứ tự | Tên màu sắc | Mã màu RGB | Trạng thái hiển thị |
| :---: | :--- | :--- | :---: |
| **1** | 🔴 Đỏ (*Red*) | `(255, 0, 0)` | 3000 ms |
| **2** | 🟡 Vàng (*Yellow*) | `(255, 255, 0)` | 3000 ms |
| **3** | 🟢 Xanh lá (*Green*) | `(0, 255, 0)` | 3000 ms |
| **4** | 🔵 Xanh dương (*Blue*) | `(0, 0, 255)` | 3000 ms |
| **5** | 🟣 Tím (*Purple*) | `(128, 0, 128)` | 3000 ms |

- **Kiểm chứng:** NeoPixel luân chuyển 5 màu sắc mượt mà, đúng thời lượng và tuần hoàn lặp lại liên tục.

---

### 🕹️ Bài 5: Máy trạng thái chuyển đổi chế độ hoạt động
- **Mục tiêu:** Tích hợp tương tác người dùng để điều hướng linh hoạt giữa các chế độ hiệu ứng.
- **Yêu cầu thực hiện:**
  - Nhận lệnh điều khiển qua nút bấm vật lý hoặc ký tự nhập từ Serial Monitor.
  - Thiết kế FSM chuyển đổi qua lại giữa:
    - **Chế độ 1:** Đơn sắc (*Single Color Alert*).
    - **Chế độ 2:** Hiệu ứng cầu vồng tuần tự (*Rainbow Sequence*).
    - **Chế độ 3:** Báo động khẩn cấp (*Emergency Flash*).

---

## 🚀 5. Hướng dẫn biên dịch & Nạp mã nguồn

Khởi chạy bằng công cụ PlatformIO Core CLI từ thư mục gốc của dự án:

```bash
# 1. Biên dịch toàn bộ dự án
pio run

# 2. Nạp firmware vào kit Yolo:UNO qua cổng USB-C
pio run --target upload

# 3. Theo dõi log qua Serial Monitor
pio device monitor -b 115200
```

---

## ⚙️ 6. Quy tắc kỹ thuật & Lưu ý quan trọng

1. **Giới hạn dòng tiêu thụ NeoPixel:** Khi cấp nguồn từ cổng USB 5V, không thiết lập độ sáng LED WS2812B ở mức tối đa (255) vì dòng tải lớn có thể gây sụt áp vi điều khiển; khuyến nghị cài đặt an toàn: `pixels.setBrightness(50)`.
2. **Từ khóa `volatile` cho biến chia sẻ:** Mọi biến toàn cục được nhiều tác vụ đọc/ghi đồng thời bắt buộc phải có tiền tố `volatile` nhằm tránh việc trình biên dịch GCC tối ưu hóa cất giá trị vào thanh ghi.
3. **Ngăn xếp FreeRTOS (Stack Size):** Khi dùng `xTaskCreate()`, nên cấp phát kích thước tối thiểu **2048 bytes** (hoặc `4096 bytes` nếu có thao tác in chuỗi `Serial.printf`) để phòng ngừa lỗi tràn ngăn xếp (*Stack Overflow Crash*).
