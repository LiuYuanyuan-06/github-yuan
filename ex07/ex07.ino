#include <WiFi.h>
#include <WebServer.h>

const char* ap_ssid     = "ESP32-lyy";
const char* ap_password = "2024117050";
const int ledPin     = 2;
const int freq       = 5000;
const int resolution = 8;

WebServer server(80);

String makePage() {
  String html = R"HTML(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>PWM 亮度调节</title>
  <style>
    body{font-family:Arial; text-align:center; margin-top:50px;}
    input[type="range"]{width:300px; margin:20px 0;}
  </style>
</head>
<body>
  <h1>LED 亮度调节</h1>
  <input type="range" min="0" max="255" value="0" id="slider">
  <p>当前亮度：<span id="val">0</span></p >

<script>
const slider = document.getElementById('slider');
const val    = document.getElementById('val');

slider.addEventListener('input', function(){
  let v = this.value;
  val.textContent = v;
  fetch('/set?bright=' + v);
});
</script>
</body>
</html>
)HTML";
  return html;
}

void handleRoot() {
  server.send(200, "text/html; charset=UTF-8", makePage());
}

void handleSet() {
  if (server.hasArg("bright")) {
    int bright = server.arg("bright").toInt();
    bright = constrain(bright, 0, 255);
    ledcWrite(ledPin, bright);
  }
  server.send(204);
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== ESP32 AP模式启动 ===");

  // PWM初始化
  ledcAttach(ledPin, freq, resolution);
  ledcWrite(ledPin, 0);
  Serial.println(" PWM 初始化完成");

  // 开启ESP32热点
  WiFi.softAP(ap_ssid, ap_password);
  IPAddress apIP = WiFi.softAPIP();
  Serial.println(" 热点已开启");
  Serial.print(" 热点名称: ");
  Serial.println(ap_ssid);
  Serial.print(" 访问地址: /");
  Serial.println(apIP);

  // 注册路由
  server.on("/", handleRoot);
  server.on("/set", handleSet);
  server.begin();
  Serial.println("服务器已启动");
}

void loop() {
  server.handleClient();
}
