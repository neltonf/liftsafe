#include <ESP8266WiFi.h> 
#include <WiFiClientSecure.h> 

//***************MASTER***************//

//***************AMBIENT LIGHT SENSOR - PIN VALUES***************//
int temt6000Pin = A0; // Analog pin for reading sensor data

//***************AMBIENT LIGHT SENSOR - VARIABLES***************//
float light;
int light_value;


//***************PIR SENSOR - PIN VALUES***************//
int pirPin = D6;

//***************PIR SENSOR - VARIABLES***************//
//the time we give the sensor to calibrate (10-60 secs according to the datasheet)
int calibrationTime = 30;       
 
//the time when the sensor outputs a low impulse
long unsigned int lowIn;        
 
//the amount of milliseconds the sensor has to be low
//before we assume all motion has stopped
long unsigned int pause = 5000; 
 
boolean lockLow = true;
boolean takeLowTime; 

//***************WiFi - Pushbullet API Conn***************//
// PushingBox scenario DeviceId code and API
String deviceId = "v6390AC6C275B711";
const char* logServer = "api.pushingbox.com";

const char* ssid = "Nelton";
const char* password = "hellonelton";

//***************SETUP***************//
void setup() {

  // SERIAL DEBUGGER
  Serial.begin(115200);
  
  //AMBIENT LIGHT SENSOR
  pinMode(temt6000Pin, INPUT); //data pin for ambientlight sensor

  //PIR SENSOR
  pinMode(pirPin, INPUT);
  digitalWrite(pirPin, LOW);
  
  Serial.print("calibrating sensor ");    //give the sensor some time to calibrate
    for(int i = 0; i < calibrationTime; i++){
      Serial.print(".");
      delay(1000);
      }
    Serial.println(" done");
    Serial.println("SENSOR ACTIVE");
    delay(50);

}

void loop() {
  int light_value = analogRead(temt6000Pin);
  light = light_value * 0.0976;// percentage calculation
  
  if (light_value < 1000) {     // LIGHT IS PRESENT
  }
  else {                        //  LIGHT IS NOT PRESENT
    pirSensor ();
  }
}

void pirSensor () {
  if(digitalRead(pirPin) == HIGH) {   //the led visualizes the sensors output pin state
    Serial.println("---");
    Serial.print("motion detected at ");
    Serial.print(millis()/1000);
    Serial.println(" sec");
    sendNotification("Hello World from ESP8266!");
    delay(50);      
  }
}

void sendNotification(String message){

  Serial.println("- connecting to Home Router SID: " + String(ssid));
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.println("- succesfully connected");
  Serial.println("- starting client");
  
  WiFiClient client;

  Serial.println("- connecting to pushing server: " + String(logServer));
  if (client.connect(logServer, 80)) {
    Serial.println("- succesfully connected");
    
    String postStr = "devid=";
    postStr += String(deviceId);
    postStr += "&message_parameter=";
    postStr += String(message);
    postStr += "\r\n\r\n";
    
    Serial.println("- sending data...");
    
    client.print("POST /pushingbox HTTP/1.1\n");
    client.print("Host: api.pushingbox.com\n");
    client.print("Connection: close\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
  }
  client.stop();
  Serial.println("- stopping the client");
}
