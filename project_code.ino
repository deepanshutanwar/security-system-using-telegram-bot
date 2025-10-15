#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h> // Universal Telegram Bot Library written by Brian Lough: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <ArduinoJson.h>
//#include <Adafruit_BME280.h>
//#include <Adafruit_Sensor.h>
//const int pingPin = 13; // Trigger Pin of Ultrasonic Sensor
//const int echoPin = 12; // Echo Pin of Ultrasonic Sensor
const int trigPin = 2;
const int echoPin = 5;


// Replace with your network credentials
const char* ssid = "ANITA";
const char* password = "anshu.com";

// Use @myidbot to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
#define CHAT_ID "1034198293"

// Initialize Telegram BOT
#define BOTtoken "1503464103:AAFTHnUJqbBLRAwrUkKIMalUhrg59K5TG_M"  // your Bot Token (Get from Botfather)

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

//Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

// BME280 connect to ESP32 I2C (GPIO 21 = SDA, GPIO 22 = SCL)
// BME280 connect to ESP8266 I2C (GPIO 4 = SDA, GPIO 5 = SCL)
//Adafruit_BME280 bme;   

long duration;
int distance;

// Get BME280 sensor readings and return them as a String variable
String getReadings(){
long duration, inches, cm;
   
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
  String message = "distance: " + String(distance) + " cm \n";
  //message += "cm: " + String (cm) + " cm \n";
  return message;
}

long microsecondsToInches(long microseconds) {
   return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
   return microseconds / 29 / 2;
}

//Handle what happens when you receive new messages
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Welcome, " + from_name + ".\n";
      welcome += "Use the following command to get current readings.\n\n";
      welcome += "/readings \n";
      bot.sendMessage(chat_id, welcome, "");
    }

    if (text == "/readings") {
      String readings = getReadings();
      bot.sendMessage(chat_id, readings, "");
    }  
  }
}

void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);
  Serial.begin(115200);

  #ifdef ESP8266
    client.setInsecure();
  #endif

  // Init BME280 sensor
  //if (!bme.begin(0x76)) {
   // Serial.println("Could not find a valid BME280 sensor, check wiring!");
    //while (1);
  //}
  
  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
}

void loop() {
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
  if (distance != 10  ){
      bot.sendMessage(CHAT_ID, "distance is less than 100", "");
      Serial.println(" distance is less than 100");

  }
}

