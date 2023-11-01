
When you suspect that there is a problem with the hardware, you can brush in our test firmware

- The difference between V1.0 and V1.1 is that the backlight driver is different. If the wrong firmware is written, the backlight may change.

 | firmware                  | Describe                                                                      |
 | ------------------------- | ----------------------------------------------------------------------------- |
 | Factory_1.x.bin           | Default factory-installed desktop firmware                                    |
 | UnitTest_V1.x_Release.bin | Firmware for testing hardware, outputting debugging information to Qwiic UART |
 | UnitTest_V1.x_Debug.bin   | Firmware for testing hardware, output debugging information to USB CDC        |


1. Download [Flash_download_tool](https://www.espressif.com.cn/sites/default/files/tools/flash_download_tool_3.9.3_0.zip)
2. Select esp32,UART
    ![](esp32s3-1.png)
3. Select according to the following figure, finally select START, press RST after writing, or power on again, and then observe the log output from the serial port
    ![](esp32s3-2.png)
















