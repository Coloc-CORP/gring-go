
# 💍 Smart BLE Ring Project

---

## 🧠 Team

- 🛠️ **Hardware Engineers**: Mathys Pellen, Pierrick Simon
- 👨‍💻 **Software Engineers**: David Cartier, Marius Chritin

---

## 📜 Project Description

Development of an **ultra-compact smart ring** integrating:
- **Bluetooth Low Energy (BLE) communication**
- **Heart rate (BPM) measurement**
- **Activity and movement tracking**
- **Sleep monitoring**
- **Skin temperature measurement**

Mobile application dedicated to:
- Real-time collection of biometric data
- Visualization and analysis of health information
- Synchronization with **Samsung Health** using **Health Connect API**

---

## 🛠️ Project Architecture

### Hardware
- **Microcontroller**: nRF5340 (Nordic Semiconductor)
- **Main Sensors**:
  - MAX86150: Heart Rate and SpO2
  - BHI260AP: 6-axis IMU (motion detection)
  - MAX30205 (or equivalent): Skin temperature sensor
- **Charging**: Wireless charging with Qi standard (STWLC38 IC)
- **PCB**: Flexible PCB (FPC) for compact integration into a ring
- **Casing**: Titanium ring, machined or 3D printed

### Software
- **Embedded firmware**:
  - Developed with **nRF Connect SDK** (Zephyr RTOS)
  - Standard BLE GATT communication
  - OTA DFU (Firmware update over Bluetooth) supported
- **Android Application**:
  - Developed with **Android Studio**
  - BLE Central communication
  - Integration with **Samsung Health** via **Health Connect**

---

## 📂 Project Folder Structure

```plaintext
/README.md           → Main project documentation
/HW/                 → Hardware designs (schematics, PCB layouts, datasheets)
                     → 3D models of the ring (STEP, STL, F3D formats)
/SW/                 → Embedded firmware for nRF5340 (Zephyr)
                     → Android Studio mobile app project
/doc/                → Specifications, technical documents, research notes
```

---

## 🧩 Technologies and Tools

- **nRF Connect SDK** (Zephyr RTOS, Nordic Semiconductor)
- **Segger Embedded Studio** or **VS Code + nRF Connect Extensions**
- **KiCad** for PCB design
- **OnShape / SolidWorks** for 3D modeling
- **Android Studio** for mobile app development
- **GitHub** for project management and version control

---

## 📈 Roadmap

- [x] Requirements and specifications definition
- [x] Component selection
- [ ] BLE basic firmware development
- [ ] BLE mobile application creation
- [ ] First prototype PCB design
- [ ] Hardware prototype fabrication
- [ ] Full system integration and testing
- [ ] Low-power optimization
- [ ] Final miniaturization and industrialization

---

## ⚡ Inspirations

- Samsung Galaxy Ring
- Oura Ring Gen 3

---
