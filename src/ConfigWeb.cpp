#include <WiFi.h>
#include <WebServer.h>
#include "Storage.h"
#include "ConfigWeb.h"

const char* htmlRoot = R"(
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

const char* htmlSubmitted = R"(
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


void handleRoot() {
    char buffer[256];
    configWeb.storage.readOrCreate("wifiap", buffer, sizeof(buffer));
    String wifiap = String(buffer);
    configWeb.storage.readOrCreate("wifissid", buffer, sizeof(buffer));
    String wifissid = String(buffer);
    configWeb.storage.readOrCreate("wifipwd", buffer, sizeof(buffer));
    String wifipwd = String(buffer);
    configWeb.storage.readOrCreate("loconame", buffer, sizeof(buffer));
    String loconame = String(buffer);
    configWeb.storage.readOrCreate("locoaddr", buffer, sizeof(buffer));
    String locoaddr = String(buffer);

    String form = String(htmlRoot);
    form.replace("$wifiap$", (wifiap == "on") ? "checked" : "");
    form.replace("$wifissid$", wifissid);
    form.replace("$wifipwd$", wifipwd);
    form.replace("$loconame$", loconame);
    form.replace("$locoaddr$", locoaddr);
    configWeb.server.send(200, "text/html", form);
}

void handleSubmit() {
    String wifiap = server.arg("wifiap");
    String wifissid = server.arg("wifissid");
    String wifipwd = server.arg("wifipwd");
    String loconame = server.arg("loconame");
    String locoaddr = server.arg("locoaddr");

    if (wifiap != "on")
        wifiap = "of";
    configWeb.storage.write("wifiap", (void*)wifiap.c_str(), wifiap.length() + 1);
    configWeb.storage.write("wifissid", (void*)wifissid.c_str(), wifissid.length() + 1);
    configWeb.storage.write("wifipwd", (void*)wifipwd.c_str(), wifipwd.length() + 1);
    configWeb.storage.write("loconame", (void*)loconame.c_str(), loconame.length() + 1);
    configWeb.storage.write("locoaddr", (void*)locoaddr.c_str(), locoaddr.length() + 1);

////
    Serial.println("Form Submitted:");
    Serial.println(String("wifiap: ") + (wifiap ? "true" : "false"));
    Serial.println("wifissid: " + wifissid);
    Serial.println("wifipwd: " + wifipwd);
    Serial.println("loconame: " + loconame);
    Serial.println("locoaddr: " + locoaddr);
    Serial.println(strlen(htmlRoot));
    Serial.println(strlen(htmlSubmitted));
////      

    configWeb.server.send(200, "text/html", htmlSubmitted);
}


void ConfigWeb::begin() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/submit", HTTP_POST, handleSubmit);

  server.begin();
  Serial.println("[Web] Server started");
}
