#pragma once

struct Config
{
    char ssid[32];
    char wifi_password[63];
    char host[253];
    char name[32];
    char mqtt_password[63];
};

namespace config
{
    void init();
    void reset();
    bool setData(Config dataConfig);
}

extern Config DataConfig;