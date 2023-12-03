#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

ESP8266WebServer server(80);

const int ledPin = D6;
DHT dht14(D4, DHT11);

void init_wifi(String ssid, String password) {
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

void setup(void) {
  Serial.begin(115200);
  init_wifi("PIYAKORN 6827", "62$K4x05");

  pinMode(ledPin, OUTPUT);

  dht14.begin();

  server.on("/", HTTP_GET, []() {
    float temperature = dht14.readTemperature();
    float humidity = dht14.readHumidity();

    String html = "<html><head>";
    html += "<link href='https://fonts.googleapis.com/css2?family=Roboto:wght@300;400;500&display=swap' rel='stylesheet'>";
    html += "<style>";
    html += "body { font-family: 'Roboto', sans-serif; background-color: #f2f2f2; margin: 0; padding: 0; }";
    html += "div.content { max-width: 800px; background-color: #fff; border-radius: 5px; padding: 20px; margin: 40px auto; text-align: center; box-shadow: 0 4px 8px rgba(0,0,0,0.1); }";
    html += "h1 { color: #333; font-size: 36px; text-decoration: underline; }";
    html += "p { color: #666; font-size: 24px; font-weight: 400; margin: 10px 0; }";
    html += "input[type='submit'] { background-color: #86DB8A; color: white; padding: 10px 20px; border: none; border-radius: 5px; cursor: pointer; font-size: 16px; transition: background-color 0.3s ease; }";
    html += "input[type='submit']:hover { background-color: #86DB8A; }";
    html += "#turnOffButton { background-color: #FF9F9F; }";
    html += "#turnOffButton:hover { background-color: #FF9F9F; }";
    html += "</style>";
    html += "</head>";
    html += "<body>";
    html += "<div class='content'>";
    html += "<h1>IoT Server</h1>";
    html += "<p>Temperature: <span id='temperature'>" + String(temperature, 2) + "C</span></p>";
    html += "<p>Humidity: <span id='humidity'>" + String(humidity, 2) + "%</span></p>";
    html += "<p>LED Status: ";
    if (digitalRead(ledPin) == HIGH) {
      html += "<span style='color: #8BE88F;'>On</span>";
    } else {
      html += "<span style='color: #E88B8B;'>Off</span>";
    }
    html += "</p>";
    html += "<form action='/TurnOn' method='get' style='display: inline; margin-right: 10px;'><input type='submit' value='Turn led On'></form>";
    html += "<form action='/TurnOff' method='get' style='display: inline;'><input type='submit' id='turnOffButton' value='Turn led Off'></form>";

    html += "</div>";  // Close content div
    html += "<script>";
    // ... [Your JavaScript remains unchanged]
    html += "</script>";
    html += "</body></html>";


    html += "<script>";
    html += "function updateData() {";
    html += "  var xhr = new XMLHttpRequest();";
    html += "  xhr.open('GET', '/data', true);";
    html += "  xhr.onreadystatechange = function() {";
    html += "    if (xhr.readyState == 4 && xhr.status == 200) {";
    html += "      var data = JSON.parse(xhr.responseText);";
    html += "      document.getElementById('temperature').textContent = data.temperature.toFixed(2) + ' C';";
    html += "      document.getElementById('humidity').textContent = data.humidity.toFixed(2) + ' %';";
    html += "    }";
    html += "  };";
    html += "  xhr.send();";
    html += "}";
    html += "setInterval(updateData, 500);";
    html += "</script>";

    server.send(200, "text/html", html);
  });


  server.on("/TurnOn", HTTP_GET, []() {
    digitalWrite(ledPin, HIGH);
    server.sendHeader("Location", "/");
    server.send(302, "text/plain", "");
  });

  server.on("/TurnOff", HTTP_GET, []() {
    digitalWrite(ledPin, LOW);
    server.sendHeader("Location", "/");
    server.send(302, "text/plain", "");
  });

  server.on("/data", HTTP_GET, []() {
    float temperature = dht14.readTemperature();
    float humidity = dht14.readHumidity();

    String data = "{\"temperature\": " + String(temperature) + ", \"humidity\": " + String(humidity) + "}";
    server.send(200, "application/json", data);
  });

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}