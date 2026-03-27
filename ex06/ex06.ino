#define LED_A_PIN 14       
#define LED_B_PIN 27      

const int freq = 5000;    // PWM 频率
const int resolution = 8; // 分辨率

int brightness_A = 1;     // LED A 的亮度 (0-255)
int fadeAmount = 2;       // 亮度变化步长 

void setup() {
  Serial.begin(115200);
  
  ledcAttach(LED_A_PIN, freq, resolution);

  ledcAttach(LED_B_PIN, freq, resolution);
  
  Serial.println("警车双闪灯效启动 ...");
}

void loop() {
  
  int brightness_B = 255 - brightness_A;

  ledcWrite(LED_A_PIN, brightness_A);
  
  ledcWrite(LED_B_PIN, brightness_B);
  
  brightness_A = brightness_A + fadeAmount;

  //  边界检测
  if (brightness_A <= 0 || brightness_A >= 255) {
    fadeAmount = -fadeAmount; // 步长取反，实现往复运动
  }

  //  控制变化速度
  delay(10); 
}
