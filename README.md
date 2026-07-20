# 🤖 VMO – Autonomous Home Pet & Safety Monitoring Robot

<div align="center">

### Autonomous Robotics • Embedded Systems • Edge AI • ROS 2 • IoT

![ROS2](https://img.shields.io/badge/ROS-2%20Humble-22314E?logo=ros)
![ESP32](https://img.shields.io/badge/ESP32-S3-E7352C?logo=espressif)
![Raspberry Pi](https://img.shields.io/badge/Raspberry%20Pi-4-C51A4A?logo=raspberrypi)
![FreeRTOS](https://img.shields.io/badge/RTOS-FreeRTOS-00979D)
![OpenCV](https://img.shields.io/badge/OpenCV-Computer%20Vision-5C3EE8?logo=opencv)
![YOLO](https://img.shields.io/badge/YOLO-Object%20Detection-blue)
![Docker](https://img.shields.io/badge/Docker-Containerized-2496ED?logo=docker)

*A multi-layer autonomous robot that combines embedded systems, robotics, computer vision, and AI for intelligent home monitoring.*

</div>

---

# 📖 Overview

VMO (Virtual Monitoring Operator) is an autonomous mobile robot designed to operate as both a **smart home safety monitor** and an **interactive robotic companion**.

Instead of relying on a single controller, VMO distributes computation across multiple processing layers to ensure deterministic real-time control while simultaneously performing AI inference, computer vision, autonomous navigation, cloud synchronization, and voice interaction.

This architecture demonstrates how heterogeneous embedded systems can cooperate to build scalable autonomous robotic platforms.

---

# ✨ Features

- 🤖 Autonomous robot navigation
- 👤 Human detection & tracking
- 📷 Computer vision with YOLO
- 🚧 Obstacle avoidance
- 🌡️ Temperature & humidity monitoring
- 🔥 Gas leak detection
- 📡 IoT communication
- ☁️ Firebase cloud synchronization
- 🎤 Voice assistant using Speech Recognition + Gemini
- 📱 Android-based animated face & user interface
- ⚡ Multi-controller embedded architecture
- 🧠 ROS 2 autonomous decision making

---

# 🏗 System Architecture

```text
                     Android Smartphone
           Face • Voice • Expressions • UI
                          │
                          ▼
                 Raspberry Pi 4 (Brain)
      ROS 2 • AI • YOLO • Navigation • Cloud
                          │
                 USB Serial Communication
                          │
                          ▼
                 ESP32-S3 (Middleware)
          FreeRTOS • Networking • Scheduling
                          │
                     UART Communication
                          │
                          ▼
              PIC16F877A (Real-Time Layer)
       Sensors • PWM • Motors • Interrupts
```

---

# ⚙️ Technology Stack

| Category | Technologies |
|-----------|--------------|
| **Embedded Systems** | PIC16F877A, ESP32-S3, FreeRTOS |
| **Robotics** | ROS 2 Humble, Differential Drive, Docker |
| **Computer Vision** | OpenCV, YOLO26 Nano, ONNX Runtime |
| **Programming** | C, C++, Python |
| **Networking** | UART, UDP, Protobuf, COBS |
| **Cloud** | Firebase, Google Speech API, Gemini API |
| **Hardware Design** | KiCad, Proteus |

---

# 🔌 Hardware

### Processing Units

- Raspberry Pi 4
- ESP32-S3
- PIC16F877A

### Sensors

- MQ-2 Gas Sensor
- DHT11 Temperature & Humidity
- HC-SR04 Ultrasonic Array
- Optical Wheel Encoders

### Actuation

- Differential Drive Motors
- L298N Motor Driver

### Power

- 3S LiPo Battery
- Battery Management System
- INA226 Power Monitor
- Dual Buck Regulators

---

# 📂 Repository Structure

```text
VMO-Autonomous-Home-Monitoring-Robot
│
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

# 🚀 Engineering Highlights

✅ Multi-layer embedded architecture

✅ ROS 2 autonomous navigation

✅ Edge AI inference using YOLO

✅ FreeRTOS task scheduling

✅ Custom PCB design

✅ Multi-microcontroller communication

✅ Cloud-connected IoT robot

✅ Computer vision based tracking

---

# 💡 Future Improvements

- Visual-Inertial Odometry (VIO)
- Autonomous charging dock
- Mobile application
- SLAM-based navigation
- Smartphone NPU acceleration
- Improved autonomous path planning

---

# 📸 Project Gallery

> *<p align="center">
  <img src="images/Robot.jpg" width="700">
</p>.*

```
Robot Photo

PCB Design

Proteus Simulation

ROS 2 Architecture

Android Interface
```

---

# 📄 License

This repository is shared for educational and portfolio purposes.
