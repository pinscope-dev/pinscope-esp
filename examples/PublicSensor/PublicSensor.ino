/*
 * A sensor page anyone can open: the ESP32 serves a small page with its
 * latest reading and the last ten minutes, and Pinscope gives it an HTTPS
 * address on the internet.
 *
 * 1. Fill in your WiFi and the project key (dashboard: Connect a device).
 * 2. Replace readSensor() with your own sensor. As it is, it reads the chip's
 *    own temperature, so it runs with nothing connected (on the original
 *    ESP32 that reading is not meaningful; S2, S3, C3 and later have a real one).
 * 3. Flash, and open the device in the dashboard. Make public (paid plans)
 *    gives an address anyone can open without an account; Share gives a link
 *    for up to a week on any plan.
 *
 * Many visitors at once are fine. On a public page or a share link Pinscope
 * keeps the last answer for two seconds and gives it to everyone who asks
 * meanwhile, so the device sees about one request per address every two
 * seconds, however many people are watching. Keep answers small, and do not
 * mark them Cache-Control: no-store, or every visitor goes to the device.
 */
#include <WebServer.h>
#include <WiFi.h>
#include <pinscope.h>

#include "page.h"  // the HTML page, in the next tab

const char *WIFI_SSID = "your-network";
const char *WIFI_PASSWORD = "your-password";
const char *PINSCOPE_KEY = "psk_...";  // Connect a device, in the dashboard

const char *SENSOR_NAME = "Temperature";
const char *SENSOR_UNIT = "°C";
const uint32_t READ_EVERY_MS = 10000;  // the page shows the last 60 readings

WebServer server(80);

// A new firmware sent from the dashboard stays on trial until it reaches
// Pinscope. If it cannot (a wrong WiFi password, a crash at start), the device
// restarts after five minutes into the firmware it had before. Without this
// line the Arduino core accepts every new firmware as soon as it boots.
extern "C" bool verifyRollbackLater() { return true; }

// How often guests opened this device's link (share link or public page), as
// the dashboard counts it: Pinscope sends it with every request, in the
// header X-Pinscope-Visits ("today=12;today_visitors=5;7d=226;30d=562;total=1840").
// -1 when the request did not come through Pinscope.
long visits(const char *key) {
  String header = server.header("X-Pinscope-Visits");
  String name = String(key) + "=";
  int at = header.startsWith(name) ? 0 : header.indexOf(";" + name);
  if (at < 0) return -1;
  return header.substring(at + (at ? 1 : 0) + name.length()).toInt();
}

float readSensor() {
  return temperatureRead();  // your sensor here
}

float history[60];
int count = 0;  // readings kept so far, up to 60
int next = 0;   // where the next one goes

void addReading(float value) {
  history[next] = value;
  next = (next + 1) % 60;
  if (count < 60) count++;
}

void sendData() {
  String json = "{\"name\":\"" + String(SENSOR_NAME) + "\",\"unit\":\"" + String(SENSOR_UNIT) + "\",\"value\":";
  json += count ? String(history[(next + 59) % 60], 2) : "0";
  json += ",\"history\":[";
  for (int i = 0; i < count; i++) {
    if (i) json += ',';
    json += String(history[(next - count + i + 60) % 60], 2);
  }
  json += "]";
  if (visits("total") >= 0) {
    json += ",\"visits\":{\"today\":" + String(visits("today")) + ",\"total\":" + String(visits("total")) + "}";
  }
  json += "}";
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  const char *headers[] = {"X-Pinscope-Visits"};
  server.collectHeaders(headers, 1);  // so server.header() can read it
  server.on("/", [] { server.send_P(200, "text/html", PAGE); });
  server.on("/data.json", sendData);
  server.begin();

  // Starts in the background; it waits for WiFi and reconnects on its own.
  pinscope_config_t config = {};
  config.project_key = PINSCOPE_KEY;
  config.firmware_version = "1.0.0";  // shown in the dashboard; raise it with each build you ship
  pinscope_start(&config);

  addReading(readSensor());
}

void loop() {
  server.handleClient();

  static uint32_t last;
  if (millis() - last >= READ_EVERY_MS) {
    last = millis();
    float value = readSensor();
    addReading(value);
    pinscope_metric("sensor", value);  // also graphed in the dashboard
  }
}
