#include <FastLED.h>
#include <SPI.h>
#include <WiFi.h>
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
const int trigPin = 7;
const int echoPin = 6;
// defines variables
long duration;
int distance;


void setup() {
  //delay( 3000 ); // power-up safety delay
  Serial.begin(9600);
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  wifi_setup();
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
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
    delay(5000);
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
void addDropEffect(CRGB dropColor, CRGB mainColor, int lowLED, int highLED){ 
  int pos = random(lowLED, highLED);
    leds[pos] = dropColor;
    FastLED.show();
    delay(100);
    leds[pos] = mainColor;  
    FastLED.show();
    delay(random(100, 1000));
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
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
    if (!client.connected()) {
      reconnect();
    }
    // Clears the trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
    // Calculating the distance
    distance= duration*0.034/2;
    // Prints the distance on the Serial Monitor
    Serial.print("Distance: ");
    Serial.println(distance);
        client.loop();
    String curr_payload((char*)inc_payload); //convert to a string data type/////
    FastLED.show();

    //Show the Sunny LEDS
    if (curr_payload == "Clear" && distance > 0 && distance <= 100) {
      for (int i = 0; i <= 11; i++) {
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
    else {
      for (int i = 0; i <= 11; i++) {
        leds[i] = CRGB::Black;
      }
    }

    //Show the Rainy LEDS
    if (curr_payload == "Rain" && distance > 0 && distance <= 100) {
      for (int i = 12; i <= 23; i++) {   
      leds[i] = CRGB::Blue;   
      addDropEffect(CRGB::DeepSkyBlue, CRGB::Blue, 12, 23);
      }
    }
    else {
      for (int i = 12; i <= 23; i++) {
        leds[i] = CRGB::Black;
      }
    }

    //Show the Stormy LEDS
    if (curr_payload == "Thunderstorm" && distance > 0 && distance <= 100) {
      for (int i = 24; i <= 35; i++) {
        leds[i] = CRGB::DimGray;  
        addDropEffect(CRGB::Yellow, CRGB::DimGray, 24, 35);
      
    }
    }
    else {
      for (int i = 24; i <= 35; i++) {
        leds[i] = CRGB::Black;
      }
    }

    //Show the Cloudy LEDS
    if (curr_payload == "Clouds" || curr_payload == "Atmosphere" && distance > 0 && distance <= 100) {
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
    if (curr_payload == "Snow" && distance > 0 && distance <= 100) {
      for (int i = 48; i <= 59; i++) {
        leds[i] = CRGB::Gray;
        addDropEffect(CRGB::GhostWhite, CRGB::Gray, 48, 59);
        
      }
    }
    else {
      for (int i = 48; i <= 59; i++) {
        leds[i] = CRGB::Black;
      }
    }

    if (curr_payload == "OFF") {
      for (int i = 0; i <= 59; i++) {
        leds[i] = CRGB::Black;
      }
    }
 

  
}
