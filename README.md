<h1 align = "center">🌟 T-Display-S3-Pro 🌟</h1> 

[![PlatformIO CI](https://github.com/Xinyuan-LilyGO/T-Display-S3-Pro/actions/workflows/pio.yml/badge.svg)](https://github.com/Xinyuan-LilyGO/T-Display-S3-Pro/actions/workflows/pio.yml)

![](./dimensions/BackCover/BackCover.jpg)


## News

- 
- Cool back cover design from @moononournation , Design files can be found [here](./dimensions/BackCover/)
- The early version of T-Display-S3-Pro V1.0 uses PWM for backlight adjustment. The V1.1 upgrade uses backlight constant current drive. The drive method is different. Please see the [example](./examples/AdjustBacklight/AdjustBacklight.ino) for specific usage.
- How to distinguish versions?
  * If USBC is marked with V1.1, it is the current version. If not, it is V1.0.
- T-Display-S3-Pro uses the SY6970 power path switching chip. In order to maintain stable power supply when the battery is not connected, the charging function must be turned off. Devices connected to the battery do not need to be turned off. How to turn off charge please refer [here](https://github.com/Xinyuan-LilyGO/T-Display-S3-Pro/blob/c267d7463609e1f6c62e74455ac5048c9ae93bc3/examples/PMU_Example/PMU_Example.ino#L58)


## 1️⃣Product

| Product(PinMap)       | SOC        | Flash | PSRAM    | Resolution |
| --------------------- | ---------- | ----- | -------- | ---------- |
| [T-Display-S3-Pro][1] | ESP32-S3R8 | 16MB  | 8MB(OPI) | 222x480    |

[1]: https://www.lilygo.cc/products/t-display-s3-pro

## 2️⃣Examples

```txt
examples
├── examples/Arduino_GFX_HelloWorld           # Arduino_GFX example
├── examples/Arduino_GFX_LvglBenchmark        # Arduino_GFX example
├── examples/Arduino_GFX_PDQgraphicstest      # Arduino_GFX example
├── examples/CameraShield                     # Camera example
├── examples/CapacitiveTouch                  # Capacitive Touch example
├── examples/LTR553ALS_Sensor                 # Light & Proximity Sensor example
├── examples/TFT_eSPI_Simple                  # TFT_eSPI example
├── examples/USB_HID_Example                  # USB Host HID & OTG example
├── examples/UnitTest                         # Hardware unit testing example
├── examples/PMU_Example                      # Power management settings and battery information detection example
├── examples/AdjustBacklight                  # Backlight adjustment example
├── examples/Cellphone                        # With camera that can take pictures, to view the photos in the album (sd card required)
└── examples/factory                          # Factory example

If use the 'examples/Cellphone' examples,
be careful not block the top of the "S3-Pro" screen, if it is blocked, the screen will sleep.
```

## 3️⃣ PlatformIO Quick Start (Recommended)

1. Install [Visual Studio Code](https://code.visualstudio.com/) and [Python](https://www.python.org/)
2. Search for the `PlatformIO` plugin in the `VisualStudioCode` extension and install it.
3. After the installation is complete, you need to restart `VisualStudioCode`
4. After restarting `VisualStudioCode`, select `File` in the upper left corner of `VisualStudioCode` -> `Open Folder` -> select the `T-Display-S3-Pro` directory
5. Wait for the installation of third-party dependent libraries to complete
6. Click on the `platformio.ini` file, and in the `platformio` column
7. Uncomment one of the lines `default_envs = xxxx` to make sure only one line works
8. Click the (✔) symbol in the lower left corner to compile
9. Connect the board to the computer USB
10. Click (→) to upload firmware
11. Click (plug symbol) to monitor serial output
12. If it cannot be written, or the USB device keeps flashing, please check the **FAQ** below

## 4️⃣ Arduino IDE Quick Start

* It is recommended to use platformio without cumbersome steps
* [T-Display-S3-Pro Arduino IDE Record](https://youtu.be/z4w9Qj5ArAw)

1. Install [Arduino IDE](https://www.arduino.cc/en/software)
2. Install [Arduino ESP32 V 2.0.5 or above and below V3.0](https://docs.espressif.com/projects/arduino-esp32/en/latest/)
3. Download `T-Display-S3-Pro` to Decktop
4. Copy all folders in [lib folder](./lib/)  to Arduino library folder (e.g. C:\Users\YourName\Documents\Arduino\libraries)
5. Open ArduinoIDE  ,`Tools` , Look at the picture to choose
  ![setting](images/ArduinoIDE.jpg)
1. Open `T-Display-S3-Pro` -> `examples` -> `any examples` -> `any eaxmples.ino`
2. Select `Port`
3. Click `upload` , Wait for compilation and writing to complete
4. If it cannot be written, or the USB device keeps flashing, please check the **FAQ** below


# 5️⃣ ESP32 basic examples

* [BLE Examples](https://github.com/espressif/arduino-esp32/tree/master/libraries/BLE)
* [WiFi Examples](https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFi)
* [SPIFFS Examples](https://github.com/espressif/arduino-esp32/tree/master/libraries/SPIFFS)
* [FFat Examples](https://github.com/espressif/arduino-esp32/tree/master/libraries/FFat)
* For more examples of esp32 chip functions, please refer to [arduino-esp32-libraries](https://github.com/espressif/arduino-esp32/tree/master/libraries)

# 6️⃣ FAQ

1. The board uses USB as the JTAG upload port. When printing serial port information on USB_CDC_ON_BOOT configuration needs to be turned on.
If the port cannot be found when uploading the program or the USB has been used for other functions, the port does not appear.
Please enter the upload mode manually.
   1. Connect the board via the USB cable
   2. Press and hold the BOOT button , While still pressing the BOOT button, press RST
   3. Release the RST
   4. Release the BOOT button
   5. Upload sketch

2. If the above is invalid, burn the [binary file](./firmware/README.MD)  to check whether the hardware is normal
3. The OTG external power supply function requires turning on the  [PMU OTG enablement](https://github.com/Xinyuan-LilyGO/T-Display-S3-Pro/blob/d7f15b379da2b6f711998315401c02a740a8bfa8/examples/CameraShield/CameraShield.ino#L65). If the USB input is connected and the OTG is set to output, the battery will not be charged.
4. Turning the physical switch to OFF will completely disconnect the battery from the motherboard. When charging is required, turn the switch to ON.
5. When the battery is not connected and the USB is plugged in, the board's LED status indicator light will flash. You can use `PMU.disableStatLed();` to turn off the indicator light, but this means that if the battery is connected for charging, the LED light will also be disabled. If you need to enable the charging status indicator, please call `PMU.enableStatLed();`
6. Device shuts down suddenly?

  - When the battery is connected, the device will not shut down suddenly. Sudden shutdown will only occur when the device is not connected to the battery. The solution is to turn off the charging function of SY6970. How to turn off charge please refer [here](https://github.com/Xinyuan-LilyGO/T-Display-S3-Pro/blob/c267d7463609e1f6c62e74455ac5048c9ae93bc3/examples/PMU_Example/PMU_Example.ino#L58)

# 7️⃣ Depends on required libraries

* [XPowersLib](https://github.com/lewisxhe/XPowersLib)
* [SensorLib](https://github.com/lewisxhe/SensorLib)
* [lvgl 8.3.1](https://github.com/lvgl/lvgl)
* [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI)
* [TouchLib](https://github.com/mmMicky/TouchLib)
* [Arduino_GFX](https://github.com/moononournation/Arduino_GFX)
* [MPU9250](https://github.com/hideakitai/MPU9250)
* [MPU6050](https://github.com/electroniccats/mpu6050)



