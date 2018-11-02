#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>


FASTLED_USING_NAMESPACE

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    3
//#define CLK_PIN   4
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    64
CRGB leds[NUM_LEDS];
WiFiClient espClient;
PubSubClient client(espClient);
#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120
int buzzer = 4;
unsigned long delayStart = 0; // the time the delay started
bool delayRunning = false;
const char* ssid     = "OnePlus5";
const char* password = "password123";
const char* mqttServer = "broker.i-dat.org";
const int mqttPort = 80;
uint8_t my_str[6]; // sting to store the incoming data from the publisher
String str;
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

void setup() {
  Serial.begin(9600);
  pinMode(buzzer, OUTPUT);
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  delayStart = millis();   // start delay
  delayRunning = true; // not finished yet
  wifi_setup();
  client.setClient(espClient);
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
    }
    else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
  client.publish("iFrame", "Hello ESP World");
  client.subscribe("iFrame");
}


void flashRainbowLED(int timer) {
  delay(timer);
  noTone(buzzer);
  FastLED.show();
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
  delay(timer);
  tone(buzzer, 1000);
  FastLED.show();
  fill_solid(leds, NUM_LEDS, CRGB::Black);
}



void flashSolidLED(int delayer, uint32_t color) {
  delay(delayer);
  noTone(buzzer);
  FastLED.show();
  fill_solid( leds, NUM_LEDS, color);
  delay(delayer);
  tone(buzzer, 1000);
  FastLED.show();
  fill_solid(leds, NUM_LEDS, CRGB::Black);
}



//void timer(int delayer, uint32_t color, int lengthOfTime) {
//  if (delayRunning && ((millis() - delayStart) >= lengthOfTime)) {
//    // prevent this code being run more then once
//    flashSolidLED(delayer, color);
//    //delayRunning = false;
//  }
//}

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
    my_str[i] = (char)payload[i];  // copies the payload to my_str
  }
  Serial.println();
  
  String str((char*)my_str); //convert to a string data type/////
  
 


}

void clearstring() {
  //Serial.flush(); // clears the buffer, you dont need this
  for (int r=0; r<7; r++){
  my_str[r] = '\0'; // deletes each block
  }
}
void loop()
{
  client.loop();
  FastLED.show();
   // Switch on the LED if an 1 was received as first character
  if (str == "timerOn"){
    Serial.print("LED on");
    fill_solid(leds, NUM_LEDS, CRGB::Green);
  }
  if (str == "timerOf"){
    Serial.print("LED off");
    fill_solid(leds, NUM_LEDS, CRGB::Black);
  }
  clearstring(); // clears the string
  //flashRainbowLED(1000);
  //flashSolidLED(3000, CRGB::Blue);
  //timer(2000, CRGB::Red, 10000);
  //digitalWrite(buzzer, HIGH);
  //Serial.write("buzzer off");
  //delay(2000);
  // send the 'leds' array out to the actual LED strip




}
