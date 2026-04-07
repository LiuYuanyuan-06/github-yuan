#include <WiFi.h>
#include <WebServer.h>

const char* ap_ssid = "ESP32-lyy";
const char* ap_pass = "2024117050";

#define LED_PIN 26
#define TOUCH_PIN 27 // 确保使用的是GPIO 27，而不是GPIO 4或其他

WebServer server(80);

bool isArmed = false;         // 系统是否已布防
bool isAlarmTriggered = false;// 是否已触发报警
int touchThreshold = 50;      // 触摸阈值 (默认值，建议根据串口输出调整)
unsigned long lastBlinkTime = 0; // 用于非阻塞延时
bool ledState = false;        // LED当前状态

String makePage() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>安防报警主机</title>
  <style>
    body { font-family: sans-serif; text-align: center; margin-top: 50px; background-color: #f0f0f0; }
    h1 { color: #333; }
    .status { font-size: 1.5em; margin: 20px; font-weight: bold; min-height: 1.5em; padding: 10px; border-radius: 5px; }
    .btn { padding: 15px 30px; font-size: 1.2em; margin: 10px; border: none; border-radius: 5px; cursor: pointer; color: white; }
    .arm { background-color: #e74c3c; } 
    .disarm { background-color: #2ecc71; } 
    .alarm-active { color: red; animation: blink 0.5s infinite; background-color: #ffcccc; }
    @keyframes blink { 0% { opacity: 1; } 50% { opacity: 0.5; } 100% { opacity: 1; } }
  </style>
</head>
<body>
  <h1>🛡️ 智能安防主机</h1>
  <div id="status" class="status">系统就绪</div>
  
  <button class="btn arm" onclick="sendCommand('arm')">🔴 布防 (ARM)</button>
  <button class="btn disarm" onclick="sendCommand('disarm')">🟢 撤防 (DISARM)</button>

  <script>
    function sendCommand(cmd) {
      fetch('/action?cmd=' + cmd)
        .then(response => response.text())
        .then(text => {
          const statusDiv = document.getElementById('status');
          statusDiv.innerHTML = text; 
          
          // 样式更新逻辑
          if(text.includes("报警")) {
             statusDiv.className = "status alarm-active";
          } else if (text.includes("布防")) {
             statusDiv.className = "status";
             statusDiv.style.backgroundColor = "#fff3cd"; // 黄色背景
             statusDiv.style.color = "#856404";
          } else {
             statusDiv.className = "status";
             statusDiv.style.backgroundColor = "#d4edda"; // 绿色背景
             statusDiv.style.color = "#155724";
          }
        });
    }
    // 页面加载时获取一次状态
    window.onload = function() { sendCommand('status'); };
    // 每2秒轮询一次状态，确保网页显示与硬件同步
    setInterval(() => sendCommand('status'), 2000);
  </script>
</body>
</html>
)rawliteral";
  return html;
}


void handleRoot() {
  server.send(200, "text/html", makePage());
}

void handleAction() {
  if (server.hasArg("cmd")) {
    String cmd = server.arg("cmd");
    String msg = "";

    if (cmd == "arm") {
      isArmed = true;
      isAlarmTriggered = false; // 布防时重置报警
      msg = "系统状态: 已布防 (警戒中)";
      Serial.println(">>> 系统已布防");
    } 
    else if (cmd == "disarm") {
      isArmed = false;
      isAlarmTriggered = false;
      msg = "系统状态: 已撤防 (安全)";
      Serial.println(">>> 系统已撤防");
      digitalWrite(LED_PIN, LOW); // 强制灭灯
    }
    else if (cmd == "status") {
       // 仅查询状态
    }
    
    // 根据当前状态返回文本
    if (isAlarmTriggered) {
      msg = "⚠️ 系统状态: 报警触发!";
    } else if (isArmed) {
      msg = "系统状态: 已布防 (警戒中)";
    } else {
      msg = "系统状态: 已撤防 (安全)";
    }

    server.send(200, "text/plain", msg);
  } else {
    server.send(200, "text/plain", "Error");
  }
}


void setup() {
  Serial.begin(115200);
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // --- 触摸传感器自动校准 ---
  Serial.println("正在初始化触摸传感器，请确保此时没有触碰引脚...");
  delay(1000);
  int baseline = touchRead(TOUCH_PIN);
  // 阈值设置为基准值减去20，这样只要触碰导致数值下降就会触发
  touchThreshold = baseline - 20; 
  Serial.print("校准完成。基准值: ");
  Serial.print(baseline);
  Serial.print("，触发阈值设定为: ");
  Serial.println(touchThreshold);

  WiFi.softAP(ap_ssid, ap_pass);
  Serial.print("AP IP Address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/action", handleAction);
  
  server.begin();
  Serial.println("安防服务器已启动");
}

void loop() {
  server.handleClient();

  if (isAlarmTriggered) {
    unsigned long currentMillis = millis();
    if (currentMillis - lastBlinkTime >= 200) { // 每200ms切换一次，即0.2秒
      lastBlinkTime = currentMillis;
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState ? HIGH : LOW);
    }
  } 
  else if (isArmed) {
    int touchValue = touchRead(TOUCH_PIN);
    
    // 打开串口监视器查看数值，如果一直不触发，请降低 touchThreshold
    Serial.print("触摸数值: "); 
    Serial.print(touchValue);
    Serial.print(" (阈值: ");
    Serial.print(touchThreshold);
    Serial.println(")");

    if (touchValue < touchThreshold) { 
      Serial.println(">>> 检测到入侵！触发报警！");
      isAlarmTriggered = true;
      // 立即点亮LED，给用户即时反馈
      digitalWrite(LED_PIN, HIGH); 
    }
  } 

 else {
    digitalWrite(LED_PIN, LOW);
  }
  
  // 极短的延时，防止串口打印过快导致卡顿，但不影响Web服务器
  delay(10); 
}
