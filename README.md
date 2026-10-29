# ☁️ LAB 1: NỀN TẢNG IOT CLOUD VÀ GIAO THỨC MQTT VỚI COREIOT
### Quản lý thiết bị, Telemetry Streaming & Điều khiển hai chiều qua RPC

> **Trường Đại học Bách Khoa - ĐHQG-HCM**  
> **Khoa Khoa học và Kỹ thuật Máy tính**  
> **Môn học:** Internet vạn vật (CO3061 / CO3062)  
> **Sinh viên thực hiện:** Võ Phạm (MSSV: 2313946)  
> **Nhánh Git:** `lab1_coreiot` | **Nền tảng:** CoreIOT Cloud Platform (ThingsBoard)

---

## 📖 Mục lục
- [1. Giới thiệu & Mục tiêu](#1-giới-thiệu--mục-tiêu)
- [2. Cấu trúc thư mục dự án](#2-cấu-trúc-thư-mục-dự-án)
- [3. Thông số máy chủ & Giao thức MQTT](#3-thông-số-máy-chủ--giao-thức-mqtt)
- [4. Nội dung các bài thực hành](#4-nội-dung-các-bài-thực-hành)
  - [Bài 1: Quản lý thiết bị & Khởi tạo Access Token](#bài-1-quản-lý-thiết-bị--khởi-tạo-access-token)
  - [Bài 2: Gửi dữ liệu Telemetry Streaming](#bài-2-gửi-dữ-liệu-telemetry-streaming)
  - [Bài 3: Xây dựng Dashboard giám sát trực quan](#bài-3-xây-dựng-dashboard-giám-sát-trực-quan)
  - [Bài 4: Điều khiển từ xa bằng cơ chế Two-Way RPC](#bài-4-điều-khiển-từ-xa-bằng-cơ-chế-two-way-rpc)
  - [Bài 5: Xuất bản và chia sẻ Dashboard công khai](#bài-5-xuất-bản-và-chia-sẻ-dashboard-công-khai)
- [5. Hướng dẫn cài đặt & Khởi chạy](#5-hướng-dẫn-cài-đặt--khởi-chạy)
- [6. Quy tắc kỹ thuật & Lưu ý quan trọng](#6-quy-tắc-kỹ-thuật--lưu-ý-quan-trọng)

---

## 🎯 1. Giới thiệu & Mục tiêu

Bài thực hành Lab 1 trang bị kiến thức nền tảng về kiến trúc hệ thống IoT chuẩn công nghiệp và cách kết nối thiết bị biên (*Edge Device*) tới đám mây thông qua giao thức **MQTT**:

1. **Kiến trúc 3 tầng IoT tiêu chuẩn:**
   - **Tầng Thu nhận (Perception Layer):** Thiết bị biên thu thập và mô phỏng số liệu cảm biến (Nhiệt độ, Độ ẩm, Cường độ ánh sáng).
   - **Tầng Mạng (Network Layer):** Giao thức truyền thông MQTT trên nền TCP/IP, kiến trúc Publish - Subscribe nhẹ, tiết kiệm tài nguyên.
   - **Tầng Ứng dụng (Application Layer):** Đám mây CoreIOT (nền tảng ThingsBoard) thu thập, lưu trữ dữ liệu chuỗi thời gian (*Time-series*) và trực quan hóa.
2. **Cơ chế xác thực an toàn:** Sử dụng Device Access Token độc bản để định danh thiết bị.
3. **Truyền nhận dữ liệu thời gian thực:** Định dạng gói tin Telemetry dạng JSON Payload với chu kỳ cố định 3 giây.
4. **Điều khiển hai chiều từ xa (Bidirectional RPC):** Nhận lệnh điều khiển trực tiếp từ Web Dashboard xuống thiết bị và phản hồi trạng thái xác nhận.
5. **Phân quyền và chia sẻ:** Quản lý nhóm thiết bị (*Device Groups*), cấp quyền khách hàng (*Customer Assignment*) và tạo liên kết truy cập công khai (*Public Dashboard*).

---

## 📁 2. Cấu trúc thư mục dự án

```text
Report_IOT_Lab1/
├── lab1_device.py                      # Mã nguồn Python client giả lập thiết bị IoT
├── LAB1_2313946.pdf                    # Báo cáo thí nghiệm hoàn chỉnh (PDF)
├── LAB1_CoreIOT_Exercises_Report_VI.docx # Báo cáo thí nghiệm định dạng Word
├── Report_IOT_Lab1/                    # Mã nguồn LaTeX bài báo cáo
│   ├── main.tex                        # File LaTeX chính
│   ├── bkthesis.sty                    # Template định dạng luận văn ĐHBK
│   ├── content/exercises.tex           # Nội dung giải thích từng bài tập
│   └── image/                          # Hình ảnh minh họa kết quả thực nghiệm
└── README.md                           # Tài liệu hướng dẫn kỹ thuật Lab 1
```

---

## 🌐 3. Thông số máy chủ & Giao thức MQTT

Hệ thống kết nối trực tiếp tới máy chủ Cloud Broker CoreIOT:

| Thông số cấu hình | Giá trị | Giải thích kỹ thuật |
| :--- | :--- | :--- |
| **MQTT Broker Host** | `app.coreiot.io` | Địa chỉ máy chủ Cloud CoreIOT |
| **Cổng kết nối (Port)** | `1883` | Cổng TCP truyền thông tiêu chuẩn (Plaintext) |
| **Phương thức xác thực** | `Device Access Token` | Chuỗi ký tự bảo mật riêng cho thiết bị (≥ 12 ký tự) |
| **Topic gửi Telemetry** | `v1/devices/me/telemetry` | Publish dữ liệu đo đạc lên máy chủ |
| **Topic đăng ký nhận RPC** | `v1/devices/me/rpc/request/+` | Subscribe lắng nghe yêu cầu điều khiển từ Dashboard |
| **Topic phản hồi kết quả RPC**| `v1/devices/me/rpc/response/{id}` | Publish kết quả thực thi lệnh về Dashboard |

---

## 📊 4. Nội dung các bài thực hành

### 🛠️ Bài 1: Quản lý thiết bị & Khởi tạo Access Token
- **Mục tiêu:** Thiết lập cấu hình thiết bị ban đầu trên giao diện quản trị CoreIOT.
- **Yêu cầu thực hiện:**
  - Tạo **Device Profile** phù hợp với yêu cầu quản lý.
  - Khởi tạo thiết bị với Access Token chứa MSSV: `Vo_Lab1_Token_2313946`.
  - Gom nhóm vào **Device Group** và phân quyền truy cập.
- **Kiểm chứng:** Trạng thái thiết bị chuyển sang **Active** ngay khi có kết nối MQTT đầu tiên.

---

### 📡 Bài 2: Gửi dữ liệu Telemetry Streaming
- **Mục tiêu:** Xây dựng script Python gửi dữ liệu cảm biến định kỳ lên Cloud.
- **Yêu cầu thực hiện:**
  - Sử dụng thư viện `paho-mqtt` để kết nối và truyền tin.
  - Thu thập hoặc mô phỏng 3 thông số: Nhiệt độ (°C), Độ ẩm (%RH), Ánh sáng (Lux).
  - Chu kỳ phát tin (*Publish Rate*): **3 giây / lần**.
  - Cấu trúc gói tin JSON:
    ```json
    {
      "temperature": 28.5,
      "humidity": 65.2,
      "light": 420
    }
    ```
- **Kiểm chứng:** Tab **Latest Telemetry** trên giao diện CoreIOT cập nhật liên tục kèm nhãn thời gian (*Timestamp*) tương ứng.

---

### 📈 Bài 3: Xây dựng Dashboard giám sát trực quan
- **Mục tiêu:** Thiết kế giao diện Dashboard theo chuẩn công nghiệp để giám sát số liệu trực quan.
- **Các Widget cấu hình:**
  - **Digital Gauge Widget:** Hiển thị tức thời Nhiệt độ và Độ ẩm kèm dải màu cảnh báo (Xanh: Bình thường, Đỏ: Cảnh báo).
  - **Card Status Widget:** Thể hiện giá trị cường độ ánh sáng thời gian thực.
  - **Time-series Line Chart:** Biểu đồ đường vẽ lịch sử biến thiên của dữ liệu theo thời gian thực, hỗ trợ phóng to/thu nhỏ và lọc khoảng thời gian.
- **Kiểm chứng:** Dữ liệu trên Dashboard cập nhật tức thì theo chu kỳ truyền tin của client.

---

### 🔄 Bài 4: Điều khiển từ xa bằng cơ chế Two-Way RPC
- **Mục tiêu:** Thiết lập cơ chế điều khiển hai chiều thời gian thực giữa Web Dashboard và thiết bị chấp hành.
- **Yêu cầu thực hiện:**
  - Cấu hình các công tắc gạt (*Control Switch*) điều khiển 3 ngoại vi: `LED`, `PUMP` (Bơm), `FAN` (Quạt).
  - Client subscribe vào topic `v1/devices/me/rpc/request/+`.
  - Nhận lệnh RPC dạng JSON từ máy chủ:
    ```json
    {
      "method": "setDeviceState",
      "params": { "device": "PUMP", "status": true }
    }
    ```
  - Xử lý bật/tắt thiết bị và phản hồi bắt buộc về `v1/devices/me/rpc/response/{request_id}`:
    ```json
    { "success": true, "device": "PUMP", "status": true }
    ```
- **Kiểm chứng:** Nút gạt trên Dashboard giữ nguyên trạng thái xác nhận, terminal thiết bị in thông báo đổi trạng thái tức thì.

---

### 🌐 Bài 5: Xuất bản và chia sẻ Dashboard công khai
- **Mục tiêu:** Xuất bản Dashboard ra ngoài Internet để giám sát mà không cần tài khoản quản trị viên.
- **Yêu cầu thực hiện:**
  - Kích hoạt tính năng **Make Dashboard Public** trong cài đặt Dashboard của CoreIOT.
  - Trích xuất đường dẫn Public URL kèm token truy cập công khai.
  - Kiểm thử mở trên trình duyệt ẩn danh (*Incognito Mode*).
- **Kiểm chứng:** Người dùng bất kỳ có thể theo dõi đầy đủ biểu đồ và số liệu cảm biến mà không phải đăng nhập.

---

## 🚀 5. Hướng dẫn cài đặt & Khởi chạy

Chạy thiết bị giả lập IoT bằng Python từ thư mục gốc của dự án:

```bash
# 1. Cài đặt thư viện giao tiếp MQTT
pip install paho-mqtt

# 2. Khởi chạy thiết bị giả lập CoreIOT
python lab1_device.py
```

> **Lưu ý:** Trước khi chạy, đảm bảo hằng số `ACCESS_TOKEN` trong file `lab1_device.py` trùng khớp với Token bạn đã tạo trên Dashboard CoreIOT.

---

## ⚙️ 6. Quy tắc kỹ thuật & Lưu ý quan trọng

1. **Bảo mật Access Token:** Không chia sẻ hoặc đưa Token thiết bị lên các kho lưu trữ mã nguồn mở không bảo mật. Token nên có độ dài tối thiểu 12 ký tự để phòng ngừa tấn công dò quét (*brute-force*).
2. **Cấp độ dịch vụ (QoS):** Dữ liệu Telemetry thường truyền với `QoS = 0` hoặc `QoS = 1` để tối ưu hóa băng thông đường truyền và giảm thiểu độ trễ.
3. **RPC Timeout:** Thiết bị cần phản hồi lại yêu cầu RPC trong vòng tối đa **5000 ms**; nếu quá thời gian này máy chủ sẽ hủy yêu cầu và nút gạt trên Dashboard sẽ tự động quay về trạng thái ban đầu.
