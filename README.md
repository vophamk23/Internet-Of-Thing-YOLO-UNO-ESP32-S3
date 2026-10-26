# LAB 4: EMBEDDED WEB SERVER AND RESTFUL API ARCHITECTURE ON ESP32

Khoa Khoa hoc va Ky thuat May tinh - Truong Dai hoc Bach Khoa, DHQG-HCM  
Mon hoc: Internet van vat (CO3061 / CO3062)  
Nhanh Git: `lab4_webserver`  
Nen tang: Kit Yolo:UNO (ESP32-S3 WROOM-1, Dual-core Xtensa LX7 @ 240 MHz)  
Moi truong: PlatformIO IDE (Framework Arduino for ESP32 & FreeRTOS)  

---

## 1. Muc tieu thi nghiem

1. Nam vung kien truc mang Wi-Fi tren he vi dieu khien ESP32-S3:
   - Che do **Wi-Fi Station (STA):** Ket noi truc tiep vao mang cuc bo (LAN) hoac Router gia dinh de nhan dia chi IP dong qua DHCP.
   - Che do **SoftAP (Access Point):** Bo mach tu phat song Wi-Fi de phuc vu cau hinh mang ban dau.
2. Xay dung may chu Web bat dong bo (Asynchronous HTTP Web Server voi `ESPAsyncWebServer` va `AsyncTCP`):
   - Xu ly da luong ket noi dong thoi ma khong can thiep hay lam cham cac tac vu doc cam bien FreeRTOS.
   - Thiet ke he thong API RESTful chuan JSON su dung thu vien `ArduinoJson`.
3. Nang cao do tin cay du lieu va toi uu hoa thuat toan nhung:
   - **Bo loc trung binh truot (Moving Average Filter 10 mau):** Lam min du lieu, khu nhieu ngau nhien tu cam bien nhiet do va do am.
   - **Thuat toan tre Hysteresis 1%:** Ngan ngua hien tuong rung dao dong (Chattering / Flip-flop) tai cac nguong chuyen trang thai do am.
   - **Kien truc Client-Side Processing:** Phan bo tinh toan giua ESP32 va trinh duyet nguoi dung (JavaScript) de giam tai CPU cho thiet bi nhung.
4. Tich hop tinh nang nang cap phan mem qua mang khong day (Over-The-Air Update - ElegantOTA).

---

## 2. Phan bo kien truc da nhiem FreeRTOS (Multitasking Architecture)

He thong phan chia tai nguyen thanh 2 tac vu chinh hoat dong song song:

```
+--------------------------------------------------------------------------+
|                       ESP32-S3 CORE 0 & CORE 1                           |
+------------------------------------+-------------------------------------+
|      TASK 1: sensor_task           |       TASK 2: internet_task         |
|  - Doc I2C DHT20 moi 1 giay        |  - Duy tri ket noi Wi-Fi            |
|  - Tinh Moving Average (10 mau)    |  - ESPAsyncWebServer phuc vu HTTP   |
|  - Tinh toan Hysteresis 1%         |  - Phuc vu REST API & Web Dashboard |
|  - Dong bo qua Mutex/Global state  |  - ElegantOTA Firmware Update       |
+------------------------------------+-------------------------------------+
```

---

## 3. Dac ta cac Route HTTP va Giao tiep Web Server

| Phuong thuc | Route URI | Dinh dang tra ve | Chuc nang ky thuat |
| :--- | :--- | :--- | :--- |
| `GET` | `/` | `text/html` | Phuc vu giao dien Web Dashboard HTML5/CSS/JS |
| `GET` | `/api/sensors` | `application/json` | Tra ve so lieu do cam bien sau khi qua bo loc |
| `POST` | `/api/led` | `application/json` | Nhan lenh bat/tat User LED hoac doi mau NeoPixel |
| `GET` | `/api/status` | `application/json` | Thong tin he thong: Wi-Fi RSSI, Heap memory, Uptime |
| `GET/POST`| `/update` | `text/html` / Binary | Giao dien va cong nap firmware OTA qua trinh duyet |

**Dinh dang JSON mau tai Route `/api/sensors`:**
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

## 4. Danh muc cac bai tap thuc hanh va Mo ta chi tiet

### Bai 0: Cac Checkpoint thuc hanh co ban va Dieu khien LED
- **Muc tieu:** Khoi tao he thong, ket noi Wi-Fi thanh cong va phuc vu trang Web tinh tu ma nguon PROGMEM.
- **Thuc thi:**
  - Hoan thien cac diem kiem tra D1 den D6 de dam bao stack Web Server hoat dong on dinh.
  - Xay dung Route `POST /api/led` de nguoi dung co the bat tat LED tren board thong qua nut nhan tren giao dien Web ma khong bi load lai trang (dung `fetch()` API).

### Bai 1: Bo loc trung binh truot (Moving Average 10 mau) tren ESP32
- **Muc tieu:** Loai bo nhieu dot bien cua cam bien DHT20 bang bo dem vong (Circular Buffer 10 phan tu).
- **Thuat toan:**
  $$\bar{X}_k = \frac{1}{N} \sum_{i=0}^{N-1} X_{k-i} \quad (N = 10)$$
- **Thuc thi:**
  - Khai bao lop `SensorMovingAvg` quan ly mang truot 10 phan tu.
  - Khi du 10 mau dau tien, gia tri trung binh tro nen cuc ky on dinh, loai bo cac gai nhieu tuc thoi.
- **Kiem chung:** So sanh gia tri tho va gia tri loc qua Serial Monitor va Dashboard; duong do thi tro nen muot ma, tin cay.

### Bai 2: Phan loai nguong nhiet do dong tren trinh duyet (Client-Side)
- **Muc tieu:** Ap dung triet ly Edge Computing: Chuyen viec hien thi va phan loai mau sac ve cho trinh duyet xu ly thay vi bat vi dieu khien phai tinh toan HTML.
- **Thuc thi:**
  - Ma JavaScript tren trinh duyet goi `fetch('/api/sensors')` dinh ky moi 1000 ms.
  - Dua tren gia tri nhiet do nhan duoc, JavaScript tu dong doi mau Card giao dien (Xanh: < 25°C, Vang: 25-32°C, Do: > 32°C).

### Bai 3: Muc do am tinh tren ESP32 voi Thuat toan tre (1% Hysteresis)
- **Muc tieu:** Chong hien tuong nhap nhang chuyen trang thai do am khi gia tri do dao dong quanh vung bien.
- **Nguyen ly Hysteresis:**
  - Gia su nguong chuyen giua NORMAL va HUMID la 75%:
    - Chuyen tu NORMAL len HUMID chi khi: `Do_am >= 75.0%`.
    - Chuyen tu HUMID ve NORMAL chi khi: `Do_am < (75.0% - 1.0%) = 74.0%`.
  - Vung chet 1.0% (Deadband) ngan chan viec may bom/quat bat tat lien tuc gay hong thiet bi chap hanh.
- **Kiem chung:** Khi tho nhe vao cam bien o muc 74.8%, trang thai he thong van giu nguyen tinh trang thai cu mot cach on dinh.

### Bai 4 & 5: Tich hop OTA Firmware Update va Dashboard truc quan
- **Muc tieu:** Hoan thien he thong nhung thuong mai voi kha nang nang cap phan mem tu xa khong can cam day USB.
- **Thuc thi:**
  - Tich hop thu vien `ElegantOTA` vao `internet_task`.
  - Truyet cap vao `http://<ESP32_IP>/update`, keo tha file `firmware.bin` de nap chuong trinh moi qua Wi-Fi trong vong vai giay.

---

## 5. Huong dan Bien dich va Nap chuong trinh

```bash
# Bien dich ma nguon
pio run

# Nap vao kit Yolo:UNO lan dau qua USB
pio run --target upload

# Giam sat IP duoc cap boi Router
pio device monitor -b 115200

# Truyet cap giao dien bang trinh duyet tai: http://<IP_DUOC_CAP>
```

---

## 6. Quy tac ky thuat va Luu y can nho

1. **Mutex bao ve bien toan cuc:** Khi `sensor_task` cap nhat bien do am/nhiet do va `internet_task` doc cac bien nay de tao chuoi JSON, bat buoc phai su dung `portMUX_TYPE` hoac `FreeRTOS SemaphoreMutex` de dam bao tinh toan ven du lieu (Thread-safe).
2. **Bo nho Heap tren ESP32:** May chu `ESPAsyncWebServer` cap phat dong bo dem trong Heap; tranh su dung qua nhieu bien `String` kieu Arduino de tranh phan manh bo nho (Memory Fragmentation). Khuyen nghi dung `char[]` hoac `snprintf()`.
