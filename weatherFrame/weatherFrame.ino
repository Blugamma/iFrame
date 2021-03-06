#include <FastLED.h>
#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <MFRC522.h>
#include <NewPing.h>

FASTLED_USING_NAMESPACE


#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif
#define SS_PIN SDA
#define RST_PIN 9
#define TRIGGER_PIN 5
#define ECHO_PIN 4
#define MAX_DISTANCE 50

WiFiClient weatherFrame;
PubSubClient client(weatherFrame);
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); 

#define DATA_PIN    3
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    60
CRGB leds[NUM_LEDS];
#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  60
#define COOLING  55
#define SPARKING 120
char ssid[] = "OnePlus5";     //  your network SSID (name)
char pass[] = "password123";  // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status
const char* mqttServer = "test.mosquitto.org";
const int mqttPort = 1883;
uint8_t inc_payload[100]; // sting to store the incoming data from the publisher
String curr_payload;
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
int fadeAmount = 5;  // Set the amount to fade I usually do 5, 10, 15, 20, 25 etc even up to 255.
int brightness = 0;
// defines pins numbers
//const int trigPin = 7;
//const int echoPin = 6;
//long duration;
//long distance;
uint32_t period = 1 * 60000L;
String content= "";

  
void setup() {
   

  delay( 3000 ); // power-up safety delay
  //pinMode(echoPin, INPUT);
  //pinMode(trigPin, OUTPUT);
  
  Serial.begin(9600);
         // Initiate  SPI bus
 
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
   SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();
}

void wifi_setup() {
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv != "1.1.0") {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(2000);
  }

  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");
  printCurrentNet();
  printWifiData();
}


void printWifiData() {
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  Serial.print(mac[5], HEX);
  Serial.print(":");
  Serial.print(mac[4], HEX);
  Serial.print(":");
  Serial.print(mac[3], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[1], HEX);
  Serial.print(":");
  Serial.println(mac[0], HEX);

}

void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  Serial.print(bssid[5], HEX);
  Serial.print(":");
  Serial.print(bssid[4], HEX);
  Serial.print(":");
  Serial.print(bssid[3], HEX);
  Serial.print(":");
  Serial.print(bssid[2], HEX);
  Serial.print(":");
  Serial.print(bssid[1], HEX);
  Serial.print(":");
  Serial.println(bssid[0], HEX);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
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

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "weatherFrame";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("weatherFrame", "reconnected to Weather Frame");
      // ... and resubscribe
      client.subscribe("weatherFrame");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      // Wait 5 seconds before retrying
      delay(2000);
    }
  }
}

void addDrizzleEffect(CRGB dropColor, CRGB mainColor, int lowLED, int highLED) {
  int pos = random(lowLED, highLED);
  leds[pos] = dropColor;
  FastLED.show();
  delay(100);
  leds[pos] = mainColor;
  FastLED.show();
  delay(random(100, 1000));
}

void addRainEffect(CRGB dropColor, CRGB mainColor, int lowLED, int highLED) {
  int pos = random(lowLED, highLED);
  leds[pos] = dropColor;
  FastLED.show();
  delay(100);
  leds[pos] = mainColor;
  FastLED.show();
  delay(100);
}




void loop() {
   
  if (!client.connected()) {
    reconnect();
  }
 unsigned int distance = sonar.ping_cm();
 
  Serial.print("Distance: ");
  Serial.println(distance);

  client.loop();
  String curr_payload((char*)inc_payload); //convert to a string data type//

  
  FastLED.show();
  //curr_payload = "278.15";
  int curr_payload_int = curr_payload.toInt() - 273.15; //calculate celsius from the Kelvin value

  //Hard reset of the LEDs turning them all off.
  for (int i = 0; i <= 59; i++) {
      leds[i] = CRGB::Black;
    }

 // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  //Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     //Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    // Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
 
  content.toUpperCase();  
 
 if (content.substring(1) == "65 E3 8B C3") //change here the UID of the card/cards that you want to give access
  {
  //Very Cold
  if (curr_payload_int > 0 && curr_payload_int <= 10 && distance > 5 && distance <= 50) {
    for ( uint32_t tStart = millis();  (millis() - tStart) < period;  ) { /*This for loop is used to run the LEDs for exactly a minute*/
      for (int i = 0; i <= 11; i++) {
        leds[i] = CRGB::Purple;
        FastLED.show();
        delay(100);
      }
    }
  }
  else {
    for (int i = 0; i <= 59; i++) {
      leds[i] = CRGB::Black;
      FastLED.show();
    }
  }
  //Cold
  if (curr_payload_int > 10 && curr_payload_int <= 15 && distance > 5 && distance <= 50) {
    for ( uint32_t tStart = millis();  (millis() - tStart) < period;  ) { /*This for loop is used to run the LEDs for exactly a minute*/
      for (int i = 0; i <= 23; i++) {
        leds[i] = CRGB::Blue;
        FastLED.show();
        delay(100);
      }
    }
  }
  else {
    for (int i = 0; i <= 59; i++) {
      leds[i] = CRGB::Black;
    }
  }
  //Warm
  if (curr_payload_int > 15 && curr_payload_int <= 20 && distance > 5 && distance <= 50) {
    for ( uint32_t tStart = millis();  (millis() - tStart) < period;  ) { /*This for loop is used to run the LEDs for exactly a minute*/
      for (int i = 0; i <= 35; i++) {
        leds[i] = CRGB::Yellow;
        FastLED.show();
        delay(100);
      }
    }
  }
  else {
    for (int i = 0; i <= 59; i++) {
      leds[i] = CRGB::Black;
    }
  }
  //Hot
  if (curr_payload_int > 20 && curr_payload_int <= 25 && distance > 5 && distance <= 50) {
    for ( uint32_t tStart = millis();  (millis() - tStart) < period;  ) { /*This for loop is used to run the LEDs for exactly a minute*/
      for (int i = 0; i <= 47; i++) {
        leds[i] = CRGB::Orange;
        FastLED.show();
        delay(100);
      }
    }
  }
  else {
    for (int i = 0; i <= 59; i++) {
      leds[i] = CRGB::Black;
    }
  }

  //Very Hot
  if (curr_payload_int > 25 && curr_payload_int <= 35 && distance > 5 && distance <= 50) {
    for ( uint32_t tStart = millis();  (millis() - tStart) < period;  ) { /*This for loop is used to run the LEDs for exactly a minute*/
      for (int i = 0; i <= 59; i++) {
        leds[i] = CRGB::Red;
        FastLED.show();
        delay(100);
      }
    }
  }
  else {
    for (int i = 0; i <= 59; i++) {
      leds[i] = CRGB::Black;
    }
  }
  }
 

  if (content.substring(1) == "04 1E 64 DA 66 5A 80") //change here the UID of the card/cards that you want to give access
  {
  //Show the Sunny LEDS
  if (curr_payload == "Clear" && distance > 5 && distance <= 50) {
    for ( uint32_t tStart = millis();  (millis() - tStart) < period;  ) { /*This for loop is used to run the LEDs for exactly a minute*/
      for (int i = 26; i <= 35; i++) {
        leds[i] = CRGB::Orange;
        leds[i].fadeLightBy(brightness);
      }
      FastLED.show();
      brightness = brightness + fadeAmount;
      // reverse the direction of the fading at the ends of the fade:
      if (brightness == 0 || brightness == 255) {
        fadeAmount = -fadeAmount ;
      }
      delay(20); //delay of the fade in and out
    }
  }
  else {
    for (int i = 26; i <= 35; i++) {
      leds[i] = CRGB::Black;
    }
  }

  //Show the Rainy LEDS
  if (curr_payload == "Rain" && distance > 5 && distance <= 50) {
    for ( uint32_t tStart = millis();  (millis() - tStart) < period;  ) { /*This for loop is used to run the LEDs for exactly a minute*/
      for (int i = 14; i <= 23; i++) {
        leds[i] = CRGB::Blue;
        addRainEffect(CRGB::DeepSkyBlue, CRGB::Blue, 14, 23);
      }
    }
  }
  else {
    for (int i = 14; i <= 23; i++) {
      leds[i] = CRGB::Black;
    }
  }

  //Show the Drizzle LEDS
  if (curr_payload == "Drizzle" && distance > 5 && distance <= 50) {
    for ( uint32_t tStart = millis();  (millis() - tStart) < period;  ) { /*This for loop is used to run the LEDs for exactly a minute*/
      for (int i = 14; i <= 23; i++) {
        leds[i] = CRGB::Blue;
        addDrizzleEffect(CRGB::DeepSkyBlue, CRGB::Blue, 14, 23);
      }
    }
  }
  else {
    for (int i = 14; i <= 23; i++) {
      leds[i] = CRGB::Black;
    }
  }

  //Show the Stormy LEDS
  if (curr_payload == "Thunderstorm" && distance > 5 && distance <= 50) {
    for ( uint32_t tStart = millis();  (millis() - tStart) < period;  ) { /*This for loop is used to run the LEDs for exactly a minute*/
      leds[38] = CRGB::Yellow;
      leds[39] = CRGB::Yellow;
      leds[40] = CRGB::Yellow;
      leds[41] = CRGB::Yellow;
      leds[42] = CRGB::Yellow;
      leds[43] = CRGB::Yellow;
      leds[44] = CRGB::Yellow;
      leds[45] = CRGB::Yellow;
      leds[46] = CRGB::Yellow;
      leds[47] = CRGB::Yellow;
      FastLED.show();
      delay(random8(5) * 20);
      leds[38] = CRGB::Black;
      leds[39] = CRGB::Black;
      leds[40] = CRGB::Black;
      leds[41] = CRGB::Black;
      leds[42] = CRGB::Black;
      leds[43] = CRGB::Black;
      leds[44] = CRGB::Black;
      leds[45] = CRGB::Black;
      leds[46] = CRGB::Black;
      leds[47] = CRGB::Black;
      FastLED.show();
      delay(random8(5) * 600);
    }
  }
  else {
    for (int i = 38; i <= 47; i++) {
      leds[i] = CRGB::Black;
    }
  }

  //Show the Cloudy LEDS
  if (curr_payload == "Clouds" && distance > 5 && distance <= 50) {
    for ( uint32_t tStart = millis();  (millis() - tStart) < period;  ) { /*This for loop is used to run the LEDs for exactly a minute*/
      leds[50] = CRGB::Gray;
      leds[51] = CRGB::Gray;
      leds[52] = CRGB::Gray;
      leds[53] = CRGB::Gray;
      leds[54] = CRGB::Gray;
      leds[55] = CRGB::Gray;
      leds[56] = CRGB::Gray;
      leds[57] = CRGB::Gray;
      leds[58] = CRGB::Gray;
      leds[59] = CRGB::Gray;
      FastLED.show();
    }
  }
  else {
    for (int i = 0; i <= 9; i++) {
      leds[i] = CRGB::Black;
    }
  }

  //Show the Snowy LEDS
  if (curr_payload == "Snow" && distance > 5 && distance <= 50) {
    for ( uint32_t tStart = millis();  (millis() - tStart) < period;  ) { /*This for loop is used to run the LEDs for exactly a minute*/
      for (int i = 2; i <= 11; i++) {
        leds[i] = CRGB::Gray;
        addDrizzleEffect(CRGB::GhostWhite, CRGB::Gray, 2, 11);
      }
    }
  }
  else {
    for (int i = 2; i <= 11; i++) {
      leds[i] = CRGB::Black;
    }
  }
  }
  //Turn off all LEDs for testing
  if (curr_payload == "OFF") {
    for (int i = 0; i <= 59; i++) {
      leds[i] = CRGB::Black;
    }
  }
}
