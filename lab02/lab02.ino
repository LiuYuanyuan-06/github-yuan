// 定义板载LED引脚，分别定义三个
#define LED_PIN 2
#define LED_PIN_R 26
#define LED_PIN_1 27
#define LED_PIN_2 14

void setup() {
  // 初始化串口通信
  Serial.begin(115200);
  //板载LED引脚为输出模式
  pinMode(LED_PIN, OUTPUT); 
    pinMode(LED_PIN_R, OUTPUT); 
     
}

void loop() {
  Serial.println("Hello ESP32!");
  digitalWrite(LED_PIN, HIGH); // 点亮LED
   digitalWrite(LED_PIN_R, HIGH);
    
  Serial.println("LED ON");    // 串口输出提示
  delay(1000); // 等待1秒
  
  digitalWrite(LED_PIN, LOW);    // 熄灭LED
   digitalWrite(LED_PIN_R, LOW);
     
  Serial.println("LED OFF");   // 串口输出提示

  delay(1000);              // 等待1秒

}
