# ĐỒ ÁN CUỐI KỲ: THIẾT KẾ HỆ THỐNG IOT HOÀN CHỈNH (FULL-STACK IOT SYSTEM)

Khoa Khoa học và Kỹ thuật Máy tính - Trường Đại học Bách Khoa, ĐHQG-HCM  
Môn học: Internet vạn vật (CO3061 / CO3062)  
Nhánh Git: `assignment`  
Nền tảng: Kit Yolo:UNO (ESP32-S3 WROOM-1, Dual-core Xtensa LX7 @ 240 MHz)  
Môi trường: PlatformIO IDE (Framework Arduino for ESP32 & FreeRTOS)  

---

## 1. Tổng quan đề tài Bài tập lớn

Nhánh `assignment` dành riêng cho việc nghiên cứu, thiết kế kiến trúc, lập trình và nghiệm thu Đồ án Hệ thống Internet vạn vật (IoT) cuối kỳ. Đề tài yêu cầu phát triển một giải pháp hệ thống IoT hoàn chỉnh (Full-stack IoT System) giải quyết các bài toán thực tế như: Nông nghiệp thông minh (Smart Agriculture), Tòa nhà thông minh (Smart Building), hoặc Giám sát môi trường công nghiệp (Industrial Environmental Monitoring).

Hệ thống phải tích hợp đồng bộ 4 yếu tố cốt lõi:
- **Thu thập dữ liệu đa cảm biến (Multi-Sensor Edge Node):** Đọc dữ liệu nhiệt độ, độ ẩm (DHT20 qua I2C), độ ẩm đất (Soil Moisture qua ADC), cường độ ánh sáng (LDR/I2C) với bộ lọc nhiễu số (Digital Filtering).
- **Xử lý cục bộ và Đa nhiệm FreeRTOS (Edge Processing & RTOS):** Áp dụng kiến trúc đa tác vụ tối ưu cho lõi kép ESP32-S3, thuật toán điều khiển tự động dạng FSM và cơ chế trễ Hysteresis chống rung tải chấp hành.
- **Truyền thông đa kênh (Multi-channel Connectivity):** Kết nối Wi-Fi tự động khôi phục (Auto-reconnect), giao thức MQTT gửi Telemetry và nhận lệnh RPC hai chiều lên CoreIOT / ThingsBoard, đồng thời phục vụ máy chủ Web Server cục bộ.
- **Giám sát, Điều khiển và Bảo an (Dashboard, HMI & Fail-Safe):** Dashboard trực quan hóa trên Cloud, cơ chế cảnh báo sự cố tự động ngắt Rơ-le và tính năng nâng cấp phần mềm qua mạng không dây (ElegantOTA).

---

## 2. Kiến trúc Hệ thống phân tầng (End-to-End IoT Architecture)

```
+--------------------------------------------------------------------------+
|                  CLOUD & APPLICATION LAYER (CoreIOT / ThingsBoard)       |
|  - Real-time Telemetry Dashboard (Gauges, Charts, Cards)                 |
|  - Bidirectional RPC Controls (Relay / Fan / Pump switches)              |
|  - Rule Engine (Email / SMS / Webhook Alarm Alerts)                      |
+--------------------------------------------------------------------------+
                                    ^  | MQTT Protocol (Port 1883)
                                    |  v (Telemetry / RPC / Attributes)
+--------------------------------------------------------------------------+
|                  COMMUNICATION & SERVICES LAYER (ESP32-S3)               |
|  - Wi-Fi Station Manager (Auto-reconnect with Exponential Backoff)       |
|  - Embedded Asynchronous HTTP Web Server (RESTful API & Local Dashboard) |
|  - ElegantOTA Remote Firmware Update Service                             |
+--------------------------------------------------------------------------+
|                  EDGE PROCESSING & FREERTOS RUNTIME                      |
|  - FreeRTOS Tasks: Sensor Acquisition, FSM Controller, Network Manager   |
|  - Thread-Safe IPC: Mutexes, Queues, Event Flags                         |
|  - Digital Signal Processing: Moving Average (10 samples) & Hysteresis   |
+--------------------------------------------------------------------------+
|                  PHYSICAL HARDWARE & TRANSDUCERS (Yolo:UNO)              |
|  - Sensors: DHT20 (I2C: SDA=11, SCL=12), Soil Moisture (ADC), Light LDR  |
|  - Actuators: Solid State Relay / Electro-mechanical Relay, NeoPixel RGB |
+--------------------------------------------------------------------------+
```

---

## 3. Cấu trúc thư mục mã nguồn tiêu chuẩn (Project Structure)

Mỗi nhóm phát triển đồ án duy trì tổ chức thư mục khoa học như sau:

```
assignment/
├── docs/                                  # Tài liệu đặc tả hệ thống
│   ├── system_requirements_spec.md        # Đặc tả yêu cầu hệ thống (SRS)
│   ├── hardware_schematic_wiring.md       # Sơ đồ nguyên lý và kết nối chân
│   └── mqtt_api_interface_spec.md         # Định nghĩa JSON schema và MQTT topics
├── firmware/                              # Dự án PlatformIO nạp cho bo mạch Yolo:UNO
│   ├── platformio.ini                     # Cấu hình thư viện và board ESP32-S3
│   ├── include/
│   │   ├── config.h                       # Wi-Fi credentials, MQTT Tokens, Thresholds
│   │   ├── sensor_manager.h               # Thu thập cảm biến và bộ lọc Moving Average
│   │   ├── actuator_controller.h          # Điều khiển Relay, Quạt, Máy bơm, NeoPixel
│   │   ├── fsm_controller.h               # Máy trạng thái điều khiển tự động
│   │   ├── mqtt_handler.h                 # Truyền thông MQTT và RPC với CoreIOT
│   │   └── web_server_handler.h           # Web server cục bộ và RESTful API
│   └── src/
│       ├── main.cpp                       # Setup FreeRTOS tasks và khởi động hệ thống
│       ├── sensor_manager.cpp
│       ├── actuator_controller.cpp
│       ├── fsm_controller.cpp
│       ├── mqtt_handler.cpp
│       └── web_server_handler.cpp
├── dashboard/                             # Cấu hình trực quan hóa trên Cloud
│   └── coreiot_dashboard_layout.json      # File xuất bản layout Dashboard CoreIOT
└── README.md                              # Tài liệu hướng dẫn của nhánh assignment
```

---

## 4. Quy hoạch Tác vụ FreeRTOS và Phân bổ Tài nguyên (Task Planning)

Để đảm bảo tính thời gian thực và tránh hiện tượng nghẽn tài nguyên, các tác vụ được phân bổ như sau:

| Tên Tác vụ (Task Name) | Chức năng kỹ thuật | Core ESP32 | Priority | Stack Size | Cơ chế đồng bộ |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `Task_Sensors` | Đọc I2C DHT20, ADC đất định kỳ 1000 ms | Core 1 | 3 | 3072 B | `vTaskDelay`, `Mutex` |
| `Task_Control_FSM` | Tính toán Hysteresis và máy trạng thái tự động | Core 1 | 4 (Cao) | 2048 B | `Mutex`, `Queue` |
| `Task_Actuators` | Điều khiển Relay và NeoPixel cảnh báo | Core 1 | 2 | 2048 B | `vTaskDelay`, `Queue` |
| `Task_Network_MQTT` | Publish Telemetry mỗi 3s, xử lý gói RPC | Core 0 | 2 | 4096 B | `PubSubClient`, `Mutex` |
| `Task_WebServer` | Phục vụ REST API và Dashboard cục bộ | Core 0 | 1 (Thấp) | 4096 B | `AsyncTCP`, non-blocking |

---

## 5. Các Tính năng An toàn và Chống chịu sự cố (Reliability & Fail-Safe)

1. **Tự động kết nối lại Wi-Fi và MQTT (Auto-reconnect):** Khi sóng Wi-Fi bị ngắt hoặc máy chủ Broker bảo trì, hệ thống tự động thử lại sau mỗi khoảng thời gian tăng dần (Exponential Backoff) mà không làm ngưng trệ các tác vụ điều khiển máy bơm cục bộ.
2. **Cơ chế bảo vệ chống cháy nổ (Fail-Safe Emergency):** Nếu cảm biến nhiệt độ bị lỗi hoặc vượt qua ngưỡng khẩn cấp (> 55°C), hệ thống lập tức ngắt Rơ-le máy bơm/đèn sưởi, chuyển đèn NeoPixel sang chế độ nhấp nháy màu Đỏ cảnh báo và gửi tin nhắn khẩn cấp lên Cloud.
3. **Thuật toán trễ Hysteresis:** Bắt buộc áp dụng trên tất cả các kênh điều khiển tự động (Độ ẩm đất, Nhiệt độ) để bảo vệ tuổi thọ cho Rơ-le và Động cơ.

---

## 6. Hướng dẫn Biên dịch và Nạp chương trình

```bash
# Di chuyển vào thư mục firmware
cd firmware

# Biên dịch mã nguồn
pio run

# Nạp vào kit Yolo:UNO
pio run --target upload

# Theo dõi Serial Monitor
pio device monitor -b 115200
```

---

## 7. Tiêu chí Đánh giá và Nghiệm thu Đồ án

- **Khả năng vận hành thực tế (40%):** Hệ thống vận hành ổn định liên tục ít nhất 24 giờ, dữ liệu truyền đều đặn lên CoreIOT Cloud mà không bị treo.
- **Đáp ứng thời gian thực và RPC (25%):** Thao tác bật tắt thiết bị trên Dashboard phản hồi dưới 500 ms; chế độ tự động chuyển pha chính xác.
- **Kiến trúc mã nguồn và Module hóa (20%):** Tuân thủ nguyên lý Clean Code, chia tách file logic rõ ràng, ứng dụng FreeRTOS đúng chuẩn và không dùng các hàm chặn `delay()`.
- **Tài liệu kỹ thuật và Báo cáo (15%):** Bản báo cáo chi tiết, đầy đủ sơ đồ kiến trúc, biểu đồ FSM và nhật ký thực nghiệm.
