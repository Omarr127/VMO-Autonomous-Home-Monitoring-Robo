# 🤖 VMO – Autonomous Home Pet & Safety Monitoring Robot

> **An autonomous robotics platform combining Embedded Systems, ROS 2, Edge AI, IoT, and Computer Vision for smart home monitoring.**

![Platform](https://img.shields.io/badge/Platform-Raspberry%20Pi%204-red)
![ROS2](https://img.shields.io/badge/ROS-2%20Humble-blue)
![ESP32](https://img.shields.io/badge/ESP32-S3-black)
![PIC](https://img.shields.io/badge/PIC16F877A-Microcontroller-green)
![FreeRTOS](https://img.shields.io/badge/RTOS-FreeRTOS-orange)
![Language](https://img.shields.io/badge/C%2FC%2B%2B-Python-success)

---

# 📖 Overview

VMO (Virtual Monitoring Operator) is an autonomous mobile robot designed to function as both a **home safety monitor** and an **interactive robotic companion**.

The system combines multiple processing layers to achieve deterministic real-time control, intelligent perception, autonomous navigation, and cloud connectivity.

Unlike conventional embedded projects that rely on a single controller, VMO distributes computation across four specialized processing units to maximize performance and reliability.

---

# ✨ Features

- 🤖 Autonomous mobile navigation
- 👤 Human detection and tracking
- 📷 Computer vision using YOLO
- 🧠 ROS 2 autonomous control
- 📡 IoT communication
- ☁️ Firebase cloud synchronization
- 🌐 ESP32 web interface
- 🎤 Voice interaction using Speech Recognition + Gemini
- 🌡️ Environmental monitoring
- 🔥 Gas leakage detection
- 🚧 Obstacle avoidance
- 📱 Android-based expressive user interface

---

# 🏗 System Architecture

The robot is organized into four processing layers.

```text
                Android Smartphone
          (Face • Voice • Expressions)

                     │
                     ▼

             Raspberry Pi 4
      ROS 2 + Computer Vision + AI
      Navigation + Cloud Services

                     │
              USB Serial (115200)

                     ▼

              ESP32-S3 (FreeRTOS)
        Communication & Task Scheduler

                     │
             UART (9600 baud)

                     ▼

              PIC16F877A
      Real-Time Sensors & Motor Control
```

Each processor performs only the tasks it is best suited for:

| Layer | Responsibility |
|--------|---------------|
| Android | User Interface & Voice Assistant |
| Raspberry Pi 4 | AI, Computer Vision, ROS 2 |
| ESP32-S3 | Communication & Task Scheduling |
| PIC16F877A | Real-time Motor and Sensor Control |

---

# 🧠 Technologies Used

### Robotics

- ROS 2 Humble
- Differential Drive Kinematics
- Subsumption Architecture
- Docker

### Embedded Systems

- PIC16F877A
- ESP32-S3
- FreeRTOS
- Embedded C
- UART
- PWM
- ADC
- Timers
- Interrupts

### Artificial Intelligence

- YOLO26 Nano
- ONNX Runtime
- OpenCV
- Computer Vision

### Cloud & Networking

- Firebase
- Google Speech API
- Gemini API
- UDP
- Protobuf
- COBS Serialization

### Hardware Design

- KiCad PCB
- Proteus
- Custom PCB Design

---

# 🔌 Hardware Components

## Processing Units

- Raspberry Pi 4
- ESP32-S3
- PIC16F877A

## Sensors

- MQ-2 Gas Sensor
- DHT11 Temperature & Humidity Sensor
- HC-SR04 Ultrasonic Sensors
- Optical Wheel Encoders

## Actuators

- Differential Drive Motors
- L298N Motor Driver

## Power System

- 3S LiPo Battery
- Battery Management System
- Dual Buck Regulators
- INA226 Power Monitor

---

# ⚙️ Software Architecture

```
Application Layer
        │
ROS 2 Nodes
        │
Communication Middleware
        │
ESP32 FreeRTOS Tasks
        │
PIC Firmware
        │
Hardware Drivers
```

The firmware follows a layered architecture separating hardware abstraction, middleware, communication, and application logic for improved scalability and maintainability.

---

# 📂 Repository Structure

```
VMO-Autonomous-Home-Monitoring-Robot

├── Report/
│   └── Technical Documentation.pdf
│
├── Software_and_Firmware/
│   ├── Dockerfile
│   ├── ESP32_Code/
│   └── PIC_Code/
│
├── Hardware_and_Simulation/
│   ├── PCB Design
│   └── Proteus Simulation
│
└── README.md
```

---

# 🚀 Key Engineering Challenges

During development, several real-world engineering challenges were addressed, including:

- Electrical noise from motor switching
- Serial communication reliability
- Multi-controller synchronization
- Ultrasonic sensor cross-talk
- Embedded memory limitations
- Raspberry Pi thermal management
- Real-time scheduling
- Logic level conversion between 5V and 3.3V systems

---

# 📈 Future Improvements

- Visual-Inertial Odometry (VIO)
- Autonomous charging dock
- Smartphone NPU acceleration
- Improved autonomous navigation
- Mobile application
- SLAM integration

---

# 👨‍💻 Team

- Matthew Nader
- **Omar Tamer**
- Mohamed Gadwal
- Amr Emad Mohamed
- Mohamed Magdy

---

# 📚 Course

**CIE 349 – Embedded Systems**

Computer and Information Engineering

---

# 📄 License

This project is shared for educational and portfolio purposes.
