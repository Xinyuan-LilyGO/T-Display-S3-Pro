<div align="center" markdown="1">
  <img src=".github/LilyGo_logo.png" alt="LilyGo logo" width="100"/>
</div>

<h1 align = "center">🌟 T-Display-S3-Pro 🌟</h1>

[![PlatformIO CI](https://github.com/Xinyuan-LillyGO/T-Display-S3-Pro/actions/workflows/pio.yml/badge.svg)](https://github.com/Xinyuan-LilyGO/T-Display-S3-Pro/actions/workflows/pio.yml)

![BackCover](./dimensions/BackCover/BackCover.jpg)

---

## 📖 Table of Contents

- [📋 Product](#-product)
- [🚀 Quick Start](#-quick-start)
  - [PlatformIO (Recommended)](#platformio-quick-start-recommended)
  - [Arduino IDE](#arduino-ide-quick-start)
- [🔧 Hardware Description](#-hardware-description)
  - [📍 Pin Map](#-pins-map)
  - [✨ Display Features](#-display-features)
  - [🧑🏼‍🔧 I2C Devices](#-i2c-devices-address)
  - [⚡ Electrical Parameters](#-electrical-parameters)
  - [💡 LED](#-led)
  - [🔌 QWIIC Connector](#-qwiic-connector)
- [📚 Examples](#-examples)
- [📄 Datasheet](#datasheet)
- [📐 Case & Dimensions](#-case--dimensions)
- [❓ FAQ](#faq)
- [📦 Dependencies](#-dependencies)

---

## 📋 Product

| Product (PinMap)                 | SOC        | Flash | PSRAM    | Resolution |
| -------------------------------- | ---------- | ----- | -------- | ---------- |
| [T-Display-S3-Pro][1]           | ESP32-S3R8 | 16MB  | 8MB(OPI) | 222x480    |
| [T-Display-S3-Pro-MVSRBoard][1] | -          | -     | -        | -          |
| [T-Display-S3-Pro-MVSRLora][1]  | -          | -     | -        | -          |

[1]: https://www.lilygo.cc/products/t-display-s3-pro

---

## 🚀 Quick Start

### PlatformIO Quick Start (Recommended)

1. Install [Visual Studio Code](https://code.visualstudio.com/) and [Python](https://www.python.org/)
2. Search for the `PlatformIO` plugin in Visual Studio Code extension marketplace and install it
3. Restart Visual Studio Code after installation
4. Open `File` → `Open Folder` → select the `T-Display-S3-Pro` directory
5. Wait for third-party library dependencies to install automatically
6. Open `platformio.ini` and uncomment the desired `default_envs = xxxx` line (only one should be active)
7. Click the (✔) icon in the bottom-left corner to compile
8. Connect the board via USB, click (→) to upload firmware
9. Click the (plug symbol) to monitor serial output
10. If upload fails or the USB device keeps flashing, refer to the **FAQ** section below

### Arduino IDE Quick Start

> **Note:** PlatformIO is recommended for a smoother development experience.

1. Install [Arduino IDE](https://www.arduino.cc/en/software)
2. Add the ESP32 board URL in Arduino Preferences → `Additional boards manager URLs`:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
   > **Important:** Use Arduino ESP32 core version **2.0.14** or lower. Versions above 2.0.14 may have compatibility issues with TFT_eSPI. See [TFT_eSPI Issue #3329](https://github.com/Bodmer/TFT_eSPI/issues/3329)

3. Copy all folders from the [lib folder](./lib/) to your Arduino libraries directory (e.g., `C:\Users\YourName\Documents\Arduino\libraries`)
4. Configure the board settings as follows:

   | Setting                           | Value                                |
   | --------------------------------- | ------------------------------------ |
   | Board                             | **ESP32S3 Dev Module**               |
   | Port                              | Your port                            |
   | USB CDC On Boot                   | Enable                               |
   | CPU Frequency                     | 240MHz (WiFi)                        |
   | Flash Mode                        | QIO 80MHz                            |
   | Flash Size                        | **16MB (128Mb)**                     |
   | Partition Scheme                  | **16M Flash (3MB APP/9.9MB FATFS)**  |
   | PSRAM                             | **QSPI PSRAM**                       |
   | Upload Speed                      | 921600                               |

5. Open `T-Display-S3-Pro` → `examples` → select an example → open the `.ino` file
6. Select the correct Port and click Upload

> **Troubleshooting:** If upload fails or the USB device keeps flashing, refer to the **FAQ** section below.

---

## 🔧 Hardware Description

### 📍 Pins Map

| Name                     | GPIO | Note                        |
| ------------------------ | ---- | --------------------------- |
| SDA                      | 5    |                             |
| SCL                      | 6    |                             |
| SPI MISO                 | 8    |                             |
| SPI MOSI                 | 17   |                             |
| SPI SCLK                 | 18   |                             |
| DISPLAY CS               | 39   |                             |
| DISPLAY MISO/MOSI/SCLK   | 8/17/18 | Share SPI Bus           |
| BACKLIGHT PIN            | 48   | 16-level dimming            |
| SD CS                    | 14   |                             |
| SD MISO/MOSI/SCLK        | 8/17/18 | Share SPI Bus           |
| TOUCH RST                | 13   |                             |
| TOUCH SDA/SCL            | 5/6  | Share I2C Bus              |
| BUTTON 1                 | 0    |                             |
| BUTTON 2                 | 12   |                             |
| BUTTON 3                 | 16   |                             |
| On board LED (Green)     | 38   |                             |
| CAMERA RST               | -    | NC                          |
| CAMERA SDA/SCL           | 5/6  | Share I2C Bus              |
| CAMERA VSYNC             | 7    |                             |
| CAMERA POWER DOWN        | 46   |                             |
| CAMERA HREF              | 15   |                             |
| CAMERA XCLK              | 4    |                             |
| CAMERA PCLK              | 2    |                             |
| CAMERA Y2-Y9             | 45,41,40,42,1,3,10,4 |                     |
| CAMERA LED               | 38   | Share Onboard LED           |
| CAMERA Default frequency | 20MHz|                             |

> **Note:** V1.0 uses PWM for backlight adjustment, while V1.1 uses constant current drive. See [AdjustBacklight example](./examples/AdjustBacklight/AdjustBacklight.ino) for details.

### ✨ Display Features

| Feature                | Value           |
| ---------------------- | --------------- |
| Resolution             | 480 × 222       |
| Display Size           | 2.33 Inch       |
| Luminance              | 450 cd/m²       |
| Driver IC              | ST7796U (SPI)   |
| Contrast Ratio         | 1000:1          |
| Color Gamut            | 70%             |
| PPI                    | 221             |
| Display Colors         | 262K            |
| View Direction         | All (IPS)       |
| Operating Temperature  | -20 ~ 70°C      |

### 🧑🏼‍🔧 I2C Devices Address

| Device                           | 7-Bit Address | Shared Bus |
| -------------------------------- | ------------- | ---------- |
| [PowerManage SY6970][2]          | 0x6A          | ✅         |
| [Optical Sensor LTR-553ALS][3]   | 0x23          | ✅         |
| Capacitive Touchscreen CST226SE  | 0x5A          | ✅         |

[2]: https://www.visvie.com/static/upload/file/20220916/1663318546578220.pdf
[3]: https://optoelectronics.liteon.com/upload/download/DS86-2014-0007/LTR-553ALS-01_DS_V1.pdf

### ⚡ Electrical Parameters

| Feature                   | Details                         |
| ------------------------- | ------------------------------- |
| 🔗 USB-C Input Voltage    | 3.9V - 6V                       |
| 🔗 USB-C Output Voltage   | 4.55 - 5.55V                    |
| ⚡ USB-C Output Current   | 0.5 - 1A                        |
| ⚡ Charge Current         | 0 - 5056mA (Programmable)      |
| 🔋 Battery Voltage        | 3840 ~ 4608mV (Programmable)   |
| 🔋 Battery Capacity       | 3800mV / 470mAh                 |
| 🔋 Battery Connector      | [ACHL Connector 1.2mm][4]     |
| 🔋 Charge Temperature     | 0 ~ 60°C                        |

[4]: https://www.jst.com/zh/products/crimp-style-connectors-wire-to-board-type/achl-connector/

> [!IMPORTANT]
> - ⚠️ Recommended charging current: **< 200mA**
> - ⚠️ Charging current should not exceed half of battery capacity
> - The T-Display-S3-Pro uses the SY6970 power path switching chip. To maintain stable power when battery is disconnected, charging must be disabled. See [PMU_Example](https://github.com/Xinyuan-LilyGO/T-Display-S3-Pro/blob/c267d7463609e1f6c62e74455ac5048c9ae93bc3/examples/PMU_Example/PMU_Example.ino#L58) for details.

### 💡 LED

| LED       | GPIO | Note                                                            |
| --------- | ---- | --------------------------------------------------------------- |
| Green LED | 38   | Shared with camera fill light on camera shield models           |
| Red LED   | -    | Charger status indicator                                        |

> **Note:** The red LED flashes when no battery is connected. Disable via `PMU.disableStatLed();` - see [FAQ](#faq).

### 🔌 QWIIC Connector

| Connector | GPIO                       | Description         |
| --------- | -------------------------- | ------------------- |
| LEFT      | 44, 43, 3V3, GND           | UART / I2C / GPIO   |
| RIGHT     | SCL(6), SDA(5), 3V3, GND   | I2C only            |

> **Note:** Screen should be facing you to determine direction.

---

## 📚 Examples

```txt
T-Display-S3-Pro examples
├── examples/Arduino_GFX_HelloWorld      # Arduino_GFX getting started
├── examples/Arduino_GFX_LvglBenchmark   # Arduino_GFX + LVGL benchmark
├── examples/Arduino_GFX_PDQgraphicstest  # Arduino_GFX graphics test
├── examples/CameraShield                 # Camera example
├── examples/CapacitiveTouch              # Capacitive touch example
├── examples/LTR553ALS_Sensor             # Light & proximity sensor
├── examples/TFT_eSPI_Simple             # TFT_eSPI example
├── examples/USB_HID_Example              # USB Host HID & OTG
├── examples/UnitTest                     # Hardware unit testing
├── examples/PMU_Example                  # Power management & battery info
├── examples/AdjustBacklight              # Backlight adjustment
├── examples/Cellphone                    # Camera & photo album (SD required)
└── examples/factory                     # Factory testing
```

> [!CAUTION]
> When using `examples/Cellphone`, do not block the top of the "S3-Pro" screen area. Blocking this area will cause the screen to sleep.

Expansion board examples:

- [T-Display-S3-Pro-MVSRBoard](https://github.com/Xinyuan-LilyGO/T-Display-S3-Pro-MVSRBoard)
- [T-Display-S3-Pro-MVSRLora](https://github.com/Xinyuan-LilyGO/T-Display-S3-Pro-MVSRLora)

### ESP32 Basic Examples

- [BLE Examples](https://github.com/espressif/arduino-esp32/tree/master/libraries/BLE)
- [WiFi Examples](https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFi)
- [SPIFFS Examples](https://github.com/espressif/arduino-esp32/tree/master/libraries/SPIFFS)
- [FFat Examples](https://github.com/espressif/arduino-esp32/tree/master/libraries/FFat)
- More examples available at [arduino-esp32-libraries](https://github.com/espressif/arduino-esp32/tree/master/libraries)

---

## 📄 Datasheet

- [Camera-Shield](./schematic/Camer-Shield.pdf)
- [T-Display-S3-Pro](./schematic/T-Display-Pro.pdf)
- [T-BHI260](./schematic/T-BHI260_V1.1.pdf)
- [T-Display-S3-Pro-MVSRBoard_V1.0_20240913](./schematic/T-Display-S3-Pro-MVSRBoard_V1.0_20240913.pdf)
- [T-Display-S3-Pro-MVSRBoard_V1.0_20241109](./schematic/T-Display-S3-Pro-MVSRBoard_V1.0_20241109.pdf)

## 📐 Case & Dimensions

- 🔓 Cool back cover design by @moononournation - [Design files](./dimensions/BackCover/)
- [T-Display-Pro PCB](./dimensions/T-Display-Pro-PCB.DWG)
- [T-Display-Pro-Vice-Shield](./dimensions/T-Display-Pro-Vice-Shield.dxf)
- [T-Display-Pro-Vice-Shield Gerber](./dimensions/T-Display-Pro-Vice-Shield%20Gerber%2023-11-23.zip)
- [T-BHI260.kicad_mod](./schematic/kicad_sensor.pretty/T-BHI260.kicad_mod)
- [T-CIM29048.kicad_mod](./schematic/kicad_sensor.pretty/T-CIM29048.kicad_mod)

---

## ❓ FAQ

### 1. Upload port not found?

The board uses USB as the JTAG upload port. Ensure `USB_CDC_ON_BOOT` is enabled.

If the port does not appear or USB is used for other functions, enter upload mode manually:

1. Connect the board via USB cable
2. Press and hold the **BOOT** button
3. While holding BOOT, press **RST**
4. Release RST
5. Release BOOT
6. Retry upload

### 2. Manual upload didn't work?

Try burning the [firmware file](./firmware/README.MD) to verify hardware functionality.

### 3. OTG external power not working?

Enable [PMU OTG](https://github.com/Xinyuan-LilyGO/T-Display-S3-Pro/blob/d7f15b379da2b6f711998315401c02a740a8bfa8/examples/CameraShield/CameraShield.ino#L65). When USB input is connected and OTG is set to output, the battery will not charge.

### 4. What does the physical switch do?

Setting the switch to **OFF** completely disconnects the battery from the mainboard. Set to **ON** for charging.

### 5. LED flashing when battery not connected?

Use `PMU.disableStatLed();` to disable the indicator. Note this also disables the charging status indicator. To re-enable, call `PMU.enableStatLed();`

### 6. Device suddenly shuts down?

Sudden shutdown only occurs when battery is not connected. Solution: disable SY6970 charging. See [PMU_Example](https://github.com/Xinyuan-LilyGO/T-Display-S3-Pro/blob/c267d7463609e1f6c62e74455ac5048c9ae93bc3/examples/PMU_Example/PMU_Example.ino#L58).

---

## 📦 Dependencies

| Library             | Repository |
| ------------------- | ---------- |
| XPowersLib          | [GitHub](https://github.com/lewisxhe/XPowersLib) |
| SensorLib           | [GitHub](https://github.com/lewisxhe/SensorLib) |
| lvgl                | [GitHub](https://github.com/lvgl/lvgl) (v8.3.1) |
| TFT_eSPI            | [GitHub](https://github.com/Bodmer/TFT_eSPI) |
| TouchLib            | [GitHub](https://github.com/mmMicky/TouchLib) |
| Arduino_GFX         | [GitHub](https://github.com/moononournation/Arduino_GFX) |
| MPU9250             | [GitHub](https://github.com/hideakitai/MPU9250) |
| MPU6050             | [GitHub](https://github.com/electroniccats/mpu6050) |
| ESP32_USB_Stream    | [GitHub](https://github.com/esp-arduino-libs/ESP32_USB_Stream) (v0.0.1) |
| ESP32-audioI2S      | [GitHub](https://github.com/schreibfaul1/ESP32-audioI2S) (v3.0.8) |
| JPEGDEC             | [GitHub](https://github.com/bitbank2/JPEGDEC) (v1.2.8) |
