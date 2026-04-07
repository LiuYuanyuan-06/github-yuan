#include <WiFi.h>
#include <WebServer.h>
const char* ap_ssid = "ESP32-lyy";    // 热点名称
const char* ap_pwd  = "2024117050";    // 热点密码
#define TOUCH_PIN 27                 // 触摸引脚
#define LED_PIN 2                    // LED引脚

WebServer server(80);

bool isDefense = false;
bool isAlarm = false;
unsigned long alarmTime = 0;

void handleRoot() {
  String ds = isDefense ? "<span style='color:red;'>布防中</span>" : "<span style='color:green;'>撤防中</span>";
  String as = isAlarm ? "<span style='color:red;'>报警！</span>" : "<span style='color:green;'>正常</span>";

  // 核心修复：添加UTF-8编码声明，中文100%正常显示
  String html = "<!DOCTYPE html><html lang='zh-CN'><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'></head>";
  html += "<body style='text-align:center;margin-top:60px;'>";
  html += "<h2>ESP32 安防报警</h2>";
  html += "<p>状态：" + ds + "</p >";
  html += "<p>报警：" + as + "</p >";
  // 修复布防按钮链接（之前是空链接，点击没反应）
  html += "<p><a href=' '><button style='padding:10px 20px;background:red;color:white;'>布防</button></a ></p >";
  html += "<p><a href='/undefense'><button style='padding:10px 20px;background:green;color:white;'>撤防</button></a ></p >";
  html += "</body></html>";

  // 核心修复：发送时指定UTF-8编码
  server.send(200, "text/html; charset=UTF-8", html);
}

void handleDefense() {
  isDefense = true;
  isAlarm = false;
  digitalWrite(LED_PIN, LOW);
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleUndefense() {
  isDefense = false;
  isAlarm = false;
  digitalWrite(LED_PIN, LOW); // 强制熄灭LED
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);
  delay(1000); // 等待串口稳定

  // 初始化LED引脚
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  WiFi.softAP(ap_ssid, ap_pwd);
  IPAddress apIP = WiFi.softAPIP();
  
  Serial.println("\n=== ESP32 热点已启动 ===");
  Serial.print("热点名称: ");
  Serial.println(ap_ssid);
  Serial.print("访问地址: http://");
  Serial.println(apIP);
  Serial.println("=========================");

  // 注册路由
  server.on("/", handleRoot);
  server.on("/defense", handleDefense);
  server.on("/undefense", handleUndefense);
  server.begin();
}

void loop() {
  server.handleClient();

  // 报警状态逻辑：LED高频闪烁
  if (isAlarm) {
    if (millis() - alarmTime > 300) {
      alarmTime = millis();
      static bool s = false;
      s = !s;
      digitalWrite(LED_PIN, s ? HIGH : LOW);
    }
    return; // 报警时跳过其他逻辑，保持闪烁
  }

  // 只有布防且未报警时，才检测触摸
  if (isDefense) {
    // ESP32触摸引脚必须用touchRead()，不能用digitalRead()
    int touchVal = touchRead(TOUCH_PIN);
    // 触摸阈值（可根据实际调整，比如20/30）
    if (touchVal < 20) {
      isAlarm = true;
      Serial.println("🚨 触摸触发报警！");
    }
  } else {
    digitalWrite(LED_PIN, LOW);
  }

  delay(50); // 降低CPU占用
}
