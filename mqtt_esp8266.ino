/*
 Basic ESP8266 MQTT example
 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.
 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off
 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h> 
// Update these with values suitable for your network.



int servoPin = 5; // d1 
Servo Servo1; 

int limitSwitch = 13; //d7
int state = LOW;


//ultrasonic
int trigPin = 12; //d6
int echoPin = 10; //sdd3
long duration;
int distance;





const char* ssid = "FoxFi31";
const char* password = "zero221211_00";
const char* mqtt_server = "192.168.43.233";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

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

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

//void callback(char* topic, byte* payload, unsigned int length) {
//  Serial.print("Message arrived [");
//  Serial.print(topic);
//  Serial.print("] ");
//  for (int i = 0; i < length; i++) {
//    Serial.print((char)payload[i]);
//  }
//  Serial.println();
//
//  // Switch on the LED if an 1 was received as first character
//  if ((char)payload[0] == '1') {
//    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
//    // but actually the LED is on; this is because
//    // it is active low on the ESP-01)
//  } else {
//    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
//  }
//
//}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
//    String clientId = "ESP8266Client-";
//    clientId += String(random(0xffff), HEX);


    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("sensor1","");
      client.publish("sensor2","");
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
//  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);

  Servo1.attach(servoPin); 
  
  Serial.begin(9600);
  pinMode(limitSwitch,INPUT);

// ultrasonic

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  
//  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }



  
int val = digitalRead(limitSwitch);


    // ultrasonic
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
//    distancemm = distance*10+30;

//==============================================================================
//deteksi sampah
    if( val != state ){
       state = val;
       
//       Serial.print("Terdeteksi = ");
       if( state == 0 && distance <=100 ){
         Servo1.write(90);     
         client.publish("sensor1", "sampah elektronik");
         client.publish("sensor2", "penuh");
//         Serial.println("p");
         Serial.println( "sampah elektrtonik" );
         delay(1000);
         }
       else if( state == 0 && distance >=200 ){
         Servo1.write(90);
        
         client.publish("sensor1", "sampah elektronik");        
         client.publish("sensor2", "kosong");
         Serial.println( "sampah elektronik");
          delay(1000);
        }
      else if( state == 1 && distance <=100 ){
         Servo1.write(0);
       
         client.publish("sensor1", "non elektronik");        
         client.publish("sensor2", "penuh");
         Serial.println( "non elektronik");
           delay(1000);
        }
        
      else if( state == 1 && distance >=200 ){
         Servo1.write(0);
         delay(1000);
         client.publish("sensor1", "non elekrtonik");        
         client.publish("sensor2", "kosong");
         Serial.println( "non elektronik");
      delay(1000);
        }
      else{
            
        }  
    }



    
//deteksi kapasitas
//
//      if (distance <=100)
//      {
//          client.publish("sensor2", "penuh");
//            Serial.println("p");
//      }
//      else (distance >= 200);
//      {
//        client.publish("sensor2", "");
//        Serial.println("");
//      }
//
//
//delay(250);







  
  client.loop();

//  unsigned long now = millis();
//  if (now - lastMsg > 2000) {
//    lastMsg = now;
//    ++value;
//    snprintf (msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
//    Serial.print("Publish message: ");
//    Serial.println(msg);


    
    
//  }
}
