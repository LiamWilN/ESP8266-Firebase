#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif

#include <Wire.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#define trigPin 4

//Wifi SSID
#define WIFI_SSID "Mandirigma"
//Wifi Password
#define WIFI_PASSWORD "PutanginaMo"
//Firebase WebAPI
#define API_KEY "AIzaSyBi7ttZWm5N_ybq_1vX6O0PII_mIZtMhF4"
//Firebase URL
#define DATABASE_URL "https://appinventor-e9bfb-default-rtdb.firebaseio.com/"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  
  Serial.println("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  pinMode(trigPin,  INPUT);

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
    config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
}

void loop() {
  
  String TriggerOut;
  
  int IRState = digitalRead(trigPin);
  if (IRState == 1){
    TriggerOut = "No Obstacle Detected";
  } else {
    TriggerOut = "Obstacle Detected";
  }
  
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
    // Write an Int number on the database path test/int
    /*
    if (Firebase.RTDB.setInt(&fbdo, "test/int", count)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    count++;
    */
    
    
    if (Firebase.RTDB.setString(&fbdo, "UltrasonicSensor/SensorData", TriggerOut)){
      Serial.println("Sending Data");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
      else {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbdo.errorReason());
      }

   if (Firebase.RTDB.setInt(&fbdo, "UltrasonicSensor/ActualData", IRState)){
      Serial.println("Sending Data");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
      else {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbdo.errorReason());
      }
    // Write an Float number on the database path test/float
    /*
    if (Firebase.RTDB.setFloat(&fbdo, "test/float", 0.01 + random(0,100))){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }*/
  }
}

