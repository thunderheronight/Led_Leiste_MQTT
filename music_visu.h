#include <Arduino.h>
#include <WebSocketsServer.h>
#include <Hash.h>
#include <WiFiUdp.h>
#include "ws2812_i2s.h"



// Set to the number of LEDs in your LED strip
static WS2812 ledstrip;
static Pixel_t pixels[LED_COUNT];
// Maximum number of packets to hold in the buffer. Don't change this.
#define BUFFER_LEN 1024
// Toggles FPS output (1 = print FPS over serial, 0 = disable output)
#define PRINT_FPS 0

unsigned int localPort = 7777;
char packetBuffer[BUFFER_LEN];
int wifi_counter;

uint8_t N = 0;
#if PRINT_FPS
uint16_t fpsCounter = 0;
uint32_t secondTimer = 0;
#endif

// LED strip

WiFiUDP port;

//setup
int music_visu_setup() {
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("");
  Serial.println(WIFI_SSID);
  Serial.println(WIFI_PASSWORD);
  Serial.println(ip);
  // Connect to wifi and print the IP address over serial
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(S_LED, LOW);
    delay(250);
    digitalWrite(S_LED, HIGH);
    delay(250);
    Serial.print(".");
    wifi_counter = wifi_counter + 1;
    if (wifi_counter == 20) {
      Serial.println("Wifi failed, using Backup...");
      led_blink(5);
      WiFi.config(ip2, gateway2, subnet);
      WiFi.begin(WIFI_SSID2, WIFI_PASSWORD2);
    } else if (wifi_counter == 60) {
      Serial.println("Wifi 2 failed, Restarting...");
      ESP.reset();
    }
  }
  led_blink(2);
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  port.begin(localPort);
  ledstrip.init(LED_COUNT);
  server.begin();
  return (0);
}

//loop
void music_visu_loop() {
  // Read data over socket
  int packetSize = port.parsePacket();
  // If packets have been received, interpret the command
  if (packetSize) {
    int len = port.read(packetBuffer, BUFFER_LEN);
    for (int i = 0; i < len; i += 4) {
      packetBuffer[len] = 0;
      N = packetBuffer[i];
      pixels[N].R = (uint8_t)packetBuffer[i + 1];
      pixels[N].G = (uint8_t)packetBuffer[i + 2];
      pixels[N].B = (uint8_t)packetBuffer[i + 3];
    }
    ledstrip.show(pixels);
#if PRINT_FPS
    fpsCounter++;
#endif
  }
#if PRINT_FPS
  if (millis() - secondTimer >= 1000U) {
    secondTimer = millis();
    Serial.printf("FPS: %d\n", fpsCounter);
    fpsCounter = 0;
  }
#endif

}
