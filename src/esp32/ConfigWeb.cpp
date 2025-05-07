#if defined(ARDUINO_ARCH_ESP32)

#include <WiFi.h>
#include <WebServer.h>
#include "ConfigWeb.h"
#include "Settings.h"

extern const int locoKeySize;
extern const char *locoKeys[];


const char *htmlPrefix = R"(
<html>
    <head>
      <style>
        body { font-family: Arial, sans-serif; }
        table { width: 100%; }
        th, td { padding: 15px; text-align: left; border: 1px solid #ddd; font-size: 18px; }
        th { background-color: #f2f2f2; }
        input[type="text"], input[type="checkbox"] { 
          width: 100%; 
          font-size: 18px; 
          padding: 10px; 
          box-sizing: border-box;
        }
        input[type="submit"] { 
          font-size: 18px; 
        }
      </style>
    </head>
    <body>
      <h1>RCC Configuration</h1>
      <form method="POST" action="/submit">
        <table>
)";

const char *htmlCheckbox = R"(
          <tr>
            <th>Access point mode:</th>
            <td><input type="checkbox" name="wifiap" $wifiap$></td>
          </tr>
)";

const char *htmlInput = R"(
          <tr>
            <th>$user_readable$:</th>
            <td><input type="text" name="$key$" value="$value$"></td>
          </tr>
)";

const char *htmlSuffix = R"(
          <tr>
            <td colspan="2" style="text-align: center;">
              <input type="submit" value="Submit">
            </td>
          </tr>
        </table>
      </form>
    </body>
  </html>
)";

const char *htmlSubmitted = R"(
  <html>
    <head>
      <style>
        body { font-family: Arial, sans-serif; padding: 20px; margin: 0; }        
      </style>
    </head>
    <body>
      <h1>Changes submitted</h1>
      <p><a href="/">Back to settings</a></p>
    </body>
  </html>
)";

const char *userReadable[] = 
    {"wifiap", "Wifi SSID", "WiFi Password", "Loco Name", "Loco Address", 
     "MQTT Broker IP", "MQTT Broker port", "Acceleration", "managespeed", 
     "heartbeat"};


WebServer ConfigWeb::server(80);

void ConfigWeb::handleRoot()
{
    String wifiap = settings.get("wifiap");

    String form = String(htmlPrefix);
    String checkbox = String(htmlCheckbox);
    checkbox.replace("$wifiap$", (wifiap == "on") ? "checked" : "");
    form += checkbox;
    for (int i = 1; i < locoKeySize; i++) {
        String inputbox = String(htmlInput);
        inputbox.replace("$user_readable$", userReadable[i]);
        inputbox.replace("$key$", locoKeys[i]);
        inputbox.replace("$value$", settings.get(locoKeys[i]));
        form += inputbox;
    }
    form += htmlSuffix;
    server.send(200, "text/html", form);
}

void ConfigWeb::handleSubmit()
{
    String wifiap = server.arg("wifiap");
    if (wifiap != "on")
        wifiap = "of";
    settings.put("wifiap", wifiap);

    for (int i = 1; i < locoKeySize; i++) {
        String value = server.arg(locoKeys[i]);
        settings.put(locoKeys[i], value);
    }
    server.send(200, "text/html", htmlSubmitted);
}

void ConfigWeb::begin()
{
    // if (WiFi.status() != WL_CONNECTED)
    //     return;

    server.on("/", HTTP_GET, handleRoot);
    server.on("/submit", HTTP_POST, handleSubmit);

    server.begin();
    Serial.println("[Web] Server started");
}
#endif