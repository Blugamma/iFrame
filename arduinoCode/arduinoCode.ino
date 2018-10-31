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

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120
int buzzer = 4;
unsigned long delayStart = 0; // the time the delay started
bool delayRunning = false;
const char* ssid     = "OnePlus5";
const char* password = "password123";

void setup() {
  Serial.begin(9600);
  pinMode(buzzer, OUTPUT);
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  delayStart = millis();   // start delay
  delayRunning = true; // not finished yet
  wifi_setup();
}

uint8_t gHue = 0; // rotating "base color" used by many of the patterns

void flashRainbowLED(int timer){
  delay(timer);
  noTone(buzzer);
  FastLED.show(); 
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
  delay(timer);
  tone(buzzer, 1000);
  FastLED.show();
  fill_solid(leds, NUM_LEDS, CRGB::Black);
}



void flashSolidLED(int delayer, uint32_t color){
  delay(delayer);
  noTone(buzzer);
  FastLED.show(); 
  fill_solid( leds, NUM_LEDS, color);
  delay(delayer);
  tone(buzzer, 1000);
  FastLED.show();
  fill_solid(leds, NUM_LEDS, CRGB::Black);
}


 
void timer(int delayer, uint32_t color, int lengthOfTime){
   if (delayRunning && ((millis() - delayStart) >= lengthOfTime)) {
      // prevent this code being run more then once
      flashSolidLED(delayer, color);
      //delayRunning = false;
  }
}

void wifi_setup(){
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

void loop()
{
  //flashRainbowLED(1000);
  //flashSolidLED(3000, CRGB::Blue);
  //timer(2000, CRGB::Red, 10000);
  //digitalWrite(buzzer, HIGH);
  //Serial.write("buzzer off");
  //delay(2000);
  // send the 'leds' array out to the actual LED strip
  



}
