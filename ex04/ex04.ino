#define TOUCH_PIN 27      // 触摸引脚 
#define LED_PIN 2         // LED引脚 
#define THRESHOLD 500     // 阈值

bool ledState = true;          // 记录灯是亮还是灭
unsigned long lastTime = 0;     // 记录上次触发的时间
const long debounceDelay = 400; // 400毫秒内忽略重复触发 

//  中断服务函数 
void gotTouch() {
  // 获取当前时间
  unsigned long currentTime = millis();

  if (currentTime - lastTime > debounceDelay) {
    
    //  翻转状态 
    ledState = !ledState;
    
    digitalWrite(LED_PIN, ledState);
    
    lastTime = currentTime;
    
    Serial.print("触发中断 LED: ");
    
    Serial.println(ledState ? "亮" : "灭");
  }
}


void setup() {
  Serial.begin(115200);
  delay(1000); // 等待串口稳定
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // 初始关闭

  // 绑定中断函数
  touchAttachInterrupt(TOUCH_PIN, gotTouch, THRESHOLD);
  
  Serial.println("请触摸...");
}

void loop() {
  delay(10); 
}
