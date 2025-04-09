#if defined(ARDUINO_ARCH_ESP32)

#include <WiFi.h>
#include <WebServer.h>
#include "ConfigWeb.h"
#include "Settings.h"

const char *htmlRoot = R"(
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
          <tr>
            <th>Access point mode:</th>
            <td><input type="checkbox" name="wifiap" $wifiap$></td>
          </tr>
          <tr>
            <th>Wifi SSID:</th>
            <td><input type="text" name="wifissid" value="$wifissid$"></td>
          </tr>
          <tr>
            <th>Wifi Password:</th>
            <td><input type="text" name="wifipwd" value="$wifipwd$"></td>
          </tr>
          <tr>
            <th>Loco Name:</th>
            <td><input type="text" name="loconame" value="$loconame$"></td>
          </tr>
          <tr>
            <th>Loco Address:</th>
            <td><input type="text" name="locoaddr" value="$locoaddr$"></td>
          </tr>
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

WebServer ConfigWeb::server(80);

void ConfigWeb::handleRoot()
{
    String wifiap = settings.get("wifiap");
    String wifissid = settings.get("wifissid");
    String wifipwd = settings.get("wifipwd");
    String loconame = settings.get("loconame");
    String locoaddr = settings.get("locoaddr");

    String form = String(htmlRoot);
    form.replace("$wifiap$", (wifiap == "on") ? "checked" : "");
    form.replace("$wifissid$", wifissid);
    form.replace("$wifipwd$", wifipwd);
    form.replace("$loconame$", loconame);
    form.replace("$locoaddr$", locoaddr);
    server.send(200, "text/html", form);
}

void ConfigWeb::handleSubmit()
{
    String wifiap = server.arg("wifiap");
    String wifissid = server.arg("wifissid");
    String wifipwd = server.arg("wifipwd");
    String loconame = server.arg("loconame");
    String locoaddr = server.arg("locoaddr");

    if (wifiap != "on")
        wifiap = "of";
    settings.put("wifiap", wifiap);
    settings.put("wifissid", wifissid);
    settings.put("wifipwd", wifipwd);
    settings.put("loconame", loconame);
    settings.put("locoaddr", locoaddr);

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