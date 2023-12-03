#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

const int ledPin = D6;
DHT dht11(D4, DHT11);
ESP8266WebServer server(80);

void initWiFi(String ssid, String password) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void handleRoot() {
  float temperature = dht11.readTemperature();
  float humidity = dht11.readHumidity();

  String html = F("<html><body><div style='text-align: center;'>");
  html += F("<p><h2 style='color: #7D7DFF;'>Web Server</h2>");
  html += F("<p>Humidity : <span id='humidity'>") + String(humidity) + F(" %</span></p>");
  html += F("<p>Temperature : <span id='temperature'>") + String(temperature) + F(" C</span></p>");
  html += F("<p>LED Status: ");
  html += digitalRead(ledPin) == HIGH ? F("<span style='color: #00FF00;'>On</span>") : F("<span style='color: #FF0000;'>Off</span>");
  html += F("</p>");
  html += F("<form action='/TurnOn' method='get' style='display: inline; margin-right:25px;'><input type='submit' value='Turn On LED' style='background-color:##7ED957;'></form>&nbsp;");
  html += F("<form action='/TurnOff' method='get' style='display: inline;'><input type='submit' value='Turn Off LED' style='background-color:##D60000;'></form>");
  html += F("</div></body></html>");

  html += F("<script>");
  html += F("function updateData() {");
  html += F("  var xhr = new XMLHttpRequest();");
  html += F("  xhr.open('GET', '/data', true);");
  html += F("  xhr.onreadystatechange = function() {");
  html += F("    if (xhr.readyState == 4 && xhr.status == 200) {");
  html += F("      var data = JSON.parse(xhr.responseText);");
  html += F("      document.getElementById('temperature').textContent = data.temperature +'°C ';");
  html += F("      document.getElementById('humidity').textContent = data.humidity + '%' %';");
  html += F("    }");
  html += F("  };");
  html += F("  xhr.send();");
  html += F("}");
  html += F("setInterval(updateData,1000);");
  html += F("updateData();");
  html += F("</script>");

  server.send(200, "text/html", html);
}


void handleTurnOn() {
  digitalWrite(ledPin, HIGH);
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void handleTurnOff() {
  digitalWrite(ledPin, LOW);
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void handleData() {
  float temperature = dht11.readTemperature();
  float humidity = dht11.readHumidity();

  String data = "{\"temperature\": " + String(temperature) + ", \"humidity\": " + String(humidity) + "}";
  server.send(200, "application/json", data);
}

void setup() {
  Serial.begin(115200);
  initWiFi("Xiaomi 11T Pro", "1212312126");

  pinMode(ledPin, OUTPUT);
  dht11.begin();

  server.on("/", HTTP_GET, handleRoot);
  server.on("/TurnOn", HTTP_GET, handleTurnOn);
  server.on("/TurnOff", HTTP_GET, handleTurnOff);
  server.on("/data", HTTP_GET, handleData);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
