/*

    Setting the time using WiFi.getTime();

    Originally written for WiFiNINA, adapted here for WiFiS3

    WiFi.getTime() connects to network time servers and gets the
    UNIX epoch (seconds since 1/1/1970, 00:00:00). It then sets
    the realtime clock epoch with that time

    Original version works on MKR1000, MKR1010, Nano 33 IoT.
    Current version fails to get the time on WiFiS3 library

    Make sure to add a file, arduino_secrets.h, with:
    #define SECRET_SSID ""    //  your network SSID (name)
    #define SECRET_PASS ""    // your network passwordx

    created 30 April 2019
    modified 30 Jun 2023
    by Tom Igoe
*/
#include <WiFiS3.h>
#include "arduino_secrets.h"

int reconnects = 0;  // how many times you've reconnected to the network

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  if (!Serial) delay(3000);
  // try to connect to the network:
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Attempting to connect to: " + String(SECRET_SSID));
    //Connect to WPA / WPA2 network:
    WiFi.begin(SECRET_SSID, SECRET_PASS);
    delay(2000);
  }
  Serial.println("connected to: " + String(SECRET_SSID));
  IPAddress ip = WiFi.localIP();
  Serial.print(ip);
  Serial.print("  Signal Strength: ");
  Serial.println(WiFi.RSSI());
  // You're connected, turn on the LED:
  digitalWrite(LED_BUILTIN, HIGH);

  // set the time from the network:
  unsigned long epoch;
  do {
    Serial.println("Attempting to get network time");
    epoch = WiFi.getTime();
    delay(2000);
  } while (epoch == 0);
  Serial.println(epoch);
}

void loop() {
}
