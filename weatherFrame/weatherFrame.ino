#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
FASTLED_USING_NAMESPACE


#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

WiFiClient weatherFrame;
PubSubClient client(weatherFrame);

#define DATA_PIN    3
//#define CLK_PIN   4
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    60
CRGB leds[NUM_LEDS];
#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

const char* ssid     = "OnePlus5";
const char* password = "password123";
const char* mqttServer = "broker.i-dat.org";
const int mqttPort = 80;
uint8_t inc_payload[100]; // sting to store the incoming data from the publisher
String curr_payload;
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
int fadeAmount = 5;  // Set the amount to fade I usually do 5, 10, 15, 20, 25 etc even up to 255.
int brightness = 0;

void setup() {
  Serial.begin(9600);
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  wifi_setup();
  client.setClient(weatherFrame);
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("weatherFrame")) {
      Serial.println("connected");
    }
    else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
  client.publish("weatherFrame", "Hello Weather Frame");
  client.subscribe("weatherFrame");
}

long lastReconnectAttempt = 0;

boolean reconnect() {
  if (client.connect("weatherFrame")) {
    // Once connected, publish an announcement...
    client.publish("weatherFrame", "hello world");
    // ... and resubscribe
    client.subscribe("weatherFrame");
  }
  return client.connected();
}

void wifi_setup() {
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    inc_payload[i] = (char)payload[i];  // copies the payload to my_str
  }
  inc_payload[length] = '\0';
  Serial.println();
}

void clearPayload() {
  //Serial.flush(); // clears the buffer, you dont need this
  for (int r = 0; r < 7; r++) {
    inc_payload[r] = '\0'; // deletes each block
  }
}

void loop() {
  if (!client.connected()) {
    long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      // Attempt to reconnect
      if (reconnect()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {
    // Client connected
    client.loop();
    String curr_payload((char*)inc_payload); //convert to a string data type/////
    FastLED.show();

    //Show the Sunny LEDS
    if (curr_payload == "sunny") {
      for (int i = 0; i <= 11; i++) {
        leds[i] = CRGB::Orange;
        leds[i].fadeLightBy(brightness);
    }
    FastLED.show();
    brightness = brightness + fadeAmount;
    // reverse the direction of the fading at the ends of the fade:
    if(brightness == 0 || brightness == 255){
      fadeAmount = -fadeAmount ;
    }   
       delay(200);
    }
    else {
      for (int i = 0; i <= 11; i++) {
        leds[i] = CRGB::Black;
      }
    }

    //Show the Rainy LEDS
    if (curr_payload == "rainy") {
      for (int i = 12; i <= 23; i++) {
        leds[i] = CRGB::Blue;
      }
    }
    else {
      for (int i = 12; i <= 23; i++) {
        leds[i] = CRGB::Black;
      }
    }

    //Show the Stormy LEDS
    if (curr_payload == "stormy") {
      for (int i = 24; i <= 35; i++) {
        leds[i] = CRGB::Yellow;
      }
    }
    else {
      for (int i = 24; i <= 35; i++) {
        leds[i] = CRGB::Black;
      }
    }

    //Show the Overcast LEDS
    if (curr_payload == "overcast") {
      for (int i = 36; i <= 47; i++) {
        leds[i] = CRGB::Green;
      }
    }
    else {
      for (int i = 36; i <= 47; i++) {
        leds[i] = CRGB::Black;
      }
    }

    //Show the Snowy LEDS
    if (curr_payload == "snowy") {
      for (int i = 48; i <= 59; i++) {
        leds[i] = CRGB::Gray;
      }
    }
    else {
      for (int i = 48; i <= 59; i++) {
        leds[i] = CRGB::Black;
      }
    }
  }
}
