# FINAL CAPSTONE ASSIGNMENT: FULL-STACK INTERNET OF THINGS SYSTEM DESIGN

Khoa Khoa hoc va Ky thuat May tinh - Truong Dai hoc Bach Khoa, DHQG-HCM  
Mon hoc: Internet van vat (CO3061 / CO3062)  
Nhanh Git: `assignment`  
Nen tang: Kit Yolo:UNO (ESP32-S3 WROOM-1, Dual-core Xtensa LX7 @ 240 MHz)  
Moi truong: PlatformIO IDE (Framework Arduino for ESP32 & FreeRTOS)  

---

## 1. Tong quan de tai Bai tap lon

Nhanh `assignment` danh rieng cho viec nghien cuu, thiet ke kien truc, lap trinh va nghiem thu Do an He thong Internet van vat (IoT) cuoi ky. De tai yeu cau phat trien mot giai phap he thong IoT hoan chinh (Full-stack IoT System) giai quyet cac bai toan thuc te nhu: Nong nghiep thong minh (Smart Agriculture), Toa nha thong minh (Smart Building), hoac Giam sat moi truong cong nghiep (Industrial Environmental Monitoring).

He thong phai tich hop dong bo 4 yeu to cot loi:
- **Thu thap du lieu da cam bien (Multi-Sensor Edge Node):** Doc du lieu nhiet do, do am (DHT20 qua I2C), do am dat (Soil Moisture qua ADC), cuong do anh sang (LDR/I2C) voi bo loc nhieu so (Digital Filtering).
- **Xu ly cuc bo va Da nhiem FreeRTOS (Edge Processing & RTOS):** Ap dung kien truc da tac vu chia nho tai nguyen cho loi kep ESP32-S3, thuat toan dieu khien tu dong kieu FSM va co che tre Hysteresis chong rung tai chap hanh.
- **Truyen thong da kenh (Multi-channel Connectivity):** Ket noi Wi-Fi tu dong khoi phuc (Auto-reconnect), giao thuc MQTT gui Telemetry va nhan lenh RPC hai chieu len CoreIOT / ThingsBoard, dong thoi phuc vu may chu Web Server cuc bo.
- **Giam sat, Dieu khien va Bao an (Dashboard, HMI & Fail-Safe):** Dashboard truc quan hoa tren Cloud, co che canh bao su co tu dong cat Role va tinh nang nang cap phan mem qua mang khong day (ElegantOTA).

---

## 2. Kien truc He thong phan tang (End-to-End IoT Architecture)

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

## 3. Cau truc thu muc ma nguon tieu chuan (Project Structure)

Moi nhom phat trien do an duy tri to chuc thu muc khoa hoc nhu sau:

```
assignment/
├── docs/                                  # Tai lieu dac ta he thong
│   ├── system_requirements_spec.md        # Dac ta yeu cau he thong (SRS)
│   ├── hardware_schematic_wiring.md       # So do nguyen ly va ket noi chan
│   └── mqtt_api_interface_spec.md         # Dinh nghia JSON schema va MQTT topics
├── firmware/                              # Du an PlatformIO nap cho bo mach Yolo:UNO
│   ├── platformio.ini                     # Cau hinh thu vien va board ESP32-S3
│   ├── include/
│   │   ├── config.h                       # Wi-Fi credentials, MQTT Tokens, Thresholds
│   │   ├── sensor_manager.h               # Thu thap cam bien va bo loc Moving Average
│   │   ├── actuator_controller.h          # Dieu khien Relay, Quat, May bom, NeoPixel
│   │   ├── fsm_controller.h               # May trang thai dieu khien tu dong
│   │   ├── mqtt_handler.h                 # Truyen thong MQTT va RPC voi CoreIOT
│   │   └── web_server_handler.h           # Web server cuc bo va RESTful API
│   └── src/
│       ├── main.cpp                       # Setup FreeRTOS tasks va khoi dong he thong
│       ├── sensor_manager.cpp
│       ├── actuator_controller.cpp
│       ├── fsm_controller.cpp
│       ├── mqtt_handler.cpp
│       └── web_server_handler.cpp
├── dashboard/                             # Cau hinh truc quan hoa tren Cloud
│   └── coreiot_dashboard_layout.json      # File xuat ban layout Dashboard CoreIOT
└── README.md                              # Tai lieu huong dan cua nhanh assignment
```

---

## 4. Quy hoach Tac vu FreeRTOS va Phan bo Tai nguyen (Task Planning)

De dam bao tinh thoi gian thuc va khong xay ra hien tuong nghe nghen tai nguyen, cac tac vu duoc phan bo nhu sau:

| Ten Tac vu (Task Name) | Chuc nang ky thuat | Core ESP32 | Priority | Stack Size | Co che dong bo |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `Task_Sensors` | Doc I2C DHT20, ADC dat dinh ky 1000 ms | Core 1 | 3 | 3072 B | `vTaskDelay`, `Mutex` |
| `Task_Control_FSM` | Tinh toan Hysteresis va may trang thai tu dong | Core 1 | 4 (Cao) | 2048 B | `Mutex`, `Queue` |
| `Task_Actuators` | Dieu khien Relay va NeoPixel canh bao | Core 1 | 2 | 2048 B | `vTaskDelay`, `Queue` |
| `Task_Network_MQTT` | Publish Telemetry moi 3s, xu ly goi RPC | Core 0 | 2 | 4096 B | `PubSubClient`, `Mutex` |
| `Task_WebServer` | Phuc vu REST API va Dashboard cuc bo | Core 0 | 1 (Thap) | 4096 B | `AsyncTCP`, non-blocking |

---

## 5. Cac Tinh nang An toan va Chong chiu su co (Reliability & Fail-Safe)

1. **Tu dong ket noi lai Wi-Fi va MQTT (Auto-reconnect):** Khi song Wi-Fi bi ngat hoac may chu Broker bao tri, he thong tu dong thu lai sau moi khoang thoi gian tang dan (Exponential Backoff) ma khong bi dung tac vu dieu khien van hanh may bom.
2. **Co che bao ve chong chay no (Fail-Safe Emergency):** Neu cam bien nhiet do bi loi hoac vuot qua nguong khan cap (> 55°C), he thong lap tuc ngat Relay may bom/den suoi, chuyen den NeoPixel sang che do nhap nhay mau Do canh bao va gui tin nhan khan cap len Cloud.
3. **Thuat toan tre Hysteresis:** Bat buoc ap dung tren tat ca cac kenh dieu khien tu dong (Do am dat, Nhiet do) de bao ve tuoi tho cho Relay va Dong co.

---

## 6. Huong dan Bien dich va Nap chuong trinh

```bash
# Di chuyen vao thu muc firmware
cd firmware

# Bien dich ma nguon
pio run

# Nap vao kit Yolo:UNO
pio run --target upload

# Theo doi Serial Monitor
pio device monitor -b 115200
```

---

## 7. Tieu chi Danh gia va Nghiem thu Do an

- **Kha nang van hanh thuc te (40%):** He thong chay on dinh lien tuc it nhat 24 gio, du lieu truyen deu dan len CoreIOT Cloud ma khong bi treo.
- **Dap ung thoi gian thuc va RPC (25%):** Nguoi dung thao tac bat tat thiet bi tren Dashboard phan hoi duoi 500 ms; che do tu dong chuyen pha chinh xac.
- **Kien truc ma nguon va Module hoa (20%):** Tuan thu nguyen ly Clean Code, chia file logic ro rang, su dung FreeRTOS dung chuan va khong dung cac ham block `delay()`.
- **Tai lieu ky thuat va Bao cao (15%):** Ban bao cao chi tiet, day du so do kien truc, bieu do FSM va nhat ky thuc nghiem.
