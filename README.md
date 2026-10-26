# LAB 1: NỀN TẢNG IOT CLOUD VÀ GIAO THỨC MQTT VỚI COREIOT

Khoa Khoa học và Kỹ thuật Máy tính - Trường Đại học Bách Khoa, ĐHQG-HCM  
Môn học: Internet vạn vật (CO3061 / CO3062)  
Nhánh Git: `lab1_coreiot` (hoặc `lab1`)  
Nền tảng: CoreIOT Cloud Platform (phát triển trên nền ThingsBoard)  
Giao thức truyền thông: MQTT (Message Queuing Telemetry Transport)  

---

## 1. Mục tiêu thí nghiệm

1. Nắm vững kiến trúc 3 tầng tiêu chuẩn của hệ thống Internet vạn vật:
   - **Tầng Thu nhận (Perception Layer):** Thiết bị biên thu thập tín hiệu từ cảm biến nhiệt độ, độ ẩm và ánh sáng.
   - **Tầng Mạng (Network Layer):** Giao thức mạng MQTT hoạt động trên nền TCP/IP, mô hình Publish - Subscribe nhẹ, tiết kiệm băng thông.
   - **Tầng Ứng dụng (Application Layer):** Máy chủ đám mây CoreIOT tiếp nhận, lưu trữ dữ liệu chuỗi thời gian (Time-series data) và trực quan hóa trên Dashboard.
2. Thực hành xác thực thiết bị bằng Access Token bảo mật.
3. Nắm vững chu kỳ và định dạng gói tin Telemetry (JSON Payload).
4. Thiết lập cơ chế gọi thủ tục từ xa hai chiều (Bidirectional Remote Procedure Call - RPC) để điều khiển cơ cấu chấp hành từ Dashboard.
5. Quản lý phân quyền người dùng (Customer Assignment) và xuất bản giao diện Dashboard công khai (Public URL).

---

## 2. Thông số máy chủ và Giao thức MQTT

Hệ thống kết nối trực tiếp tới máy chủ Cloud Broker tiêu chuẩn của khóa học:

| Thông số | Giá trị cấu hình | Giải thích kỹ thuật |
| :--- | :--- | :--- |
| MQTT Broker Host | `app.coreiot.io` | Địa chỉ máy chủ Cloud CoreIOT |
| Cổng TCP tiêu chuẩn | `1883` | Cổng truyền thông MQTT Plaintext |
| Thông tin xác thực | Device Access Token | Token do người dùng tạo (Tối thiểu 12 ký tự) |
| Topic gửi Telemetry | `v1/devices/me/telemetry` | Publish dữ liệu cảm biến lên máy chủ |
| Topic đăng ký nhận RPC | `v1/devices/me/rpc/request/+` | Subscribe để nhận yêu cầu điều khiển từ Dashboard |
| Topic phản hồi kết quả RPC | `v1/devices/me/rpc/response/{id}` | Publish kết quả phản hồi trạng thái thiết bị về Dashboard |

---

## 3. Danh mục các bài tập thực hành và Mô tả chi tiết

### Bài 1: Tạo thiết bị, thông tin xác thực và nhóm thiết bị (Device Management)
- **Mục tiêu:** Làm quen với giao diện quản trị của hệ thống CoreIOT.
- **Yêu cầu thực hiện:**
  - Đăng nhập hệ thống, tạo một Device Profile phù hợp với yêu cầu quản lý.
  - Khởi tạo một thiết bị mới với định dạng Access Token chứa mã số sinh viên (Ví dụ: `Vo_Lab1_Token_2313946`).
  - Tạo Nhóm thiết bị (Device Group) và phân quyền thiết bị vào nhóm tương ứng.
- **Kiểm chứng:** Trạng thái thiết bị trên giao diện CoreIOT chuyển sang Active khi có kết nối đầu tiên.

### Bài 2: Gửi Telemetry từ Python kèm cảm biến ánh sáng giả lập (Telemetry Streaming)
- **Mục tiêu:** Xây dựng thiết bị biên giả lập gửi dữ liệu định kỳ lên Cloud qua MQTT.
- **Yêu cầu thực hiện:**
  - Sử dụng thư viện `paho-mqtt` trên Python để thiết lập kết nối tới `app.coreiot.io:1883`.
  - Thu thập hoặc giả lập 3 thông số môi trường: Nhiệt độ (°C), Độ ẩm (%), Cường độ ánh sáng (Lux).
  - Chu kỳ truyền gửi dữ liệu (Publish Rate) bắt buộc là **3 giây/lần**.
  - Định dạng gói tin JSON Payload:
    ```json
    {
      "temperature": 28.5,
      "humidity": 65.2,
      "light": 420
    }
    ```
- **Kiểm chứng:** Tab **Latest Telemetry** trên thiết bị tại CoreIOT cập nhật các giá trị đo mới sau mỗi 3 giây kèm theo dấu thời gian (Timestamp) chính xác.

### Bài 3: Xây dựng Dashboard giám sát trực quan (Visualization Dashboard)
- **Mục tiêu:** Thiết kế giao diện giám sát trực quan theo tiêu chuẩn công nghiệp.
- **Yêu cầu thực hiện:**
  - Tạo một Dashboard mới gắn với Alias của thiết bị đã khởi tạo ở Bài 1.
  - Cấu hình các Widget trực quan hóa:
    - **Digital Gauge Widget:** Hiển thị giá trị tức thời của Nhiệt độ và Độ ẩm với các dải màu cảnh báo trực quan (Xanh: Lý tưởng, Đỏ: Nguy hiểm).
    - **Card Status Widget:** Hiển thị cường độ ánh sáng tức thời.
    - **Time-series Line Chart:** Biểu đồ đường thể hiện sự biến thiên của các thông số theo thời gian thực, cho phép phóng to/thu nhỏ và chọn khoảng thời gian khảo sát.
- **Kiểm chứng:** Dashboard tự động cập nhật động mỗi khi thiết bị biên gửi gói tin Telemetry lên.

### Bài 4: Điều khiển thiết bị từ xa bằng cơ chế RPC (Two-Way RPC Control)
- **Mục tiêu:** Hiện thực cơ chế điều khiển hai chiều thời gian thực từ Web Dashboard về thiết bị chấp hành.
- **Yêu cầu thực hiện:**
  - Bổ sung các Widget nút gạt (Control Switch) trên Dashboard để điều khiển 3 thiết bị ngoại vi: `LED`, `PUMP` (Máy bơm), `FAN` (Quạt gió).
  - Thiết bị biên subscribe vào topic `v1/devices/me/rpc/request/+`.
  - Khi người dùng thao tác trên Dashboard, máy chủ Cloud gửi yêu cầu RPC có cấu trúc:
    ```json
    {
      "method": "setDeviceState",
      "params": { "device": "PUMP", "status": true }
    }
    ```
  - Thiết bị trích xuất `request_id`, cập nhật trạng thái phần cứng và bắt buộc phải publish gói phản hồi về topic `v1/devices/me/rpc/response/{request_id}`:
    ```json
    { "success": true, "device": "PUMP", "status": true }
    ```
- **Kiểm chứng:** Nút gạt trên Dashboard không bị nhảy ngược về vị trí cũ (nhờ nhận được phản hồi hợp lệ), thiết bị chấp hành chuyển trạng thái tức thì.

### Bài 5: Chia sẻ Dashboard công khai và quản lý phân quyền (Public Dashboard Access)
- **Mục tiêu:** Xuất bản giao diện Dashboard để giám sát từ xa mà không cần đăng nhập tài khoản quản trị.
- **Yêu cầu thực hiện:**
  - Bật tính năng **Make Dashboard Public** trong phần quản trị CoreIOT.
  - Sao chép đường dẫn liên kết công khai (Public URL kèm token truy cập).
  - Thử nghiệm mở trên trình duyệt ẩn danh (Incognito Mode) để đảm bảo bất kỳ người dùng nào có liên kết đều có thể xem được số liệu thời gian thực.
- **Kiểm chứng:** Dashboard hiển thị đầy đủ các widget đo đạc mà không yêu cầu nhập Mật khẩu hay Token quản trị.

---

## 4. Hướng dẫn chạy Mã nguồn Kiểm thử (Python Client)

Từ thư mục gốc của nhánh, thực hiện cài đặt thư viện và khởi chạy thiết bị:

```bash
# Cài đặt thư viện paho-mqtt
pip install paho-mqtt

# Chạy thiết bị giả lập IoT
python lab1_device.py
```

---

## 5. Quy tắc kỹ thuật và Lưu ý quan trọng

1. **Access Token bảo mật:** Không để lộ Token thiết bị ra ngoài. Token phải dài tối thiểu 12 ký tự để tránh bị brute-force.
2. **QoS (Quality of Service):** Truyền Telemetry nên sử dụng `QoS = 0` (hoặc `QoS = 1`) để đảm bảo tốc độ tối ưu.
3. **RPC Timeout:** Nếu thiết bị không gửi phản hồi về `v1/devices/me/rpc/response/{id}` trong vòng 5000 ms, Dashboard sẽ báo lỗi Timeout và giao diện nút gạt sẽ bị trả ngược lại về trạng thái cũ.
