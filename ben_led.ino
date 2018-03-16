#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WS2812FX.h>
#include <EEPROM.h>
#include <Adafruit_NeoPixel.h>
#include <PubSubClient.h>
//TEST

#define LED_COUNT 60
#define LED_PIN 3   // 0 = GPIO0, 2=GPIO2
#define S_LED 5
#define EEPROMAD 0

#define WIFI_SSID "Thunder 2.4"
#define WIFI_PASSWORD "21thunder90"
#define WIFI_SSID2 "Mein-Teil"
#define WIFI_PASSWORD2 "112Florian221"
#define HTTP_PORT 80
#define WIFI_TIMEOUT 30000
#define mqtt_server "10.21.11.10"
#define STATIC_IP                       // uncomment for static IP, set IP below
#ifdef STATIC_IP
IPAddress ip(10, 21, 11, 158);
IPAddress gateway(10, 21, 11, 1);

IPAddress ip2(192, 168, 178, 158);
IPAddress gateway2(192, 168, 178, 1);

IPAddress subnet(255, 255, 255, 0);
#endif
ESP8266WebServer server(80);
WiFiClient espClient;
PubSubClient client(espClient);
Adafruit_NeoPixel strip_temp = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

bool startup = true;
float max_brgt = 255;
int e_mode = 0;
int e_mode_old = 0;
int save_mode = 0;
int event;
uint32_t color;
int color_rgb;
// Timers auxiliar variables
long now = millis();
long lastMeasure = 0;
long lastreconnect = 0;
int wifi2 = 0;
String str_modes;

long buttonTimer = 10;
long PressTime = 2000;

void eeprom_change(int c_temp) {
  e_mode_old = EEPROM.read(EEPROMAD);
  delay(10);
  if (c_temp == 2) {
    Serial.println("Erster:");
    Serial.println(e_mode);
    if (e_mode_old > 1) {
      e_mode_old = 0;
    }
    e_mode = e_mode_old;
    Serial.println("Davor:");
    Serial.println(e_mode_old);
    e_mode = e_mode + 1;
  } else {
    if (c_temp > 200) {
      e_mode = 200;
    } else {
      e_mode = c_temp;
    }
  }
  Serial.println(e_mode);
  if (e_mode_old != e_mode) {
    Serial.println("Schreibe EEPROM...");
    Serial.println(e_mode);
    EEPROM.write(EEPROMAD, e_mode);
    delay(50);
    EEPROM.commit();
    EEPROM.end();
    delay(50);
  }
  Serial.println("Danach");
  Serial.println(e_mode);
  delay(1000);
  ESP.restart();
}

void str_modes_setup() {
  str_modes = "";
  for (uint8_t i = 0; i < ws2812fx.getModeCount(); i++) {
    str_modes += ws2812fx.getModeName(i);
    str_modes += ",";
  }
}
/*
   Connect to WiFi. If no connection is made within WIFI_TIMEOUT, ESP gets resettet.
*/
void wifi_setup() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  WiFi.mode(WIFI_STA);
#ifdef STATIC_IP
  WiFi.config(ip, gateway, subnet);
#endif
  unsigned long connect_start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(S_LED, LOW);
    delay(250);
    digitalWrite(S_LED, HIGH);
    delay(250);
    Serial.print(".");
    if (millis() - connect_start > WIFI_TIMEOUT) {
      Serial.println();
      if (wifi2 == 1) {
        Serial.println("Wifi 1 and 2 Failed. Restarting...");
        ESP.restart();
      } else {
        wifi2 = 1;
        connect_start = millis();
        Serial.print("Trying WiFi 2");
        led_blink(5);
        WiFi.config(ip2, gateway2, subnet);
        WiFi.begin(WIFI_SSID2, WIFI_PASSWORD2);
      }
    }
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  led_blink(2);
  Serial.println(WiFi.localIP());
  Serial.println();
}


void led_blink(int blinker) {
  for (int i = 0; i < blinker; i++) {
    digitalWrite(S_LED, LOW);
    delay(100);
    digitalWrite(S_LED, HIGH);
    delay(100);
  }
  digitalWrite(S_LED, HIGH);
}

void eeprom_read() {
  e_mode = EEPROM.read(EEPROMAD);
  delay(20);
  if (e_mode >  ws2812fx.getModeCount() && e_mode < 100) {
    event = e_mode - ws2812fx.getModeCount();
  } else if (e_mode == 200) {
    e_mode = 200;
  } else if (e_mode >= 100) {
    e_mode = e_mode - 100;
  }
}

#include "webinterface_ws2812.h"
#include "music_visu.h"
#include "main_handler.h"

void setup() {
  pinMode(S_LED, OUTPUT);
  Serial.begin(115200);
  EEPROM.begin(512);
  strip_temp.begin();
  digitalWrite(S_LED, HIGH);
  http_server_handler();
  modes_setup();
  str_modes_setup();
  eeprom_read();
  Serial.println("Setup");
  Serial.println(e_mode);
  if (e_mode > 200) {
    e_mode = 200;
  }
  if (e_mode == 200) {
    Serial.println("Starting Music Vis Setup");
    wifi_setup();
    port.begin(localPort);
    ledstrip.init(LED_COUNT);
    server.begin();
  } else {
    Serial.println("Starting WebEffects Setup");
    web_setup();
  }
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.reconnect();
    wifi_setup();
  }
  if (!client.loop()) {
    client.connect("ESP8266LedLeiste");
  }
  now = millis();
  /*
    if (now - lastreconnect > 60000 * 30) {
    lastreconnect = now;
    Serial.println("Reconnect");
    WiFi.reconnect();
    wifi_setup();
    }
  */
  if (e_mode == 200) {
    music_visu_loop();
  } else if (e_mode == 201) {
    for (int i = 0; i < LED_COUNT; i++) {
      pixels[i].R = 0;
      pixels[i].G = 0;
      pixels[i].B = 0;
    }
    ledstrip.show(pixels);
    delay(1000);
    e_mode = 200;
  } else {
    if (startup == true) {
      pub_routine();
      startup = false;
    }
    web_loop();
  }
  server.handleClient();
}


// This functions is executed when some device publishes a message to a topic that your ESP8266 is subscribed to
// Change the function below to add logic to your program, so when a device publishes a message to a topic that
// your ESP8266 is subscribed you can actually do something
void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  String modeTemp = "FX_MODE_";
  int mode_temp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic room/lamp, you check if the message is either on or off. Turns the lamp GPIO according to the message
  if (topic == "/led_leiste/mode/set") {
    //mode_temp = messageTemp.toInt();
    mode_temp = get_modenum(messageTemp);
    if (e_mode == 200 && e_mode != mode_temp && mode_temp <= ws2812fx.getModeCount()) {
      client.publish("/led_leiste/mode", ws2812fx.getModeName(mode_temp), true);
      e_mode = mode_temp;
      eeprom_change(mode_temp);
    } else if ( e_mode != 200 && mode_temp == 200) {
      client.publish("/led_leiste/mode", "Music Visu", true);
      e_mode = mode_temp;
      eeprom_change(mode_temp);
    } else if ( e_mode != 200 && e_mode != mode_temp && mode_temp <= ws2812fx.getModeCount()) {
      client.publish("/led_leiste/mode",  ws2812fx.getModeName(mode_temp), true);
      e_mode = mode_temp;
      M_CHANGE(mode_temp);
    } else {
      Serial.print("Mode out of reach");
    }
  } else if (topic == "/led_leiste/set") {
    if (messageTemp == "ON") {
      client.publish("/led_leiste", "ON", true);
      ws2812fx.start();
    } else {
      client.publish("/led_leiste", "OFF", true);
      ws2812fx.stop();
    }
  } else if (topic == "/led_leiste/color/set") {
    uint32_t color_t = (uint32_t) strtol(&messageTemp[0], NULL, 16);
    int r = ((color_t >> 16) & 0xFF);  // Extract the RR byte;
    int g = ((color_t >> 8) & 0xFF);   // Extract the GG byte;
    int b = ((color_t) & 0xFF);        // Extract the BB byte;
    String color_state = String(r) + "," + String(g) + "," + String(b);
    Serial.print("Color state: "); Serial.println(color_state);
    if (color_t >= 0x000000 && color_t <= 0xFFFFFF) {
      ws2812fx.setColor(color_t);
      client.publish("/led_leiste/color", color_state.c_str(), true);
      Serial.println("MQTT Color changed");
      Serial.println(ws2812fx.getColor());
    }
  } else if (topic == "/led_leiste/bright/set") {
    mode_temp = messageTemp.toInt();
    if (mode_temp >= 0 && mode_temp <= 255) {
      ws2812fx.setBrightness(mode_temp);
      client.publish("/led_leiste/bright", messageTemp.c_str(), true);
      Serial.println("MQTT Brightness changed");
    }
  } else if (topic == "/led_leiste/speed/set") {
    mode_temp = messageTemp.toInt();
    if (mode_temp >= 0 && mode_temp <= 255) {
      ws2812fx.setSpeed(mode_temp);
      client.publish("/led_leiste/speed", String(mode_temp).c_str(), true);
      Serial.println("MQTT Speed changed");
    }
  } else {
    Serial.print("Topic not subscriped");
    Serial.print(topic);
  }
  Serial.println();
}

//MQTT Reconnect
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    /*
      YOU MIGHT NEED TO CHANGE THIS LINE, IF YOU'RE HAVING PROBLEMS WITH MQTT MULTIPLE CONNECTIONS
      To change the ESP device ID, you will have to give a new name to the ESP8266.
      Here's how it looks:
       if (client.connect("ESP8266Client")) {
      You can do it like this:
       if (client.connect("ESP1_Office")) {
      Then, for the other ESP:
       if (client.connect("ESP2_Garage")) {
      That should solve your MQTT multiple connections problem
    */
    if (client.connect("ESP8266LedLeiste")) {
      Serial.println("connected");
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)
      client.subscribe("/led_leiste/set");
      client.loop();
      client.subscribe("/led_leiste/mode/set");
      client.loop();
      client.subscribe("/led_leiste/color/set");
      client.loop();
      client.subscribe("/led_leiste/bright/set");
      client.loop();
      client.subscribe("/led_leiste/speed/set");
      client.loop();
    } else {
      WiFi.reconnect();
      wifi_setup();
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
