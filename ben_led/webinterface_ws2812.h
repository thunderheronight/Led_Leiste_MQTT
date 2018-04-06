#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WS2812FX.h>


// QUICKFIX...See https://github.com/esp8266/Arduino/issues/263
#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

              // checks WiFi every ...ms. Reset after this time, if WiFi cannot reconnect.
#define HTTP_PORT 80

#define DEFAULT_COLOR 0xFF0000
#define DEFAULT_BRIGHTNESS 150
#define DEFAULT_SPEED 2000
#define DEFAULT_MODE FX_MODE_LARSON_SCANNER
//Effects for the 4 Events
#define EVENT1 FX_MODE_COLOR_WIPE_RANDOM
#define EVENTSPEED1 1000
#define EVENT2 FX_MODE_STROBE_RAINBOW
#define EVENTSPEED2 1000
#define EVENT3 FX_MODE_STROBE
#define EVENTSPEED3 1000
#define EVENT4 FX_MODE_STROBE
#define EVENTSPEED4 1000
#define event_time 600 // Duration of Event


#define BRIGHTNESS_STEP 15              // in/decrease brightness by this amount per click
#define SPEED_STEP 10                   // in/decrease brightness by this amount per click

unsigned long last_wifi_check_time = 0;
String modes = "";

int change_mode = 1;

/*
   Build <li> string for all modes.
*/
void modes_setup() {
  modes = "";
  for (uint8_t i = 0; i < ws2812fx.getModeCount(); i++) {
    modes += "<li><a href='#' class='m' id='";
    modes += i;
    modes += "'>";
    modes += ws2812fx.getModeName(i);
    modes += "</a></li>";
  }
}

/* #####################################################
  #  Webserver Functions
  ##################################################### */

/*

void srv_handle_modes() {
  server.send(200, "text/plain", modes);
}

void srv_handle_set() {
  for (uint8_t i = 0; i < server.args(); i++) {
    if (server.argName(i) == "c") {
      uint32_t tmp = (uint32_t) strtol(&server.arg(i)[0], NULL, 16);
      if (tmp >= 0x000000 && tmp <= 0xFFFFFF) {
        ws2812fx.setColor(tmp);
      }
    }
    if (server.argName(i) == "p") {
      uint8_t tmp = (uint8_t) strtol(&server.arg(i)[0], NULL, 10);
      if (tmp == 100) {
        eeprom_change(200);
      }
    }
    if (server.argName(i) == "m") {
      uint8_t tmp = (uint8_t) strtol(&server.arg(i)[0], NULL, 10);
      e_mode = tmp % ws2812fx.getModeCount();
      Serial.println(e_mode);
      //ws2812fx.setMode(tmp % ws2812fx.getModeCount());
    }

    if (server.argName(i) == "b") {
      if (server.arg(i)[0] == '-') {
        ws2812fx.decreaseBrightness(BRIGHTNESS_STEP);
      } else {
        ws2812fx.increaseBrightness(BRIGHTNESS_STEP);
      }
    }

    if (server.argName(i) == "s") {
      if (server.arg(i)[0] == '-') {
        ws2812fx.decreaseSpeed(SPEED_STEP);
      } else {
        ws2812fx.increaseSpeed(SPEED_STEP);
      }
    }
  }
  server.send(200, "text/plain", "OK");
}
*/
void event_event_start() {
  if (event == 1) {
    Serial.println("Event 1");
    ws2812fx.setSpeed(EVENTSPEED1);
    ws2812fx.setMode(EVENT1);
    event = 5;
  } else if (event == 2) {
    Serial.println("Event 2");
    ws2812fx.setSpeed(EVENTSPEED2);
    ws2812fx.setMode(EVENT2);
    event = 5;
  } else if (event == 3) {
    Serial.println("Event 3");
    ws2812fx.setSpeed(EVENTSPEED3);
    ws2812fx.setMode(EVENT3);
    event = 5;
  } else if (event == 4) {
    Serial.println("Event 4");
    ws2812fx.setSpeed(EVENTSPEED4);
    ws2812fx.setMode(EVENT4);
    event = 5;
  }
}


//setup
int web_setup() {
  Serial.println();
  Serial.println();
  Serial.println("Starting...");

  modes.reserve(5000);
  modes_setup();

  Serial.println("WS2812FX setup");
  ws2812fx.init();
  if (e_mode >= 100) {
    event = e_mode - 100;
    ws2812fx.setMode(event + 50);
  } else {
    ws2812fx.setMode(DEFAULT_MODE);
  }
  ws2812fx.setColor(DEFAULT_COLOR);
  ws2812fx.setSpeed(DEFAULT_SPEED);
  ws2812fx.setBrightness(DEFAULT_BRIGHTNESS);
  ws2812fx.start();

  Serial.println("Wifi setup");
  wifi_setup();

  Serial.println("ready!");
  return (change_mode);
}
//loop
void web_loop() {
  unsigned long now = millis();
  ws2812fx.service();
  if (e_mode != ws2812fx.getMode() && event == 0) {
    ws2812fx.setMode(e_mode);
  }
  event_event_start();
  if (event == event_time) {
    Serial.println("End reached");
    event = 0;
    ws2812fx.setMode(e_mode);
    ws2812fx.setSpeed(DEFAULT_SPEED);
    Serial.println("E_mode nach End");
    Serial.println(e_mode);
    if (e_mode > ws2812fx.getModeCount()) {
      eeprom_change(200);
    }
  } else if (event >= 5) {
    event = event + 1;
    delay(10);
  }
  if (now - last_wifi_check_time > WIFI_TIMEOUT) {
    Serial.print("Checking WiFi... ");
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi connection lost. Reconnecting...");
      WiFi.reconnect();
      wifi_setup();
    } else {
      Serial.println("OK");
    }
    last_wifi_check_time = now;
  }
}
