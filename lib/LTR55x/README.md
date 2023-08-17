# 关于 LTR-55x 库

这是一个支持 LTR-553ALS/556ALS 的 Arduino 库，该模块工作于 I2C 模式下，可以通过简单设置来启用该模块正常工作。
本库参考自: https://github.com/chenhonglin/LTR507-LTR553-LTR556_Arduino，这里特别感谢一下。

# 一个简单示例

```
#include <LTR55x.h>

LTR55x ltr55x;

void setup(){
    Serial.begin(115200);
    Wire.begin();
    ltr55x.begin();
}

void loop(){
    Serial.printf("als_value=%d ps_value=%d\n",ltr55x.get_als_value(),ltr55x.get_ps_value());
    delay(1000);
}
```
