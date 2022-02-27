#include <Arduino.h>
#include <EEPROM.h>
#include "config.hpp"
#include "httpserver.hpp"
#include <PubSubClient.h>
#include "main.hpp"
#include <string>

const int threshold = 250;
bool setupmode = false;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void callback(char *topic, byte *payload, unsigned int length)
{
  digitalWrite(pinLed, HIGH);
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 2; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  int readSensor = analogRead(MQ135) + 20;

  if (readSensor > threshold)
  {
    if (digitalRead(pinBuzzer) == LOW)
    {
      digitalWrite(pinBuzzer, HIGH);
    }
    else
    {
      digitalWrite(pinBuzzer, LOW);
    }
  }
  else
  {
    digitalWrite(pinBuzzer, LOW);
  }
  std::string encoded = std::to_string(readSensor);
  const char *playload = encoded.c_str();
  client.publish(DataConfig.name, playload);
  digitalWrite(pinLed, LOW);
  Serial.println();
}

void setup()
{
  Serial.begin(9600);
  Serial.println("Program Started!");
  pinMode(MQ135, INPUT);
  pinMode(pinLed, OUTPUT);
  pinMode(pinBuzzer, OUTPUT);
  digitalWrite(pinLed, LOW);
  digitalWrite(pinBuzzer, LOW);
  pinMode(pinButton, INPUT_PULLUP);
  config::init();
  setupmode = strcmp(DataConfig.name, "") == 0;
  delay(3000);
  int resetcounter = 0;

  while (digitalRead(pinButton) == 0)
  {
    if (resetcounter > 3)
    {
      digitalWrite(pinBuzzer, HIGH);
      delay(500);
      config::reset();
    }
    Serial.printf("\nReset! %d", resetcounter);
    resetcounter = resetcounter + 1;
    delay(1000);
  }
  Serial.println();
  if (setupmode)
  {
    if (WiFi.softAP("Sensor Node"))
    {
      Serial.println("Hostpot created.");
      digitalWrite(pinLed, LOW);
      delay(500);
      digitalWrite(pinLed, HIGH);
      delay(500);
      digitalWrite(pinLed, LOW);
      delay(500);
      digitalWrite(pinLed, HIGH);
      delay(500);
      Serial.println("Menjalankan webserver...");
      HTTPServer_init();
    }
    else
    {
      Serial.println("Hostpot creating is failed.");
    }
  }
  else
  {
    digitalWrite(pinLed, LOW);
    delay(1000);
    digitalWrite(pinLed, HIGH);

    Serial.printf("Device name : %s\n", DataConfig.name);
    WiFi.mode(WIFI_STA);
    WiFi.begin(DataConfig.ssid, DataConfig.wifi_password);

    Serial.print("WiFi connecting");
    while (WiFi.status() != WL_CONNECTED)
    {
      Serial.print(".");
      delay(500);
    }

    Serial.print("\nIP address: ");
    Serial.println(WiFi.localIP());
    Serial.printf("Connecting to %s...\n", DataConfig.host);
    client.setServer(DataConfig.host, 1883);
    client.setCallback(callback);
  }
}

void reconnect()
{ // Loop until we're reconnected
  digitalWrite(pinLed, HIGH);
  digitalWrite(pinBuzzer, LOW);
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = DataConfig.name;
    clientId += String("-");
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), DataConfig.name, DataConfig.mqtt_password))
    {
      Serial.println("reconnected");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(1000);
    }
  }
}

void loop()
{
  if (setupmode)
  {
    http.handleClient();
  }
  else
  {
    if (!client.connected())
    {
      reconnect();
    }
    client.loop();
  }
}