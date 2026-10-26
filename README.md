# LAB 2: LẬP TRÌNH NHÚNG IOT VÀ ĐA NHIỆM FREERTOS TRÊN ESP32 (GPIO & NEOPIXEL)

Khoa Khoa học và Kỹ thuật Máy tính - Trường Đại học Bách Khoa, ĐHQG-HCM  
Môn học: Internet vạn vật (CO3061 / CO3062)  
Nhánh Git: `lab2_gpio_neopixel` (hoặc `lab2`)  
Nền tảng: Kit Yolo:UNO (ESP32-S3 WROOM-1, Dual-core Xtensa LX7 @ 240 MHz)  
Môi trường: PlatformIO IDE (Framework Arduino for ESP32 & FreeRTOS)  

---

## 1. Mục tiêu thí nghiệm

1. Nắm vững kiến trúc phần cứng ngoại vi GPIO trên vi điều khiển ESP32-S3:
   - Chế độ hoạt động GPIO: `OUTPUT`, `INPUT`, `INPUT_PULLUP`.
   - Giao thức truyền thông 1 dây tốc độ cao (Single-wire NZR protocol @ 800 kHz) điều khiển LED thông minh WS2812B (NeoPixel RGB).
2. Xóa bỏ hoàn toàn việc sử dụng hàm dừng `delay()` gây chặn CPU; thay thế bằng kỹ thuật định thời phi chặn (Non-blocking timing) với hàm `millis()`.
3. Tiếp cận hệ điều hành thời gian thực FreeRTOS tích hợp sẵn trong nhân ESP-IDF:
   - Khởi tạo tác vụ độc lập với `xTaskCreate()` hoặc `xTaskCreatePinnedToCore()`.
   - Sử dụng `vTaskDelay()` chuyển tác vụ sang trạng thái Blocked để nhường tài nguyên CPU.
   - Đồng bộ hóa dữ liệu giữa các tác vụ song song thông qua biến dùng chung được bảo vệ bởi từ khóa `volatile`.

---

## 2. Phân bố chân phần cứng (Pinout Configuration)

| Ngoại vi / Tín hiệu | Chân GPIO ESP32-S3 | Định dạng tín hiệu | Giải thích kỹ thuật |
| :--- | :--- | :--- | :--- |
| Onboard User LED | `GPIO 13` | Digital Output, Active-High | LED đơn báo hiệu trên kit Yolo:UNO |
| Onboard NeoPixel RGB | `GPIO 6` | Single-wire Data In (DIN) | IC WS2812B tích hợp LED RGB 5050 |
| Console UART | `USB-C CDC` | Serial Baudrate 115200 bps | In nhật ký hệ thống (Serial Monitor) |

---

## 3. Danh mục các bài tập thực hành và Mô tả chi tiết

### Bài 1: Nhấp nháy luân phiên đơn giản (Asymmetric Blinking)
- **Mục tiêu:** Điều khiển LED đơn với chu kỳ bất đối xứng mà không gây block hệ thống.
- **Yêu cầu thực hiện:**
  - Khởi tạo một FreeRTOS Task chuyên biệt: `Task_LED_Blink`.
  - Điều khiển User LED (`GPIO 13`) hoạt động với chu kỳ:
    - Thời gian SÁNG (ON): **1000 ms (1 giây)**.
    - Thời gian TẮT (OFF): **2000 ms (2 giây)**.
    - Tổng chu kỳ thực thi: 3000 ms (3 giây).
  - Sử dụng `vTaskDelay(pdMS_TO_TICKS(1000))` để định thời chính xác theo xung nhịp hệ thống.
- **Kiểm chứng:** LED13 trên kit nhấp nháy đúng nhịp, đồng thời Serial Monitor in thời gian hệ thống không bị trễ.

### Bài 2: Phối hợp LED và NeoPixel (Đồng bộ qua biến dùng chung)
- **Mục tiêu:** Phối hợp hai ngoại vi riêng biệt đồng bộ trạng thái thời gian thực.
- **Yêu cầu thực hiện:**
  - Khai báo một biến toàn cục đồng bộ: `volatile bool g_led_state = false;`.
  - Luồng chủ (Master Task): Điều khiển LED13 nháy chu kỳ 2s SÁNG / 2s TẮT, đồng thời ghi trạng thái vào `g_led_state`.
  - Luồng phụ (Slave Task): Định kỳ thăm dò biến `g_led_state`:
    - Khi `g_led_state == true`: Sáng đèn NeoPixel màu XANH LÁ (Green, RGB: `0, 255, 0`).
    - Khi `g_led_state == false`: Tắt đèn NeoPixel (RGB: `0, 0, 0`).
- **Kiểm chứng:** Đèn NeoPixel bật và tắt đồng thời hoàn toàn với nhịp chớp của LED13.

### Bài 3: Các mẫu nhấp nháy độc lập (Independent Blink Patterns)
- **Mục tiêu:** Chứng minh tính ưu việt của hệ điều hành đa nhiệm FreeRTOS khi chạy các chu kỳ thời gian bất đồng bộ.
- **Yêu cầu thực hiện:**
  - Tạo 2 FreeRTOS Task độc lập chạy song song trên ESP32-S3:
    - **Task 1 (LED13):** Chu kỳ 2000 ms (Sáng 1500 ms / Tắt 500 ms).
    - **Task 2 (NeoPixel):** Chu kỳ 3000 ms (Sáng màu ĐỎ 1000 ms / Tắt 2000 ms).
  - Hai tác vụ không can thiệp vào nhau; sự chênh lệch chu kỳ được bộ lập lịch FreeRTOS phân bổ CPU mượt mà.
- **Kiểm chứng:** Quan sát thực nghiệm thấy chu kỳ của LED13 và NeoPixel hoàn toàn lệch pha tự nhiên mà không bị giật hay nhảy sai nhịp.

### Bài 4: Chuỗi màu NeoPixel nâng cao (Advanced Rainbow Sequence)
- **Mục tiêu:** Khai thác tối đa khả năng phối màu RGB 24-bit trên đèn WS2812B.
- **Yêu cầu thực hiện:**
  - Xây dựng một mảng cấu trúc hoặc máy trạng thái tuần tự chuyển đổi 5 màu sắc sắc nét, mỗi màu duy trì đúng **3000 ms (3 giây)**:
    1. **Màu ĐỎ (Red):** RGB `(255, 0, 0)`
    2. **Màu VÀNG (Yellow):** RGB `(255, 255, 0)`
    3. **Màu XANH LÁ (Green):** RGB `(0, 255, 0)`
    4. **Màu XANH DƯƠNG (Blue):** RGB `(0, 0, 255)`
    5. **Màu TÍM (Purple):** RGB `(128, 0, 128)`
  - Sau khi kết thúc chuỗi 5 màu, hệ thống tự động lặp lại từ đầu.
- **Kiểm chứng:** Màu sắc trên NeoPixel chuyển đổi sắc nét, đúng thứ tự và không bị trễ khung hình.

### Bài 5: Tích hợp chuyển chế độ hoạt động (Interactive Mode Switch)
- **Mục tiêu:** Kết hợp ngắt ngoài hoặc máy trạng thái hữu hạn (FSM) để chuyển đổi linh hoạt giữa các hiệu ứng đèn.
- **Yêu cầu thực hiện:**
  - Tích hợp thêm nút nhấn ngoài hoặc phân tích ký tự điều khiển từ cổng Serial Monitor.
  - Chuyển đổi qua lại giữa Chế độ Đơn sắc, Chế độ Cầu vồng và Chế độ Cảnh báo khẩn cấp.

---

## 4. Hướng dẫn Biên dịch và Nạp chương trình

Từ thư mục gốc của mã nguồn Lab 2 trên PlatformIO:

```bash
# Biên dịch chương trình
pio run

# Nạp vào kit Yolo:UNO
pio run --target upload

# Mở cổng Serial Monitor để kiểm tra
pio device monitor -b 115200
```

---

## 5. Quy tắc kỹ thuật và Lưu ý

1. **Độ sáng NeoPixel:** Không đặt độ sáng LED WS2812B ở mức 100% khi cấp nguồn từ cổng USB vì dòng tiêu thụ cao có thể gây sụt áp ESP32; khuyến nghị đặt `pixels.setBrightness(50)`.
2. **Khai báo biến toàn cục:** Mọi biến toàn cục được ghi bởi một task và đọc bởi task khác bắt buộc phải khai báo `volatile` để tránh trình biên dịch tối ưu hóa giá trị vào thanh ghi.
3. **Stack Size trong FreeRTOS:** Khi sử dụng `xTaskCreate`, cần cấp phát ngăn xếp tối thiểu `2048` bytes cho các tác vụ có gọi lệnh `Serial.printf()` để tránh lỗi tràn bộ nhớ (Stack Overflow Panic).
