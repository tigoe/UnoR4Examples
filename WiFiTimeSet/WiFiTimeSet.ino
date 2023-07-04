/*

    Setting the time using WiFi.getTime();

    Originally written for WiFiNINA, adapted here for WiFiS3

    WiFi.getTime() connects to network time servers and gets the
    UNIX epoch (seconds since 1/1/1970, 00:00:00). It then sets
    the realtime clock epoch with that time

    Original version works on MKR1000, MKR1010, Nano 33 IoT.
    Current version fails to get the time on WiFiS3 library.
    It goes into a continual loop of checking for the time.

    For more on NTP time servers and the messages needed to communicate 
    with them, see http://en.wikipedia.org/wiki/Network_Time_Protocol

    NTP code adapted from Michael Margolis' work on WiFiUdpNTPClient example

    this is a work in progress. WiFi.getTime() does not yet exist 
    for WiFiS3
    
    Make sure to add a file, arduino_secrets.h, with:
    #define SECRET_SSID ""    //  your network SSID (name)
    #define SECRET_PASS ""    // your network passwordx

    created 30 April 2019
    modified 30 Jun 2023
    by Tom Igoe
*/
#include <WiFiS3.h>
#include <WiFiUdp.h>
#include "RTC.h"
#include "arduino_secrets.h"

int reconnects = 0;  // how many times you've reconnected to the network
RTCTime now;
// time zone offset: NYC
const int timeZone = -5;

WiFiUDP Udp;                         // A UDP instance to let us send and receive packets over UDP
const int NTP_PACKET_SIZE = 48;      // NTP timestamp is in the first 48 bytes of the message
byte packetBuffer[NTP_PACKET_SIZE];  //buffer to hold incoming and outgoing packets


void setup() {
  Serial.begin(9600);
  RTC.begin();
  Udp.begin(2390);
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
    // get the unix epoch:
    // this needs to be replaced with WiFi.getTime():
    epoch = getTimeFromServer();
    delay(2000);
  } while (epoch == 0);
  now = RTCTime(epoch);
  now.setHour(now.getHour() + timeZone);
  RTC.setTime(now);
}

void loop() {
  RTCTime now;
  RTC.getTime(now);
  Serial.println(now.toString());
  delay(1000);
}

unsigned long getTimeFromServer() {


  sendNTPpacket();  // send an NTP packet to a time server
  // wait to see if a reply is available
  delay(1000);

  if (Udp.parsePacket()) {
    Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    //or two words, long. First, extract the two words:
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);

    // Combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;

    // Now convert NTP time into everyday time:
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    unsigned long secondsSince1970 = secsSince1900 - seventyYears;
    return secondsSince1970;
  }
  // if you fail, return 0:
  return 0;
}

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket() {
  // IPAddress address(162, 159, 200, 123);  // pool.ntp.org NTP server

  // set all bytes in the buffer to 0
  for (int i = 0; i < NTP_PACKET_SIZE; i++) {
    packetBuffer[i] = 0;
  }
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;  // LI, Version, Mode
  packetBuffer[1] = 0;           // Stratum, or type of clock
  packetBuffer[2] = 6;           // Polling Interval
  packetBuffer[3] = 0xEC;        // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket("pool.ntp.org", 123);  //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}
