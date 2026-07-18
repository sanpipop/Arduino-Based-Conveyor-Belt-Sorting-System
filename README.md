# AI-Assisted Conveyor Sorting System

An embedded-system prototype that sorts boxes by **color, weight, and damage status**.

The Arduino UNO R3 controls the conveyor, sensors, motors, and servo mechanisms. An ODROID-C4 processes images from a webcam using a TensorFlow Lite classification model and sends the damage result to the Arduino through GPIO.

The prototype processes **one box per sorting cycle**. A new box should be placed on the conveyor only after the previous box has been completely sorted.

---

## System Architecture

```mermaid
flowchart LR
    Box[Box Enters Conveyor] --> IR[IR Sensors]
    Box --> Color[TCS34725 Color Sensor]
    Box --> Weight[Load Cell + HX711]
    Box --> Camera[720p Web Camera]

    Camera --> ODROID[ODROID-C4<br/>Python + TensorFlow Lite]
    ODROID -->|Damage Signal through GPIO| Arduino[Arduino UNO R3]

    IR --> Arduino
    Color --> Arduino
    Weight --> Arduino

    Arduino --> Motors[4 DC Gearbox Motors<br/>2 L298N Drivers]
    Arduino --> Servos[5 SG90 Servo Motors]

    Motors --> Move[Move Box Along Conveyor]
    Servos --> Actuate[Switch Lane or Push Box]
```

---

## How the System Works

1. **Entry detection**  
   The first IR sensor detects a box entering the conveyor. This sensor is handled through an interrupt and can wake the Arduino from power-down sleep mode.

2. **Color and weight measurement**  
   The Arduino reads the box color using a TCS34725 sensor and measures its weight using a 1 kg Load Cell with an HX711 module.

3. **Damage classification**  
   A webcam captures the box image. The ODROID-C4 runs a TensorFlow Lite model to classify the box as `good` or `damage`.

4. **GPIO communication**  
   The ODROID-C4 sends the classification result to the Arduino through GPIO:

   ```text
   0 = good
   1 = damage
   ```

5. **Box data storage**  
   The Arduino stores the detected color, weight category, and damage status of the current box before making the sorting decision.

6. **Sorting decision**  
   Boxes with an unsupported weight or a damaged condition are rejected. Other boxes are routed according to their weight category and detected color.

7. **Mechanical sorting**  
   DC gearbox motors move the conveyor. SG90 servo motors switch lanes or push the box into the required sorting or rejection position.

8. **Cycle completion**  
   The system completes the sorting process for the current box before the next box is placed on the conveyor.

---

## Sorting Logic

```mermaid
flowchart TD
    Start[Box Detected] --> Read[Read Color and Weight]
    Read --> Inspect[Classify Box Condition]

    Inspect --> Damage{Damaged?}
    Damage -->|Yes| Reject[Send to Rejection Path]
    Damage -->|No| Weight{Supported Weight?}

    Weight -->|No| Reject
    Weight -->|Yes| Lane[Select Weight Lane]

    Lane --> Color{Detected Color}
    Color --> Orange[Move to Orange Position]
    Color --> Green[Move to Green Position]
    Color --> Yellow[Move to Yellow Position]

    Orange --> Complete[Sorting Cycle Complete]
    Green --> Complete
    Yellow --> Complete
    Reject --> Complete
```

---

## AI Damage Classification

The image-classification model was trained using **Teachable Machine** and exported as:

```text
model.tflite
labels.txt
```

The `box.py` program runs on the ODROID-C4 and:

1. Captures an image from the webcam
2. Preprocesses the image for the TensorFlow Lite model
3. Runs image classification
4. Determines whether the box is good or damaged
5. Sends the result to the Arduino through GPIO

The AI model is used only for damage classification. Color and weight are measured using dedicated sensors.

---

## Embedded Control Design

### Sensor Handling

- **IR1:** interrupt-based entry detection and wake-up signal
- **IR2–IR9:** sensor polling for box-position detection
- **TCS34725:** box-color detection
- **Load Cell + HX711:** box-weight measurement
- **ODROID GPIO input:** damage result received by the Arduino

### Box Data

For each sorting cycle, the system stores:

```text
color
weight category
damage status
```

The stored data is used only for the current box. The next box should be added after the current sorting cycle is complete.

### Power Management

The Arduino can enter power-down sleep mode when no new box is detected. The first IR sensor wakes the controller when a box enters the system.

---

## Hardware

| Category | Components |
|---|---|
| **Main Controller** | Arduino UNO R3 |
| **AI Processing** | ODROID-C4 |
| **Camera** | 720p Web Camera |
| **Color Detection** | TCS34725 Color Sensor |
| **Weight Detection** | 1 kg Load Cell and HX711 |
| **Position Detection** | 9 Infrared Sensors |
| **Sorting Mechanisms** | 5 SG90 Servo Motors |
| **Conveyor Movement** | 4 DC Gearbox Motors |
| **Motor Control** | 2 L298N Motor Driver Modules |
| **I/O Expansion** | PCF8574 |

---

## Technologies

| Area | Technologies |
|---|---|
| **Embedded Programming** | Arduino, C/C++ |
| **AI Processing** | Python, TensorFlow Lite, Teachable Machine |
| **Controller Communication** | GPIO |
| **Event Handling** | Interrupt and Sensor Polling |
| **Power Management** | Arduino Power-Down Sleep Mode |

---

## Main Source Files

| File | Responsibility |
|---|---|
| `Conveyer_belt.ino` | Main Arduino program |
| `check_color.h` | Reads and classifies box color |
| `check_weight.h` | Reads and classifies box weight |
| `box_damaged_from_odroid.h` | Reads the damage signal from the ODROID-C4 |
| `objectStruct.h` | Defines the stored data for the current box |
| `objectManager.h` | Manages the current box data |
| `laneManager.h` | Controls routing between conveyor lanes |
| `motor_control.h` | Controls the conveyor motors |
| `servo_control.h` | Controls the sorting servos |
| `pcf8574_read_ir.h` | Reads IR sensor states through the PCF8574 |
| `sleep.h` | Manages Arduino sleep behavior |
| `box.py` | Runs TensorFlow Lite classification on the ODROID-C4 |

---

## Running the Prototype

The complete system requires the physical conveyor hardware.

A general setup consists of:

1. Connect the sensors, motors, motor drivers, servos, Arduino, and ODROID-C4 according to the project pin configuration.
2. Upload `Conveyer_belt.ino` to the Arduino UNO R3.
3. Place `box.py`, `model.tflite`, and `labels.txt` on the ODROID-C4.
4. Connect the webcam to the ODROID-C4.
5. Connect the ODROID GPIO output to the Arduino input.
6. Run the Python classification program on the ODROID-C4.
7. Place one box on the conveyor.
8. Wait until the sorting cycle is complete before placing the next box.

---

## Project Scope

This project was developed as an academic embedded-system prototype by a three-person student team.

It demonstrates:

- Arduino and single-board computer integration
- Sensor-based color, weight, and position detection
- TensorFlow Lite image classification on an ODROID-C4
- GPIO communication between controllers
- Processing one box per sorting cycle
- Motor and servo control for conveyor sorting
- Interrupt, polling, and sleep-mode handling

The prototype was not designed as an industrial production system. Its AI model, supported colors, weight rules, and sorting behavior are limited to the conditions used during project development.