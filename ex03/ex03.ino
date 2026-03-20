
const int LED_PIN_2 = 14;
const unsigned int UNIT = 200; // 时间单位
const int pattern[] = {1, 1, 1,  3, 3, 3,  1, 1, 1}; 
const int patternLen = sizeof(pattern) / sizeof(pattern[0]);

int step = -1;              // 当前步骤
unsigned long lastTime = 0;
bool isLit = false;         // 当前 LED 状态

void setup() {
  pinMode(LED_PIN_2, OUTPUT);
  digitalWrite(LED_PIN_2, LOW);
}

void loop() {
  unsigned long now = millis();
  
  // 计算当前步骤应持续的时间
  int duration = 0;
  
  if (step == -1) {
    // 长停顿阶段 
    duration = UNIT * 7;
  } else {
    if (isLit) {
      // 亮灯阶段
      duration = UNIT * pattern[step];
    } else {
      // 灭灯阶段
      duration = UNIT * 1;
    }
  }

  // 时间到，切换状态
  if (now - lastTime >= duration) {
    lastTime = now;
    
    if (step == -1) {
      // 长停顿结束，开始新序列
      step = 0;
      isLit = true;
      digitalWrite(LED_PIN_2, HIGH);
    } else {
      if (isLit) {
        // 刚亮完，现在熄灭
        isLit = false;
        digitalWrite(LED_PIN_2, LOW);
      } else {
        // 刚灭完，准备下一步
        step++;
        if (step >= patternLen) {
          // 序列结束，进入长停顿
          step = -1;
          isLit = false; // 保持灭
          digitalWrite(LED_PIN_2, LOW);
        } else {
          // 开始下一个亮灯
          isLit = true;
          digitalWrite(LED_PIN_2, HIGH);
        }
      }
    }
  }
}
