 const int ledPin = 264; // 板载LED引脚
 unsigned long previousMillis = 0; // 记录上次状态变化时间
 const long interval = 500; // 500ms切换一次
 bool ledState = LOW; // 当前LED状态

void setup() {
  pinMode(ledPin, OUTPUT); // 设置引脚为输出模式
}

void loop() {
  unsigned long currentMillis = millis(); // 获取当前时间

  // 非阻塞判断：时间差达到间隔则翻转状态
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis; // 更新时间记录
    ledState = !ledState; // 翻转LED状态
    digitalWrite(ledPin, ledState); // 写入引脚
  }
}
