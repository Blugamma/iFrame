#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
FASTLED_USING_NAMESPACE

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);
WiFiClient espClient;
PubSubClient client(espClient);

#define DATA_PIN    3
//#define CLK_PIN   4
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    60
CRGB leds[NUM_LEDS];
#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

int buzzer = 4;
const char* ssid     = "OnePlus5";
const char* password = "password123";
const char* mqttServer = "broker.i-dat.org";
const int mqttPort = 80;
uint8_t inc_payload[100]; // sting to store the incoming data from the publisher
String curr_payload;
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

void setup() {
  Serial.begin(9600);
  pinMode(buzzer, OUTPUT);
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

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
  timeClient.begin();
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
  for (int e = 0; e < 5; e++) {
    delay(delayer);
    noTone(buzzer);
    FastLED.show();
    fill_solid( leds, NUM_LEDS, color);
    delay(delayer);
    tone(buzzer, 1000);
    FastLED.show();
    fill_solid(leds, NUM_LEDS, CRGB::Black);
  }
}

long lastReconnectAttempt = 0;

boolean reconnect() {
  if (client.connect("ESP8266Client")) {
    // Once connected, publish an announcement...
    client.publish("iFrame", "hello world");
    // ... and resubscribe
    client.subscribe("iFrame");
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

void loop()
{
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

    // Switch on the LED if an 1 was received as first character
    if (curr_payload == "timerOn") {
      //Serial.print("LED on");
      fill_solid(leds, NUM_LEDS, CRGB::Green);
      clearPayload();
    }
    if (curr_payload == "timerOf") {
      //Serial.print("LED off");
      fill_solid(leds, NUM_LEDS, CRGB::Black);
      clearPayload();
    }
    //clearPayload();
    timeClient.update();

    if (curr_payload == timeClient.getFormattedTime()) {
      flashSolidLED(2000, CRGB::Red);
      clearPayload();
      fill_solid(leds, NUM_LEDS, CRGB::Black);
      noTone(buzzer);
    }
    //LED 5 =  6
    //LED 9 =  7
    //LED 13 = 8
    //LED 17 = 9
    //LED 21 = 10
    //LED 25 = 11
    //LED 29 = 12 
    //LED 33 = 1
    //LED 37 = 2
    //LED 41 = 3
    //LED 45 = 4
    //LED 49 = 5
   
     //Minute 1
    if (timeClient.getMinutes() == 1){
       leds[30] = CRGB::Red;
    }
    else{
       leds[30] = CRGB::Black;
    }
    //Minute 2
    if (timeClient.getMinutes() == 2){
       leds[31] = CRGB::Red;
    }
    else{
       leds[31] = CRGB::Black;
    }
    //Minute 3
    if (timeClient.getMinutes() == 3){
       leds[32] = CRGB::Red;
    }
    else{
       leds[32] = CRGB::Black;
    }
    //Minute 4
    if (timeClient.getMinutes() == 4){
       leds[33] = CRGB::Red;
    }
    else{
       leds[33] = CRGB::Black;
    }
    //Minute 5
    if (timeClient.getMinutes() == 5){
       leds[34] = CRGB::Red;
    }
    else{
       leds[34] = CRGB::Black;
    }
    //Minute 6
    if (timeClient.getMinutes() == 6){
       leds[35] = CRGB::Red;
    }
    else{
       leds[35] = CRGB::Black;
    }
    //Minute 7
    if (timeClient.getMinutes() == 7){
       leds[36] = CRGB::Red;
    }
    else{
       leds[36] = CRGB::Black;
    }
    //Minute 8
    if (timeClient.getMinutes() == 8){
       leds[37] = CRGB::Red;
    }
    else{
       leds[37] = CRGB::Black;
    }
    //Minute 9
    if (timeClient.getMinutes() == 9){
       leds[38] = CRGB::Red;
    }
    else{
       leds[38] = CRGB::Black;
    }
    //Minute 10
    if (timeClient.getMinutes() == 10){
       leds[39] = CRGB::Red;
    }
    else{
       leds[39] = CRGB::Black;
    }
    //Minute 11
    if (timeClient.getMinutes() == 11){
       leds[40] = CRGB::Red;
    }
    else{
       leds[40] = CRGB::Black;
    }
    //Minute 12
    if (timeClient.getMinutes() == 12){
       leds[41] = CRGB::Red;
    }
    else{
       leds[41] = CRGB::Black;
    }
    //Minute 13
    if (timeClient.getMinutes() == 13){
       leds[42] = CRGB::Red;
    }
    else{
       leds[42] = CRGB::Black;
    }
    //Minute 14
    if (timeClient.getMinutes() == 14){
       leds[43] = CRGB::Red;
    }
    else{
       leds[43] = CRGB::Black;
    }
    //Minute 15
    if (timeClient.getMinutes() == 15){
       leds[44] = CRGB::Red;
    }
    else{
       leds[44] = CRGB::Black;
    }
    //Minute 16
    if (timeClient.getMinutes() == 16){
       leds[45] = CRGB::Red;
    }
    else{
       leds[45] = CRGB::Black;
    }
    //Minute 17
    if (timeClient.getMinutes() == 17){
       leds[46] = CRGB::Red;
    }
    else{
       leds[46] = CRGB::Black;
    }
    //Minute 18
    if (timeClient.getMinutes() == 18){
       leds[47] = CRGB::Red;
    }
    else{
       leds[47] = CRGB::Black;
    }
    //Minute 19
    if (timeClient.getMinutes() == 19){
       leds[48] = CRGB::Red;
    }
    else{
       leds[48] = CRGB::Black;
    }
    //Minute 20
    if (timeClient.getMinutes() == 20){
       leds[49] = CRGB::Red;
    }
    else{
       leds[49] = CRGB::Black;
    }
    //Minute 21
    if (timeClient.getMinutes() == 21){
       leds[50] = CRGB::Red;
    }
    else{
       leds[50] = CRGB::Black;
    }
    //Minute 22
    if (timeClient.getMinutes() == 22){
       leds[51] = CRGB::Red;
    }
    else{
       leds[51] = CRGB::Black;
    }
    //Minute 23
    if (timeClient.getMinutes() == 23){
       leds[52] = CRGB::Red;
    }
    else{
       leds[52] = CRGB::Black;
    }
    //Minute 24
    if (timeClient.getMinutes() == 24){
       leds[53] = CRGB::Red;
    }
    else{
       leds[53] = CRGB::Black;
    }
    //Minute 25
    if (timeClient.getMinutes() == 25){
       leds[54] = CRGB::Red;
    }
    else{
       leds[54] = CRGB::Black;
    }
    //Minute 26
    if (timeClient.getMinutes() == 26){
       leds[55] = CRGB::Red;
    }
    else{
       leds[55] = CRGB::Black;
    }
    //Minute 27
    if (timeClient.getMinutes() == 27){
       leds[56] = CRGB::Red;
    }
    else{
       leds[56] = CRGB::Black;
    }
    //Minute 28
    if (timeClient.getMinutes() == 28){
       leds[57] = CRGB::Red;
    }
    else{
       leds[57] = CRGB::Black;
    }
    //Minute 29
    if (timeClient.getMinutes() == 29){
       leds[58] = CRGB::Red;
    }
    else{
       leds[58] = CRGB::Black;
    }
    //Minute 30
    if (timeClient.getMinutes() == 30){
       leds[59] = CRGB::Red;
    }
    else{
       leds[59] = CRGB::Black;
    }
    //Minute 31
    if (timeClient.getMinutes() == 31){
       leds[0] = CRGB::Red;
    }
    else{
       leds[0] = CRGB::Black;
    }
    //Minute 32
    if (timeClient.getMinutes() == 32){
       leds[1] = CRGB::Red;
    }
    else{
       leds[1] = CRGB::Black;
    }
    //Minute 33
    if (timeClient.getMinutes() == 33){
       leds[2] = CRGB::Red;
    }
    else{
       leds[2] = CRGB::Black;
    }
    //Minute 34
    if (timeClient.getMinutes() == 34){
       leds[3] = CRGB::Red;
    }
    else{
       leds[3] = CRGB::Black;
    }
    //Minute 35
    if (timeClient.getMinutes() == 35){
       leds[4] = CRGB::Red;
    }
    else{
       leds[4] = CRGB::Black;
    }
    //Minute 36
    if (timeClient.getMinutes() == 36){
       leds[5] = CRGB::Red;
    }
    else{
       leds[5] = CRGB::Black;
    }
    //Minute 37
    if (timeClient.getMinutes() == 37){
       leds[6] = CRGB::Red;
    }
    else{
       leds[6] = CRGB::Black;
    }
    //Minute 38
    if (timeClient.getMinutes() == 38){
       leds[7] = CRGB::Red;
    }
    else{
       leds[7] = CRGB::Black;
    }
    //Minute 39
    if (timeClient.getMinutes() == 39){
       leds[8] = CRGB::Red;
    }
    else{
       leds[8] = CRGB::Black;
    }
    //Minute 40
    if (timeClient.getMinutes() == 40){
       leds[9] = CRGB::Red;
    }
    else{
       leds[9] = CRGB::Black;
    }
    //Minute 41
    if (timeClient.getMinutes() == 41){
       leds[10] = CRGB::Red;
    }
    else{
       leds[10] = CRGB::Black;
    }
    //Minute 42
    if (timeClient.getMinutes() == 42){
       leds[11] = CRGB::Red;
    }
    else{
       leds[11] = CRGB::Black;
    }
    //Minute 43
    if (timeClient.getMinutes() == 43){
       leds[12] = CRGB::Red;
    }
    else{
       leds[12] = CRGB::Black;
    }
    //Minute 43
    if (timeClient.getMinutes() == 44){
       leds[13] = CRGB::Red;
    }
    else{
       leds[13] = CRGB::Black;
    }
    //Minute 44
    if (timeClient.getMinutes() == 45){
       leds[14] = CRGB::Red;
    }
    else{
       leds[14] = CRGB::Black;
    }
    //Minute 45
    if (timeClient.getMinutes() == 46){
       leds[15] = CRGB::Red;
    }
    else{
       leds[15] = CRGB::Black;
    }
    //Minute 46
    if (timeClient.getMinutes() == 47){
       leds[16] = CRGB::Red;
    }
    else{
       leds[16] = CRGB::Black;
    }
    //Minute 47
    if (timeClient.getMinutes() == 48){
       leds[17] = CRGB::Red;
    }
    else{
       leds[17] = CRGB::Black;
    }
    //Minute 48
    if (timeClient.getMinutes() == 49){
       leds[18] = CRGB::Red;
    }
    else{
       leds[18] = CRGB::Black;
    }
    //Minute 49
    if (timeClient.getMinutes() == 50){
       leds[19] = CRGB::Red;
    }
    else{
       leds[19] = CRGB::Black;
    }
    //Minute 50
    if (timeClient.getMinutes() == 51){
       leds[20] = CRGB::Red;
    }
    else{
       leds[20] = CRGB::Black;
    }
    //Minute 51
    if (timeClient.getMinutes() == 52){
       leds[21] = CRGB::Red;
    }
    else{
       leds[21] = CRGB::Black;
    }
    //Minute 52
    if (timeClient.getMinutes() == 53){
       leds[22] = CRGB::Red;
    }
    else{
       leds[22] = CRGB::Black;
    }
    //Minute 53
    if (timeClient.getMinutes() == 54){
       leds[23] = CRGB::Red;
    }
    else{
       leds[23] = CRGB::Black;
    }
    //Minute 54
    if (timeClient.getMinutes() == 55){
       leds[24] = CRGB::Red;
    }
    else{
       leds[24] = CRGB::Black;
    }
    //Minute 55
    if (timeClient.getMinutes() == 56){
       leds[25] = CRGB::Red;
    }
    else{
       leds[25] = CRGB::Black;
    }
    //Minute 56
    if (timeClient.getMinutes() == 57){
       leds[26] = CRGB::Red;
    }
    else{
       leds[26] = CRGB::Black;
    }
    //Minute 57
    if (timeClient.getMinutes() == 58){
       leds[27] = CRGB::Red;
    }
    else{
       leds[27] = CRGB::Black;
    }
    //Minute 58
    if (timeClient.getMinutes() == 59){
       leds[28] = CRGB::Red;
    }
    else{
       leds[28] = CRGB::Black;
    }
   
    if (timeClient.getHours() == 23){
       leds[24] = CRGB::Blue;
    }
    
    Serial.println(timeClient.getMinutes());
    
    //Minutes
//    for (int m = 1; m < 61; m++) {
//      if (timeClient.getMinutes() == m){
//        for (int ledNum = 0; ledNum < 60; ledNum++){
//          leds[ledNum] = CRGB::Red;
//          delay(2000);
//        }
//      }
//    }
    //Minute 1
    
   
  }
    

 }
