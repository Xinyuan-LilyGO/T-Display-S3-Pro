
# How to flash firmware


When you suspect that there is a problem with the hardware, you can brush in our test firmware

- The difference between V1.0 and V1.1 is that the backlight driver is different. If the wrong firmware is written, the backlight may change.

 | firmware                          | Describe                                                                                              |
 | --------------------------------- | ----------------------------------------------------------------------------------------------------- |
 | Factory_1.x.bin                   | Default factory-installed desktop firmware                                                            |
 | UnitTest_V1.x_Release.bin         | Firmware for testing hardware, outputting debugging information to Qwiic UART                         |
 | UnitTest_V1.x_Debug.bin           | Firmware for testing hardware, output debugging information to USB CDC                                |
 | UnitTest_V1.x_DiscChg_Release.bin | Firmware for testing hardware,Disable charge function, outputting debugging information to Qwiic UART |
 | UnitTest_V1.x_DiscChg_Debug.bin   | Firmware for testing hardware,Disable charge function, output debugging information to USB CDC        |




## ESP-Launchpad Online

- [Esp-launchpad](https://espressif.github.io/esp-launchpad/)

![ESP-Launchpad](../docs/images/web_flasher2.gif)

* Note that after writing is completed, you need to press RST to reset.

### Use ESP Download Tool

- Download [Flash_download_tool](https://dl.espressif.com/public/flash_download_tool.zip)

![web_flasher](../docs/images/esp_downloader.gif)

* Note that after writing is completed, you need to press RST to reset.

### Use Web Flasher

- [ESP Web Flasher Online](https://espressif.github.io/esptool-js/)

![web_flasher](../docs/images/web_flasher.gif)

* Note that after writing is completed, you need to press RST to reset.

### Use command line


If system asks about install Developer Tools, do it.

```bash
python3 -m pip install --upgrade pip
python3 -m pip install esptool
```

In order to launch esptool.py, exec directly with this:

```bash
python3 -m esptool
```

For ESP32-S3 use the following command to write

```bash
esptool --chip esp32s3  --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m 0x0 firmware.bin

```














