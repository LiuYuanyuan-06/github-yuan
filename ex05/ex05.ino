#define TOUCH_PIN 27       // 触摸传感器引脚 
#define LED_PIN 2         // 板载LED 引脚 

const int freq = 5000;    // PWM 频率
const int resolution = 8; // 分辨率

#define THRESHOLD 500;     // 触摸阈值 

int brightness = 0;       // 当前亮度 
int fadeAmount = 5;       // 亮度变化步长 

int speedLevel = 1;       // 当前速度档位 (1=慢, 2=中, 3=快)
int delayTime = 30;       // 当前延时时间 

bool lastTouchState = true;    // 上一次触摸状态
unsigned long lastDebounceTime = 0; // 防抖时间戳

void setup() {
  Serial.begin(115200);
 
  ledcAttach(LED_PIN, freq, resolution);
  
  Serial.println("请触摸引脚切换速度档位...");
}

void loop() {

  int touchValue = touchRead(TOUCH_PIN);
  // 判断当前状态
  bool currentTouchState = (touchValue < THRESHOLD);

  // 边缘检测
  if (lastTouchState == false && currentTouchState == true) {
    // 软件防抖
    if (millis() - lastDebounceTime > 300) {
      
     // 切换档位逻辑 
      speedLevel++;
      if (speedLevel > 3) {
        speedLevel = 1;
      }
      
      // 根据档位更新延时时间 
      switch (speedLevel) {
        case 1: delayTime = 30; break; // 慢速
        case 2: delayTime = 10; break; // 中速
        case 3: delayTime = 3;  break; // 快速
      }

      //  串口调试信息
      Serial.print("当前档位: ");
      Serial.println(speedLevel);
     
      lastDebounceTime = millis(); // 重置防抖计时
    }
  }
  // 更新上一次的状态
  lastTouchState = currentTouchState;
  
  //  输出 PWM 信号
  ledcWrite(LED_PIN, brightness);

  // 计算下一次亮度
  brightness = brightness + fadeAmount;

  // 边界检测
  if (brightness <= 0 || brightness >= 255) {
    fadeAmount = -fadeAmount; // 步长取反
  }

  //  使用根据档位计算出延时
  delay(delayTime);
}
