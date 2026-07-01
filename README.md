# Waveshare ESP32-C6-LCD-1.47 Telemetry Monitor

A clean, non-blocking real-time CPU temperature and RAM monitor tailored specifically for the **Waveshare ESP32-C6-LCD-1.47** development board. Built using LVGL and Arduino_GFX, featuring a dynamic UI that changes color based on temperature thresholds.

## Target Hardware
- **Board:** Waveshare ESP32-C6-LCD-1.47
- **Display:** 1.47" IPS LCD (172×320, ST7789V2 driver via SPI)

---
<img height="400" alt="esp32-c6-temp-monitor" src="https://github.com/user-attachments/assets/970481f0-be2e-4484-8282-30d91b7feef1" />

## How to Install and Run

### 1. Set Up Arduino IDE & Espressif Core
1. Open Arduino IDE and go to **File** > **Preferences**.
2. In the "Additional boards manager URLs" field, paste the official Espressif link:
   `https://espressif.github.io/arduino-esp32/package_esp32_index.json`
3. Go to **Tools** > **Board** > **Boards Manager...**, search for `esp32` by **Espressif Systems**, and install version **3.0.0 or higher** (required for ESP32-C6 support).

### 2. Install Libraries
Install these libraries via the Arduino Library Manager:
- **LVGL** (v9.x)
- **GFX Library for Arduino**

### 3. File Setup
Ensure your project folder contains:
- Your main `.ino` sketch (contains the UI logic and hardware pin definitions)
- `lv_conf.h` (LVGL framework configuration)
- `atomic.h` Thread-safe memory management that prevents data corruption between the UI and sensors

### 4. Arduino IDE Settings
Go to the **Tools** menu and configure the following:
- **Board:** `ESP32C6 Dev Module`
- **USB CDC On Boot:** `Enabled` (Required for serial monitoring)

### 5. Upload
Connect the Waveshare board via USB-C, select your **Port**, and click **Upload**. The telemetry dashboard will boot up and start automatically.
