/*
  MQTT DHT22, when "temperature, c" is sent it returns the temperature in celcius
  when "humidity" is sent it returns the humidity as measured by the DHT22
  the signal pin is connected to a pull-ip resistor and GPIO 2
*/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
//#include <DHT.h> //DHT funcions not needed at the moment

//#define DHTTYPE DHT11
//#define DHTPIN  0 // Pin D3 (https://i.pinimg.com/originals/60/6c/d6/606cd6a0ccd75d60a9684d3a2ab95898.png)
#define pinout 16 // Pin D0

// Update these with values suitable for your network.

const char* ssid = "****";
const char* password = "****";
const char* mqtt_server = "123.123.123.123";
const char* clientID = "AntNest";
const char* mqtt_username = "****";
const char* mqtt_password = "****";
const char* outTopic = "home/AntOut";
const char* inTopic = "home/AntIn";
const char* waterTopic = "home/AntWater";

// Initialize DHT sensor
// NOTE: For working with a faster than ATmega328p 16 MHz Arduino chip, like an ESP8266,
// you need to increase the threshold for cycle counts considered a 1 or 0.
// You can do this by passing a 3rd parameter for this threshold.  It's a bit
// of fiddling to find the right value, but in general the faster the CPU the
// higher the value.  The default for a 16mhz AVR is a value of 6.  For an
// Arduino Due that runs at 84mhz a value of 30 works.
// This is for the ESP8266 processor on ESP-01
//DHT dht(DHTPIN, DHTTYPE, 11); // 11 works fine for ESP8266

//float humidity, temp_c;  // Values read from sensor
// Generally, you should use "unsigned long" for variables that hold time
//unsigned long previousMillis = 0;        // will store last temp was read
//const long interval = 5000;              // interval at which to read sensor
unsigned long water = 0;                 // Counter for water dispenser
 
WiFiClient espClient;
PubSubClient client(espClient);
char msg[50];

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

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
  // Conver the incoming byte array to a string
  payload[length] = '\0'; // Null terminator used to terminate the char array
  String message = (char*)payload;

  Serial.print("Message arrived on topic: [");
  Serial.print(topic);
  Serial.print("], ");
  Serial.println(message);

  /*if(message == "temperature"){
    gettemperature();
    Serial.print("Sending temperature:");
    Serial.println(temp_c);
    dtostrf(temp_c , 2, 2, msg);
    client.publish(outTopic, msg);
  } else if (message == "humidity"){
    gettemperature();
    Serial.print("Sending humidity:");
    Serial.println(humidity);
    dtostrf(humidity , 2, 2, msg);
    client.publish(outTopic, msg);
  } else */ if (message == "irrigate_2s"){   //sufficent time for pumping 4ml of water trough the peristaltic pump (pump: 2ml/s)
    Serial.print("Irrigating for 2s");
    digitalWrite(pinout, LOW);
    delay(2000);
    digitalWrite(pinout, HIGH);
    water = water+4;
    Serial.print(water);
    dtostrf(water, 4, 1, msg);
    Serial.print(msg);
    client.publish(waterTopic, msg);
  } /* else if (message == "irrigate_3s"){
    digitalWrite(pinout, LOW);
    delay(3000);
    digitalWrite(pinout, HIGH);
  }  else if (message == "irrigate_4s"){
    digitalWrite(pinout, LOW);
    delay(4000);
    digitalWrite(pinout, HIGH);
  }  else if (message == "irrigate_5s"){
    digitalWrite(pinout, LOW);
    delay(5000);
    digitalWrite(pinout, HIGH);
  } 
*/
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientID, mqtt_username, mqtt_password)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(outTopic, clientID);
      // ... and resubscribe
      client.subscribe(inTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 10 seconds");
      // Wait 5 seconds before retrying
      delay(10000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  //Serial.setDebugOutput(true);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  //dht.begin();           // initialize temperature sensor
  pinMode(pinout, OUTPUT);
  digitalWrite(pinout, HIGH);

}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

/*
void gettemperature() {
  // Wait at least 2 seconds seconds between measurements.
  // if the difference between the current time and last time you read
  // the sensor is bigger than the interval you set, read the sensor
  // Works better than delay for things happening elsewhere also
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis >= interval) {
    // save the last time you read the sensor
    previousMillis = currentMillis;   

    // Reading temperature for humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
    humidity = dht.readHumidity();          // Read humidity (percent)
    temp_c = dht.readTemperature();     // Read temperature as Celcius
    // Check if any reads failed and exit early (to try again).
    if (isnan(humidity) || isnan(temp_c)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
  }
}
*/
