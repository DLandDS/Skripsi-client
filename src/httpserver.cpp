#include "httpserver.hpp"
#include "html.hpp"
#include "config.hpp"
#include "utlis.hpp"
#include <string.h>

ESP8266WebServer http(80);

void HTTPServer_init()
{
    http.on("/setup", []()
            {
                Config data = {
                    "",
                    "",
                    "",
                    "",
                    ""};

                strcpy(data.ssid, http.arg("ap").c_str());
                strcpy(data.wifi_password, http.arg("ap-password").c_str());
                strcpy(data.host, http.arg("ip").c_str());
                strcpy(data.name, http.arg("name").c_str());
                strcpy(data.mqtt_password, http.arg("mqtt-password").c_str());

                Serial.println(data.ssid);
                Serial.println(data.wifi_password);
                Serial.println(data.host);
                Serial.println(data.name);
                Serial.println(data.mqtt_password);

                if (config::setData(data))
                {
                    http.send(200, "text/plain", "SUCCESS!");
                }
                else
                {
                    http.send(200, "text/plain", "Failed!");
                }
                delay(1000);
                resetFunc();
            });

    http.on("/", []()
            { http.send(200, "text/html", homepage); });

    http.begin();
}