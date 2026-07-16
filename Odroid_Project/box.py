import cv2
import numpy as np
import tflite_runtime.interpreter as tflite
import gpiod
from gpiod import Chip, Line
import time

# -------------------
# ตั้งค่า GPIO (BOARD Pin 12 / GPIOX16)
# -------------------
chip = Chip('gpiochip1')       # ใช้ gpiochip1
line_offset = 70               # line offset ของ BOARD Pin 12
gpio_line = chip.get_line(line_offset)
#gpio_line.request(consumer="tflite_app", type=Line.DIRECTION_OUTPUT)
gpio_line.request(consumer="odroid_out", type=gpiod.LINE_REQ_DIR_OUT)
# -------------------
# โหลดโมเดล
# -------------------
interpreter = tflite.Interpreter(model_path="model.tflite")
interpreter.allocate_tensors()
input_details = interpreter.get_input_details()
output_details = interpreter.get_output_details()

# โหลด labels
with open("labels.txt", "r") as f:
    labels = [line.strip() for line in f.readlines()]

# -------------------
# เปิด webcam
# -------------------
cap = cv2.VideoCapture(0)

try:
    while True:
        ret, frame = cap.read()
        if not ret:
            break

        # Resize ให้ตรงกับ input shape ของโมเดล
        h, w = input_details[0]['shape'][1:3]
        img = cv2.resize(frame, (w, h))

        # แปลง dtype ให้ตรงกับที่โมเดลต้องการ
        if input_details[0]['dtype'] == np.uint8:
            img = np.expand_dims(img.astype(np.uint8), axis=0)
        else:
            img = np.expand_dims(img.astype(np.float32) / 255.0, axis=0)

        # ใส่ input เข้าโมเดล
        interpreter.set_tensor(input_details[0]['index'], img)
        interpreter.invoke()

        # ดึง output
        output_data = interpreter.get_tensor(output_details[0]['index'])
        pred_idx = np.argmax(output_data)
        label = labels[pred_idx].lower().split()[-1]  # good หรือ damage

        # -------------------
        # ส่งสัญญาณไป GPIOX16
        # -------------------
        try:
            gpio_line.request(consumer="tflite_app", type=Line.DIRECTION_OUTPUT)
        except Exception:
            pass

        if label == "good":
            gpio_line.set_value(0)  # HIGH
            print(label)
        else:  # damage
            gpio_line.set_value(1)  # LOW
            print(label)

        time.sleep(0.1)  # ลดโหลด CPU
        
finally:
    cap.release()
    # cleanup GPIO
    try:
        gpio_line.request(consumer="tflite_app", type=Line.DIRECTION_OUTPUT)
        gpio_line.set_value(0)
    except:
        pass
    gpio_line.release()
    print("GPIO cleanup done")
