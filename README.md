# LAB 1: IOT CLOUD PLATFORM AND MQTT PROTOCOL WITH COREIOT

Khoa Khoa hoc va Ky thuat May tinh - Truong Dai hoc Bach Khoa, DHQG-HCM  
Mon hoc: Internet van vat (CO3061 / CO3062)  
Nhanh Git: `lab1_coreiot`  
Nen tang: CoreIOT Cloud Platform (ThingsBoard based)  
Giao thuc truyen thong: MQTT (Message Queuing Telemetry Transport)  

---

## 1. Muc tieu thi nghiem

1. Nam vung kien truc 3 tang tieu chuan cua he thong Internet van vat:
   - **Tang Thu nhan (Perception Layer):** Thiet bi bien thu thap tin hieu tu cam bien nhiet do, do am va anh sang.
   - **Tang Mang (Network Layer):** Giao thuc mang MQTT chay tren nen tang TCP/IP, mo hinh Publish - Subscribe nhe va tiet kiem bang thong.
   - **Tang Ung dung (Application Layer):** May chu dam may CoreIOT tiep nhan, luu tru chuoi thoi gian (Time-series data) va truc quan hoa du lieu tren Dashboard.
2. Thuc hanh xac thuc thiet bi bang Access Token bao mat.
3. Nam vung chu ky va dinh dang goi tin Telemetry (JSON Payload).
4. Thiet lap co che goi thu tuc tu xa hai chieu (Bidirectional Remote Procedure Call - RPC) de dieu khien co cau chap hanh tu Dashboard.
5. Quan ly phan quyen nguoi dung (Customer Assignment) va chia se giao dien Dashboard cong khai (Public URL).

---

## 2. Thong so may chu va Giao thuc MQTT

He thong ket noi truc tiep toi may chu Cloud Broker tieu chuan cua khoa hoc:

| Thong so | Gia tri cau hinh | Giai thich ky thuat |
| :--- | :--- | :--- |
| MQTT Broker Host | `app.coreiot.io` | Dia chi may chu may chu Cloud CoreIOT |
| Cong TCP tieu chuan | `1883` | Cong truyen thong MQTT Plaintext |
| Thong tin xac thuc | Device Access Token | Token do nguoi dung tao (Toi thieu 12 ky tu) |
| Topic gui Telemetry | `v1/devices/me/telemetry` | Publish du lieu cam bien len may chu |
| Topic dang ky nhan RPC | `v1/devices/me/rpc/request/+` | Subscribe de nhan yeu cau dieu khien tu Dashboard |
| Topic phan hoi ket qua RPC | `v1/devices/me/rpc/response/{id}` | Publish ket qua phan hoi trang thai thiet bi ve Dashboard |

---

## 3. Danh muc cac bai tap thuc hanh va Mo ta chi tiet

### Bai 1: Tao thiet bi, thong tin xac thuc va nhom thiet bi (Device Management)
- **Muc tieu:** Lam quen voi giao dien quan tri cua he thong CoreIOT.
- **Yeu cau thuc hien:**
  - Dang nhap he thong, tao mot Device Profile phu hop voi yeu cau quan ly.
  - Khoi tao mot thiet bi moi voi dinh dang Access Token chua ma so sinh vien (Vi du: `Vo_Lab1_Token_2313946`).
  - Tao Nhom thiet bi (Device Group) va phan quyen thiet bi vao nhom tuong ung.
- **Kiem chung:** Trang thai thiet bi tren giao dien CoreIOT chuyen sang Active khi co ket noi dau tien.

### Bai 2: Gui Telemetry tu Python kem cam bien anh sang gia lap (Telemetry Streaming)
- **Muc tieu:** Xay dung thiet bi bien gia lap gui du lieu dinh ky len Cloud qua MQTT.
- **Yeu cau thuc hien:**
  - Su dung thu vien `paho-mqtt` tren Python de thiet lap ket noi toi `app.coreiot.io:1883`.
  - Thu thap hoac gia lap 3 thong so moi truong: Nhiet do (°C), Do am (%), Cuong do anh sang (Lux).
  - Chu ky truyen gui du lieu (Publish Rate) bat buoc la **3 giay/lan**.
  - Dinh dang goi tin JSON Payload:
    ```json
    {
      "temperature": 28.5,
      "humidity": 65.2,
      "light": 420
    }
    ```
- **Kiem chung:** Tab **Latest Telemetry** tren thiet bi tai CoreIOT cap nhat cac gia tri do moi sau moi 3 giay kem theo dau thoi gian (Timestamp) chinh xac.

### Bai 3: Xay dung Dashboard giam sat truc quan (Visualization Dashboard)
- **Muc tieu:** Thiet ke giao dien giam sat truc quan theo tieu chuan cong nghiep.
- **Yeu cau thuc hien:**
  - Tao mot Dashboard moi gan voi Alias cua thiet bi da khoi tao o Bai 1.
  - Cau hinh cac Widget truc quan hoa:
    - **Digital Gauge Widget:** Hien thi gia tri tuc thoi cua Nhiet do va Do am voi cac dai mau canh bao truc quan (Xanh: Ly tuong, Do: Nguy hiem).
    - **Card Status Widget:** Hien thi cuong do anh sang tuc thoi.
    - **Time-series Line Chart:** Bieu do duong the hien su bien thien cua cac thong so theo thoi gian thuc, cho phep phong to/thu nho va chon khoang thoi gian khao sat.
- **Kiem chung:** Dashboard tu dong cap nhat dong moi khi thiet bi bien gui goi tin Telemetry len.

### Bai 4: Dieu khien thiet bi tu xa bang co che RPC (Two-Way RPC Control)
- **Muc tieu:** Hien thuc co che dieu khien hai chieu thoi gian thuc tu Web Dashboard ve thiet bi chap hanh.
- **Yeu cau thuc hien:**
  - Bo sung cac Widget nut gat (Control Switch) tren Dashboard de dieu khien 3 thiet bi ngoai vi: `LED`, `PUMP` (May bom), `FAN` (Quat gio).
  - Thiet bi bien subscribe vao topic `v1/devices/me/rpc/request/+`.
  - Khi nguoi dung thao tac tren Dashboard, may chu Cloud gui yeu cau RPC co cau truc:
    ```json
    {
      "method": "setDeviceState",
      "params": { "device": "PUMP", "status": true }
    }
    ```
  - Thiet bi trich xuat `request_id`, cap nhat trang thai phan cung va bat buoc phai publish goi phan hoi ve topic `v1/devices/me/rpc/response/{request_id}`:
    ```json
    { "success": true, "device": "PUMP", "status": true }
    ```
- **Kiem chung:** Nut gat tren Dashboard khong bi nhay nguoc ve vi tri cu (nho nhan duoc phan hoi hop le), thiet bi chap hanh chuyen trang thai tuc thi.

### Bai 5: Chia se Dashboard cong khai va quan ly phan quyen (Public Dashboard Access)
- **Muc tieu:** Xuat ban giao dien Dashboard de giam sat tu xa ma khong can dang nhap tai khoan quan tri.
- **Yeu cau thuc hien:**
  - Bat tinh nang **Make Dashboard Public** trong phan quan tri CoreIOT.
  - Sao chep duong dan lien ket cong khai (Public URL kem token truy cap).
  - Thu nghiem mo tren trinh duyet an danh (Incognito Mode) de dam bao bat ky nguoi dung nao co lien ket deu co the xem duoc so lieu thoi gian thuc.
- **Kiem chung:** Dashboard hien thi day du cac widget do dac ma khong yeu cau nhap Mat khau hay Token quan tri.

---

## 4. Huong dan chay Ma nguon Kiem thu (Python Client)

Tu thu muc goc cua nhanh, thuc hien cai dat thu vien va khoi chay thiet bi:

```bash
# Cai dat thu vien paho-mqtt
pip install paho-mqtt

# Chay thiet bi gia lap IoT
python lab1_device.py
```

---

## 5. Quy tac ky thuat va Luu y quan trong

1. **Access Token bao mat:** Khong de lo Token thiet bi ra ngoai. Token phai dai toi thieu 12 ky tu de tranh bi brute-force.
2. **QoS (Quality of Service):** Truyen Telemetry nen su dung `QoS = 0` (hoac `QoS = 1`) de dam bao toc do toi uu.
3. **RPC Timeout:** Neu thiet bi khong gui phan hoi ve `v1/devices/me/rpc/response/{id}` trong vong 5000 ms, Dashboard se bao loi Timeout va giao dien nut gat se bi tra nguoc lai ve trang thai cu.
