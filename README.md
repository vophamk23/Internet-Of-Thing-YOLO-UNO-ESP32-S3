# LAB 2: EMBEDDED IOT PROGRAMMING AND FREERTOS MULTITASKING (GPIO & NEOPIXEL)

Khoa Khoa hoc va Ky thuat May tinh - Truong Dai hoc Bach Khoa, DHQG-HCM  
Mon hoc: Internet van vat (CO3061 / CO3062)  
Nhanh Git: `lab2_gpio_neopixel`  
Nen tang: Kit Yolo:UNO (ESP32-S3 WROOM-1, Dual-core Xtensa LX7 @ 240 MHz)  
Moi truong: PlatformIO IDE (Framework Arduino for ESP32 & FreeRTOS)  

---

## 1. Muc tieu thi nghiem

1. Nam vung kien truc phan cung ngoai vi GPIO tren vi dieu khien ESP32-S3:
   - Che do hoat dong GPIO: `OUTPUT`, `INPUT`, `INPUT_PULLUP`.
   - Giao thuc truyen thong 1 day toc do cao (Single-wire NZR protocol @ 800 kHz) dieu khien den LED thong minh WS2812B (NeoPixel RGB).
2. Xoa bo viec su dung ham dung `delay()` gay chan CPU; thay the bang ky thuat dinh thoi phi chan (Non-blocking timing) voi ham `millis()`.
3. Tiep can he dieu hanh thoi gian thuc FreeRTOS tich hop san trong loi ESP-IDF:
   - Khoi tao tac vu doc lap voi `xTaskCreate()` hoac `xTaskCreatePinnedToCore()`.
   - Su dung `vTaskDelay()` chuyen tac vu sang trang thai Blocked de tiet kiem tai nguyen CPU.
   - Dong bo hoa du lieu giua cac tac vu song song thong qua bien dung chung duoc bao ve boi tu khoa `volatile`.

---

## 2. Phan bo chan phan cung (Pinout Configuration)

| Ngoai vi / Tin hieu | Chan GPIO ESP32-S3 | Dinh dang tin hieu | Giai thich ky thuat |
| :--- | :--- | :--- | :--- |
| Onboard User LED | `GPIO 13` | Digital Output, Active-High | LED don bao hieu tren kit Yolo:UNO |
| Onboard NeoPixel RGB | `GPIO 6` | Single-wire Data In (DIN) | IC WS2812B tich hop LED RGB 5050 |
| Console UART | `USB-C CDC` | Serial Baudrate 115200 bps | In nhat ky he thong (Serial Monitor) |

---

## 3. Danh muc cac bai tap thuc hanh va Mo ta chi tiet

### Bai 1: Nhap nhay luan phien don gian (Asymmetric Blinking)
- **Muc tieu:** Dieu khien LED don voi chu ky bat doi xung ma khong gay block he thong.
- **Yeu cau thuc hien:**
  - Khoi tao mot FreeRTOS Task chuyen biet: `Task_LED_Blink`.
  - Dieu khien User LED (`GPIO 13`) hoat dong voi chu ky:
    - Thoi gian SANG (ON): **1000 ms (1 giay)**.
    - Thoi gian TAT (OFF): **2000 ms (2 giay)**.
    - Tong chu ky thuc thi: 3000 ms (3 giay).
  - Su dung `vTaskDelay(pdMS_TO_TICKS(1000))` de dinh thoi chinh xac theo xung nhip he thong.
- **Kiem chung:** LED13 tren kit nhap nhay dung nhip, dong thoi Serial Monitor in thoi gian he thong khong bi tre.

### Bai 2: Phoi hop LED va NeoPixel (Dong bo qua bien dung chung)
- **Muc tieu:** Phoi hop hai ngoai vi rieng biet dong bo trang thai thoi gian thuc.
- **Yeu cau thuc hien:**
  - Khai bao mot bien toan cuc dong bo: `volatile bool g_led_state = false;`.
  - Luong chu (Master Task): Dieu khien LED13 nhay chu ky 2s SANG / 2s TAT, dong thoi ghi trang thai vao `g_led_state`.
  - Luong phu (Slave Task): Dinh ky tham do bien `g_led_state`:
    - Khi `g_led_state == true`: Sang den NeoPixel mau XANH LA (Green, RGB: `0, 255, 0`).
    - Khi `g_led_state == false`: Tat den NeoPixel (RGB: `0, 0, 0`).
- **Kiem chung:** Den NeoPixel bat va tat dong thoi hoan toan voi nhip chop cua LED13.

### Bai 3: Cac mau nhap nhay doc lap (Independent Blink Patterns)
- **Muc tieu:** Chung minh tinh uu viet cua he dieu hanh da nhiem FreeRTOS khi chay cac chu ky thoi gian bat dong bo.
- **Yeu cau thuc hien:**
  - Tao 2 FreeRTOS Task doc lap chay song song tren ESP32-S3:
    - **Task 1 (LED13):** Chu ky 2000 ms (Sang 1500 ms / Tat 500 ms).
    - **Task 2 (NeoPixel):** Chu ky 3000 ms (Sang mau DO 1000 ms / Tat 2000 ms).
  - Hai tac vu khong can thiep vao nhau; su chenh lech chu ky duoc bo lap lich FreeRTOS phan bo CPU muot ma.
- **Kiem chung:** Quan sat thuc nghiem thay chu ky cua LED13 va NeoPixel hoan toan lech pha tu nhien ma khong bi dung hinh hoac nhay sai nhip.

### Bai 4: Chuoi mau NeoPixel nang cao (Advanced Rainbow Sequence)
- **Muc tieu:** Khai thac toi da kha nang phoi mau RGB 24-bit tren den WS2812B.
- **Yeu cau thuc hien:**
  - Xay dung mot mang cau truc hoac may trang thai tuan tu chuyen doi 5 mau sac sac net, moi mau duy tri dung **3000 ms (3 giay)**:
    1. **Mau DO (Red):** RGB `(255, 0, 0)`
    2. **Mau VANG (Yellow):** RGB `(255, 255, 0)`
    3. **Mau XANH LA (Green):** RGB `(0, 255, 0)`
    4. **Mau XANH DUONG (Blue):** RGB `(0, 0, 255)`
    5. **Mau TIM (Purple):** RGB `(128, 0, 128)`
  - Sau khi ket thuc chuoi 5 mau, he thong tu dong lap lai tu dau.
- **Kiem chung:** Mau sac tren NeoPixel chuyen doi ro rang, dung thu tu va khong bi giat.

### Bai 5: Tich hop chuyen che do hoat dong (Interactive Mode Switch)
- **Muc tieu:** Ket hop ngat ngoai hoac may trang thai huu han (FSM) de chuyen doi linh hoat giua cac hieu ung den.
- **Yeu cau thuc hien:**
  - Tich hop them nut bam ngoai hoac phan tich ky tu dieu khien tu cong Serial Monitor.
  - Chuyen doi qua lai giua Che do Don sac, Che do Cau vong va Che do Canh bao khan cap.

---

## 4. Huong dan Bien dich va Nap chuong trinh

Tu thu muc goc cua ma nguon Lab 2 tren PlatformIO:

```bash
# Bien dich chuong trinh
pio run

# Nap vao kit Yolo:UNO
pio run --target upload

# Mo cong Serial Monitor de kiem tra
pio device monitor -b 115200
```

---

## 5. Quy tac ky thuat va Luu y

1. **Do sang NeoPixel:** Khong dat do sang LED WS2812B o muc 100% khi cap nguon tu cong USB vi dong tieu thu cao co the gay sut ap ESP32; khuyen nghi dat `pixels.setBrightness(50)`.
2. **Khai bao bien toan cuc:** Moi bien toan cuc duoc ghi boi mot task va doc boi task khac bat buoc phai khai bao `volatile` de tranh trinh bien dich toi uu hoa gia tri vao thanh ghi.
3. **Stack Size trong FreeRTOS:** Khi su dung `xTaskCreate`, can cap phat ngan xep toi thieu `2048` bytes cho cac tac vu co goi lenh `Serial.printf()` de tranh loi tràn bộ nhớ (Stack Overflow Panic).
