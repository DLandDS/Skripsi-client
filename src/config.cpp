#define SSD_ADDRESS 0
#define WIFI_PASSWORD_ADDRESS 33
#define HOST_ADDRESS 97
#define NAME_ADDRESS 351
#define MQTT_PASSWORD 384

#include <EEPROM.h>
#include <Arduino.h>
#include "main.hpp"
#include "config.hpp"
#include "utlis.hpp"

Config DataConfig = {
    "",
    "",
    "",
    "",
    ""};

void config::init()
{
    EEPROM.begin(500);
    EEPROM.get(0, DataConfig);
}

bool config::setData(Config dataConfig)
{
    EEPROM.put(0, dataConfig);
    return EEPROM.commit();
}

void config::reset()
{
    digitalWrite(pinLed, HIGH);
    Serial.println("Resetting...");
    Config resetConfig = {"",
                          "",
                          "",
                          "",
                          ""};
    config::setData(resetConfig);
    digitalWrite(pinBuzzer, LOW);
    resetFunc();
}
