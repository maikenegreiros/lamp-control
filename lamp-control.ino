#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

const char* ssid = "MaikeESP";
const char* password = "12345678";
String html, script, style, response, lampState;
IPAddress myIP;

ESP8266WebServer server(80);

void handleRoot() {
  html = "";
  html += "<!DOCTYPE html>";
  html += "<html lang=\"pt-BR\">";
  html += " <head>";
  html += "  <meta charset=\"utf-8\" />";
  html += "  <title>ESP8266</title>";
  html += "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  html += "  <script src=\"/javascript\"></script>";
  html += "  <link rel=\"stylesheet\" href=\"/css\">";
  html += " </head>";
  html += " <body>";
  html += "   <div class=\"container\">";
  html += "     <svg xmlns=\"http://www.w3.org/2000/svg\" id=\"bulb\" viewBox=\"0 0 69.5 83.7\"><style>.st1{display:inline}</style><path d=\"M57.4 8.1C48.5.7 39.7 0 34.8 0S21 .7 12.1 8.1C3.2 15.5-1.1 26.4.3 38c1.1 8.9 5.5 14.6 10.2 20.7 1.6 2.1 3.3 4.2 4.8 6.5 5.6 8.3 5 17 5 17.1 0 .4.1.8.4 1 .3.3.6.4 1 .4h26.4c.4 0 .7-.2 1-.4.3-.3.4-.6.4-1 0-.1-.5-8.7 5-17.1 1.5-2.3 3.2-4.4 4.8-6.5C64 52.6 68.4 46.9 69.5 38c1.1-11.6-3.2-22.5-12.1-29.9z\"/></svg>";
  html += "     <svg xmlns=\"http://www.w3.org/2000/svg\" id=\"base\" viewBox=\"0 0 28.7 13.1\"><g><g><path d=\"M27.2 3H1.5C.7 3 0 2.3 0 1.5S.7 0 1.5 0h25.7c.8 0 1.5.7 1.5 1.5S28 3 27.2 3z\"/></g><g><path d=\"M27.2 8H1.5C.7 8 0 7.3 0 6.5S.7 5 1.5 5h25.7c.8 0 1.5.7 1.5 1.5S28 8 27.2 8z\"/></g><g><path d=\"M22.5 13.1H6.2c-.8 0-1.5-.7-1.5-1.5s.7-1.5 1.5-1.5h16.2c.8 0 1.5.7 1.5 1.5s-.6 1.5-1.4 1.5z\"/></g></g></svg>";
  html += "   </div>";
  html += " </body>";
  html += "</html>";


  server.send(200, "text/html", html);
}

void handleLedOn() {
  digitalWrite(D1, HIGH);
  if (digitalRead(D1) != HIGH) {
    response = "{\"message\": \"Ocorreu um erro e o LED não foi acendido\"}";
    server.send(500, "application/json", response);
    return;
  }
  lampState = "1";
  response = "{\"message\": \"LED Ligado\"}";
  server.send(200, "application/json", response);
}

void handleLedOff() {
  digitalWrite(D1, LOW);
  if (digitalRead(D1) != LOW) {
    response = "{\"message\": \"Ocorreu um erro e o LED não foi apagado\"}";
    server.send(500, "application/json", response);
    return;
  }
  lampState = "''";
  response = "{\"message\": \"LED Desligado\"}";
  server.send(200, "application/json", response);
}

void loadJavaScript() {
  script = "";
  script += "window.addEventListener('DOMContentLoaded',() => {";
  script += " const container = document.querySelector('.container');";
  script += " const bulb = document.querySelector('#bulb');";
  script += " let on = !!" + lampState + ";";
  script += " container.classList.toggle('led_on', on);";
  script += " bulb.addEventListener('click', () => {";
  script += "   on = !on;";
  script += "   state = on ? 'led_on' : 'led_off';";
  script += "   container.classList.toggle('led_on', on);";
  script += "   fetch(`http://" + myIP.toString() + "/${state}`);";
  script += " });";
  script += "});";

  server.send(200, "application/javascript", script);
}

void loadStyle() {
  style = "";
  style += "@charset \"UTF-8\";";
  style += "html,";
  style += "body {";
  style += "  margin: 0;";
  style += "  height: 100%;";
  style += "}";
  style += "body {";
  style += "  display: flex;";
  style += "  justify-content: center;";
  style += "  align-items: center;";
  style += "  flex-direction: column;";
  style += "}";
  style += ".container {";
  style += "  width: 100%;";
  style += "  height: 100%;";
  style += "  border-radius: 2px;";
  style += "  box-shadow: 4px 8px 16px 0 rgba(0, 0, 0, 0.1);";
  style += "  overflow: hidden;";
  style += "  background: #44424A;";
  style += "  display: flex;";
  style += "  justify-content: center;";
  style += "  align-items: center;";
  style += "  flex-direction: column;";
  style += "}";
  style += ".container.led_on {";
  style += "  background: #bdbdbd;";
  style += "}";
  style += "#bulb {";
  style += "  width: 85px;";
  style += "  fill: #7E7E7E;";
  style += "  filter: drop-shadow(0 0 20px transparent);";
  style += "  transition: all .5s;";
  style += "  cursor: pointer;";
  style += "}";
  style += ".led_on #bulb {";
  style += "  fill: #fff35c;";
  style += "  filter: drop-shadow(0 0 20px #fff35c);";
  style += "}";
  style += "#.led_on bulb ~ #base {";
  style += "  fill: #fff35c;";
  style += "}";
  style += "#base {";
  style += "  padding-top: 3px;";
  style += "  width: 35px;";
  style += "  fill: #fff;";
  style += "  transition: all .5s;";
  style += "}";

  server.send(200, "text/css", style);
}

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  pinMode(D1, OUTPUT);
  digitalWrite(D1, LOW);
  lampState = digitalRead(D1) == HIGH ? "1" : "''";
  
  myIP = WiFi.softAPIP();
  Serial.println("IP: " + myIP.toString());

  server.on("/", handleRoot);
  server.on("/led_on", handleLedOn);
  server.on("/led_off", handleLedOff);
  server.on("/javascript", loadJavaScript);
  server.on("/css", loadStyle);

  server.begin();
  Serial.println("Servidor HTTP iniciado");
}

void loop() {
  server.handleClient();
}
