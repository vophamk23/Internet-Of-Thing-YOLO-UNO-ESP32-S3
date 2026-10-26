# INTERNET OF THINGS (IoT) - YOLO:UNO ESP32-S3 APPLICATIONS

Khoa Khoa hoc va Ky thuat May tinh - Truong Dai hoc Bach Khoa, DHQG-HCM  
Mon hoc: Internet van vat (CO3061 / CO3062)  
Nen tang phan cung: Kit Yolo:UNO (ESP32-S3 WROOM-1, Dual-core Xtensa LX7 @ 240 MHz)  
Moi truong phat trien: PlatformIO IDE (Arduino Core for ESP32 / FreeRTOS)  

---

## 1. Tong quan du an

Repository nay chua toan bo tai lieu thiet ke, ma nguon mau va kien truc thuc hanh cho khoa hoc Internet van vat (IoT). Khoa hoc huong toi viec xay dung he thong IoT toan dien tu thiet bi bien (Edge Devices) den ha tang dien toan dam may (Cloud IoT Platforms), bao gom:
- Giao thuc truyen thong mang cong nghiep nhe MQTT (Message Queuing Telemetry Transport) va nen tang quan ly CoreIOT / ThingsBoard.
- Lap trinh nhung dieu khien ngoai vi GPIO, den thong minh NeoPixel RGB (WS2812B) va da nhiem thoi gian thuc FreeRTOS.
- Giao tiep truyen thong noi tiep I2C voi cam bien moi truong DHT20 va man hinh hien thi LCD 1602.
- Xay dung may chu nhung bat dong bo (Embedded Asynchronous Web Server) phuc vu RESTful API JSON, giam sat Dashboard va cap nhat firmware tu xa qua mang (OTA Update).
- Thiet ke va tich hop he thong IoT thuc te trong Do an cuoi ky (Capstone Assignment).

---

## 2. Cau truc phan nhanh (Git Branching Strategy)

Ma nguon cua du an duoc phan bo khoa hoc thanh 6 nhanh tuong ung voi cac giai doan thuc hanh va nghien cuu:

| Ten nhanh | Noi dung chuyen biet | Trinh do / Muc tieu |
| :--- | :--- | :--- |
| `main` | Nhanh goc chua tai lieu tong quan, kien truc toan khoa va dac ta phan cung Yolo:UNO. | Tham chieu chung toan khoa |
| `lab1_coreiot` | Nen tang IoT Cloud & Giao thuc MQTT (CoreIOT, Telemetry, RPC hai chieu, Dashboard). | Dien toan dam may & MQTT |
| `lab2_gpio_neopixel` | Lap trinh nhung IoT & Da nhiem FreeRTOS tren ESP32 (GPIO LED, WS2812B, Non-blocking timing). | Lap trinh phan cung & RTOS |
| `lab3_dht20_lcd` | Giao tiep cam bien I2C & Hien thi (Cam bien nhiet am DHT20, LCD 1602, Bao an Fail-Safe). | Giao thuc I2C & Cam bien |
| `lab4_webserver` | Embedded Web Server & RESTful API (ESPAsyncWebServer, Moving Average, Hysteresis, OTA). | Web Server & Mang khong day |
| `assignment` | Do an He thong IoT hoan chinh (Smart Farm / Smart Home / Giam sat moi truong). | Do an tong hop (Capstone) |

---

## 3. Dac ta phan cung Bo mach Yolo:UNO

Kit Yolo:UNO la bo mach phat trien IoT tieu chuan duoc trang bi vi dieu khien the he moi ESP32-S3:

### 3.1. Thong so ky thuat vi dieu khien ESP32-S3
- Loi xu ly: Dual-core 32-bit Xtensa LX7 chay o tan so len den 240 MHz.
- Tap lenh ho tro: Vector Instructions phuc vu tang toc tinh toan AI/DSP.
- Bo nho:
  - ROM: 384 KB (Bootloader va loi he thong).
  - SRAM: 512 KB (Bo nho noi).
  - External SPI Flash: 8 MB.
  - External PSRAM: 8 MB (Octal SPI).
- Ket noi khong day:
  - Wi-Fi: 802.11 b/g/n (2.4 GHz) ho tro che do Station va SoftAP.
  - Bluetooth: Bluetooth 5.0 Low Energy (BLE) va Bluetooth Mesh.

### 3.2. Bang anh xa chan ngoai vi tren Yolo:UNO (Pinout Mapping)

| Ngoai vi / Tin hieu | Chan GPIO ESP32-S3 | Chuc nang & Dinh dang logic |
| :--- | :--- | :--- |
| Onboard User LED | `GPIO 13` | Digital Output, Active-High |
| Onboard NeoPixel RGB | `GPIO 6` | Single-wire 800 kHz protocol (WS2812B) |
| I2C Serial Data (SDA) | `GPIO 11` | Giao tiep I2C (DHT20, LCD 1602, RTC) |
| I2C Serial Clock (SCL) | `GPIO 12` | Xung nhip I2C (100 kHz standard / 400 kHz fast) |
| Cong Console Debug | USB-C (Native CDC / UART0) | Giam sat Serial Monitor (Baudrate: 115200) |

---

## 4. Yeu cau moi truong va Cai dat

### 4.1. Cong cu phat trien (IDE & Toolchain)
- Trinh soan thao ma nguon: Visual Studio Code.
- Moi truong phat trien nhung: Extension **PlatformIO IDE**.
- Framework bien dich: `framework = arduino` voi `platform = espressif32`.
- Trinh thong dich Python: Python 3.9+ (phuc vu chay script MQTT test cho Lab 1).

### 4.2. File cau hinh `platformio.ini` tieu chuan
Moi du an trong cac lab su dung file cau hinh mau sau:

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

## 5. Huong dan su dung Repository

### 5.1. Clone ma nguon
```bash
git clone https://github.com/vophamk23/INTERNET-OF-THINGS.git
cd INTERNET-OF-THINGS
```

### 5.2. Chuyen doi giua cac nhanh
De lam viec voi tung bai thuc hanh cu the, su dung lenh checkout cua Git:

```bash
# Xem danh sach toan bo cac nhanh
git branch -a

# Chuyen sang Lab 1: Nen tang Cloud CoreIOT & MQTT
git checkout lab1_coreiot

# Chuyen sang Lab 2: Lap trinh nhung GPIO & NeoPixel
git checkout lab2_gpio_neopixel

# Chuyen sang Lab 3: Cam bien I2C DHT20 & LCD 1602
git checkout lab3_dht20_lcd

# Chuyen sang Lab 4: Embedded Web Server & RESTful API
git checkout lab4_webserver

# Chuyen sang Assignment: Do an he thong IoT cuoi ky
git checkout assignment
```

### 5.3. Quy trinh bien dich va nap chuong trinh voi PlatformIO
Tu thu muc goc cua tung du an:

```bash
# Bien dich ma nguon
pio run

# Nap firmware vao bo mach Yolo:UNO
pio run --target upload

# Mo cua so Serial Monitor de theo doi nhat ky he thong
pio device monitor
```

---

## 6. Danh muc tai lieu tham khao

1. ESP32-S3 Technical Reference Manual: Espressif Systems.
2. PlatformIO Documentation: https://docs.platformio.org/
3. CoreIOT / ThingsBoard IoT Platform Documentation: https://thingsboard.io/docs/
4. FreeRTOS Real-time Kernel Documentation: https://www.freertos.org/
5. Giao trinh Thuc hanh Internet van vat - Khoa KH&KT May tinh, DH Bach Khoa TP.HCM.
