# lab1_device.py - Thiết bị giả lập CoreIOT cho LAB 1
import json
import random
import time
import paho.mqtt.client as mqtt

BROKER = "app.coreiot.io"
PORT = 1883
# THAY BẰNG TOKEN BẠN TỰ TẠO (>= 12 ký tự, ví dụ: Lab1_Vo_2313946_Token)
ACCESS_TOKEN = "Vo_Lab1_Token_2313946"

# Trạng thái ban đầu của 3 thiết bị chấp hành
state = {"LED": False, "PUMP": False, "FAN": False}

def on_connect(client, userdata, flags, reason_code, properties):
    if reason_code == 0:
        # print("[MQTT] Ket noi thanh cong toi CoreIOT!")
        # Đăng ký nhận lệnh RPC từ Dashboard
        client.subscribe("v1/devices/me/rpc/request/+")
    else:
        print(f"[MQTT] Ket noi that bai, reason code: {reason_code}")

def on_message(client, userdata, msg):
    request_id = msg.topic.split("/")[-1]
    try:
        req = json.loads(msg.payload)
    except json.JSONDecodeError:
        print("[RPC] Payload JSON khong hop le:", msg.payload)
        return

    method = req.get("method", "")
    params = req.get("params")
    result = None

    # Xử lý getState (Đọc trạng thái hiện tại khi reload Dashboard)
    if method.startswith("getState"):
        name = method[len("getState"):]
        result = state.get(name, False)

    # Xử lý setState (Nhận lệnh bật/tắt từ công tắc Dashboard)
    elif method.startswith("setState"):
        name = method[len("setState"):]
        if name in state:
            state[name] = bool(params)
            result = state[name]
            # In thông báo trạng thái rõ ràng ra terminal theo yêu cầu Bài 4
            status_str = "ON" if result else "OFF"
            print(f"[DIEU KHIEN] {name} -> {status_str}")
            # Cập nhật client attribute lên CoreIOT
            client.publish("v1/devices/me/attributes", json.dumps({name.lower() + "_state": result}))

    # Phản hồi bắt buộc về topic response để Switch trên Dashboard không bị Timeout
    client.publish(f"v1/devices/me/rpc/response/{request_id}", json.dumps(result), qos=1)

# Khởi tạo MQTT Client với paho-mqtt 2.x
client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, client_id="LAB1_2313946_CLIENT")
client.username_pw_set(ACCESS_TOKEN)  # Với ThingsBoard/CoreIOT, Token chính là Username
client.on_connect = on_connect
client.on_message = on_message

client.connect(BROKER, PORT, keepalive=60)
client.loop_start()

try:
    while True:
        # Yêu cầu Bài 2: nhiệt độ, độ ẩm và cảm biến ánh sáng light (0 - 1000)
        temp = round(random.uniform(25.0, 35.0), 1)
        humi = round(random.uniform(50.0, 85.0), 1)
        light = random.randint(0, 1000)

        payload = {
            "temperature": temp,
            "humidity": humi,
            "light": light
        }
        client.publish("v1/devices/me/telemetry", json.dumps(payload), qos=1)
        print(f"Sent: {payload}")
        
        # Yêu cầu Bài 2: Đổi chu kỳ gửi thành 3 giây
        time.sleep(3)
except KeyboardInterrupt:
    print("\nDang ngat ket noi...")
finally:
    client.loop_stop()
    client.disconnect()
    print("Da thoat an toan.")
