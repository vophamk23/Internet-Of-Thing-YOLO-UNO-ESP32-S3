# LAB 3: I2C SENSOR INTERFACING AND DISPLAY ON ESP32 (DHT20 & LCD 1602)

Khoa Khoa hoc va Ky thuat May tinh - Truong Dai hoc Bach Khoa, DHQG-HCM  
Mon hoc: Internet van vat (CO3061 / CO3062)  
Nhanh Git: `lab3_dht20_lcd`  
Nen tang: Kit Yolo:UNO (ESP32-S3 WROOM-1, Dual-core Xtensa LX7 @ 240 MHz)  
Moi truong: PlatformIO IDE (Framework Arduino for ESP32 & FreeRTOS)  

---

## 1. Muc tieu thi nghiem

1. Nam vung nguyen ly truyen thong noi tiep dong bo I2C (Inter-Integrated Circuit):
   - Co che 2 day: Day du lieu noi tiep (SDA) va Day xung nhip (SCL) voi dien tro keo len (Pull-up resistors).
   - Dinh dia chi thiet bi 7-bit tren bus: Cam bien DHT20 (`0x38`), Man hinh LCD 1602 I2C qua chip PCF8574 (`0x27` hoac `0x3F`).
   - Khung truyen I2C: Tin hieu START, dia chi thiet bi kem bit R/W, bit xac nhan ACK/NACK, cac byte du lieu va tin hieu STOP.
2. Thu thap thong so moi truong thoi gian thuc tu cam bien ky thuat so DHT20:
   - Do nhiet do chinh xac (°C) va do am tuong doi (RH%).
   - Tinh toan ma kiem tra loi (Checksum verification) de loai bo cac gia tri doc loi/nhieu.
3. Hien thi truc quan va canh bao da tang:
   - Chi thi dai nhiet do bang mau sac tren den NeoPixel RGB.
   - Dieu bien tan so chop tat cua LED don theo muc do am moi truong.
   - Hien thi thong so thoi gian thuc len man hinh ky tu LCD 1602.
4. Xay dung may trang thai huu han (FSM) xu ly su co bao an he thong (Fail-Safe Mechanism) khi mat ket noi cam bien dot ngot.

---

## 2. Phan bo chan phan cung va Dia chi I2C (Hardware Configuration)

### 2.1. Bang dau noi ngoai vi tren Yolo:UNO
Board Yolo:UNO dinh nghia cac chan I2C mac dinh cho ESP32-S3:

| Ngoai vi / Tin hieu | Chan GPIO ESP32-S3 | Chuc nang ky thuat |
| :--- | :--- | :--- |
| I2C Serial Data (SDA) | `GPIO 11` | Duong truyen du lieu hai chieu I2C |
| I2C Serial Clock (SCL) | `GPIO 12` | Duong truyen xung nhip dong bo (100 kHz) |
| Onboard NeoPixel RGB | `GPIO 6` | Chi thi mau sac canh bao nhiet do |
| Onboard User LED | `GPIO 13` | Chi thi nhip nhap nhay theo do am |
| Console UART | `USB-C CDC` | Theo doi nhat ky Serial (115200 bps) |

### 2.2. Bang tra cuu dia chi I2C (I2C Address Map)
- **Cam bien DHT20:** Dia chi co dinh `0x38`.
- **Man hinh LCD 1602 (I2C Backpack PCF8574):** Dia chi tieu chuan `0x27` (hoac `0x3F` tuy lo san xuat).

---

## 3. Danh muc cac bai tap thuc hanh va Mo ta chi tiet

### Bai 1: Doc cam bien DHT20 va NeoPixel chi thi dai gia tri nhiet do
- **Muc tieu:** Thu thap du lieu nhiet do va chuyen doi thanh tin hieu canh bao thi giac.
- **Quy chuan phan dai nhiet do va ma mau NeoPixel:**

| Dai nhiet do do duoc | Trang thai moi truong | Mau sac NeoPixel | Ma mau RGB |
| :--- | :--- | :--- | :--- |
| Nhiet do < 25.0°C | Mat me (Cool) | XANH DUONG (Blue) | `(0, 0, 255)` |
| 25.0°C <= Nhiet do <= 32.0°C | Ly tuong (Comfort) | XANH LA (Green) | `(0, 255, 0)` |
| Nhiet do > 32.0°C | Nong / Canh bao (Hot) | DO (Red) | `(255, 0, 0)` |

- **Yeu cau ky thuat:** Chu ky lay mau cam bien la 1000 ms. Du lieu duoc in dinh ky ra Serial Monitor de doi chieu voi mau den thuc te tren bo mach.

### Bai 2: Dieu bien tan so nhap nhay LED theo do am moi truong
- **Muc tieu:** Chuyen doi gia tri do am thanh tan so nhap nhay cua User LED13.
- **Quy chuan chu ky va tan so nhap nhay:**

| Dai do am moi truong (RH) | Trang thai do am | Chu ky nhap nhay LED13 | Thoi gian Sang / Tat |
| :--- | :--- | :--- | :--- |
| Do am < 50.0% | Kho han (Dry) | **2000 ms (Cham)** | Sang 1000 ms / Tat 1000 ms |
| 50.0% <= Do am <= 75.0% | Ly tuong (Normal) | **1000 ms (Vua)** | Sang 500 ms / Tat 500 ms |
| Do am > 75.0% | Am uot (Humid) | **400 ms (Nhanh)** | Sang 200 ms / Tat 200 ms |

- **Kiem chung:** Khi tho hoi am vao cam bien, do am vuot 75%, LED lap tuc tang toc do chop tat ro ret ma khong lam dung chu trinh kiem tra nhiet do cua NeoPixel o Bai 1.

### Bai 3: Xu ly su co ngat ket noi cam bien va co che bao an (Fail-Safe FSM)
- **Muc tieu:** Xay dung tinh nang tin cay cong nghiep, tu dong phat hien va xu ly khi cam bien bi hong hoac dut cap I2C.
- **Thiet ke May trang thai huu han (FSM):**
  - **Trang thai NORMAL (Binh thuong):** Cam bien tra ve ma loi `DHT20_OK` (0). He thong hoat dong nhu Bai 1 va Bai 2.
  - **Trang thai FAIL_SAFE (Su co):** Khi lenh doc cam bien tra ve ma loi (khong tim thay thiet bi hoac loi truyen tin):
    - Den NeoPixel lap tuc chuyen sang che do **chop tat lien tuc mau VANG / DO** canh bao nguy hiem.
    - User LED13 nhap nhay tan so cuc nhanh (100 ms) bao hieu loi phan cung.
    - In thong diep canh bao khan cap `[ERROR] DHT20 Disconnected! Entering Fail-Safe mode` ra cong Serial.
    - Khi cam bien duoc cam tro lai, FSM tu dong xac nhan va tro ve trang thai NORMAL ma khong can khoi dong lai MCU.

### Bai 4 & 5: Hien thi thong so len Man hinh LCD 1602 I2C
- **Muc tieu:** Tich hop man hinh LCD 1602 de tao giao dien nguoi - may (HMI) doc lap tai hien truong.
- **Dinh dang hien thi:**
  - Dong 1: `Temp: xx.x C [STATUS]`
  - Dong 2: `Humi: yy.y % [STATUS]`
  - Khi o che do Fail-Safe: Dong 2 hien thi thong bao `SENSOR ERROR!` de nhan vien van hanh biet ro tinh trang.

---

## 4. Huong dan Bien dich va Nap chuong trinh

Tu thu muc goc cua nhanh tren PlatformIO:

```bash
# Bien dich chuong trinh
pio run

# Nap vao kit Yolo:UNO
pio run --target upload

# Giam sat Serial Monitor
pio device monitor -b 115200
```

---

## 5. Quy tac ky thuat va Luu y ve Bus I2C

1. **Tro keo len (Pull-up Resistors):** Day SDA va SCL bat buoc phai co dien tro keo len nguon 3.3V (thuong tu 4.7kΩ den 10kΩ). Tren kit Yolo:UNO va module DHT20 da tich hop san tro keo.
2. **Kiem tra ket noi bang I2C Scanner:** Neu khong doc duoc du lieu, can nap chuong trinh I2C Scanner de xac nhan dung dia chi `0x38` cua DHT20 va `0x27` cua LCD tren bus truoc khi nap chuong trinh chinh.
3. **Thoi gian khoi dong DHT20:** Sau khi cap nguon, cam bien DHT20 can toi thieu 100 ms de on dinh truoc khi thuc hien lenh khoi tao `Wire.begin()` va `dht20.begin()`.
