/*
 * Copyright (c) 2024-2025 Volodymyr "Vova" Tymoshchuk
 * Distributed under MIT licence, https://github.com/vova-tymosh/RCC/blob/main/LICENSE
 * For more details go to https://github.com/vova-tymosh/RCC
 *
 * The above copyright notice shall be included in all
 * copies or substantial portions of the Software.
 */
#if defined(ARDUINO_ARCH_ESP32)

#include <WiFi.h>
#include <WebServer.h>
#include "ConfigWeb.h"
#include "Settings.h"

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

WebServer ConfigWeb::server(80);

void ConfigWeb::handleRoot()
{
    String form = String(htmlPrefix);
    String name = storage.openFirst();
    while (!name.isEmpty()) {
        String inputbox = String(htmlInput);
        inputbox.replace("$user_readable$", name);
        inputbox.replace("$key$", name);
        inputbox.replace("$value$", settings.get(name.c_str()));
        form += inputbox;
        name = storage.openNext();
    }
    form += htmlSuffix;
    server.send(200, "text/html", form);
}

void ConfigWeb::handleSubmit()
{
    for (int i = 0; i < server.args(); i++) {
        String name = server.argName(i);
        settings.put(server.argName(i).c_str(), server.arg(i));
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