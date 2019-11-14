/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */

#include <stdlib.h>

#include <WiFi.h>
#include <PubSubClient.h>

#include <Wire.h>
#include <SPI.h>

#include "config.h"


/*************************** custom config *****************************************/


/* identifier for MQTT payload + for id broker clientid */
const char *my_id = "_one";

/* relay */

#define RELAY_PIN 15
 
/* broker client id ( will be concatenated with MQTT identifier */
const char* broker_id_generic  = "Relay";
char broker_id[20];


// each device subscribes get/# but it will send temp when  get/all or get/<id>

const char* topic_subscribe = "house/relay/push";


void closeCircuit() {
  digitalWrite(RELAY_PIN,HIGH);
}

void openCircuit() {
    digitalWrite(RELAY_PIN,LOW);
}

void relaypressButton() {
  closeCircuit();
  delay(3000);
  openCircuit();
}
void setuprelay() {
  pinMode(RELAY_PIN,OUTPUT);
  openCircuit();
}

WiFiClient wificlient;
PubSubClient client(wificlient);


void check_and_connect_wifi() {
  if (WiFi.status() != WL_CONNECTED) {
       Serial.print("Attempting to connect to wifi.");
     WiFi.begin(ssid, password);
     while (  WiFi.status() != WL_CONNECTED) {
        Serial.print("Attempting to connect to WPA SSID: ");
        Serial.print(ssid);
        Serial.print(" ");
        Serial.println(password);
        Serial.println(WiFi.status());
      // Connect to WPA/WPA2 network
        delay(500);
      }
   Serial.println("");
   Serial.println("WiFi connected");
   Serial.println("IP address: ");
   Serial.println(WiFi.localIP());     
  }
}


void reconnect_mqtt() {
  // Loop until we're reconnected
  client.disconnect();
  delay(500);
  while (!client.connected()) {
    Serial.println("Connecting to broker ...");
    // Attempt to connect (clientId, username, password)
    if ( client.connect(broker_id,broker_usr,broker_pwd, NULL, 0, 0, NULL, false)) {
      Serial.println( "[DONE]" );
      if (!client.subscribe(topic_subscribe,1)) {
        Serial.println("not subscribed");
      } else {
        Serial.println("subscribed");
      }
      
    } else {
      Serial.print( "[FAILED] [ rc = " );
      Serial.print( client.state() );
      Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
      delay( 5000 );
    }
  }
}


void setup()
{
    Serial.begin(115200);
    // We start by connecting to a WiFi network
    check_and_connect_wifi();
    strcpy(broker_id,broker_id_generic);
    strcat(broker_id,my_id);
    client.setServer( broker_host,broker_port);
    client.setCallback(callback);

    setuprelay();
}


void loop()
{

  delay(100);
  check_and_connect_wifi();
  delay(100);
  if ( !client.connected() ) {
    Serial.println("reconnect");
    reconnect_mqtt();
    Serial.println("reconnected");    
  }
    
  client.loop();  
}

void callback(char* topic, uint8_t* payload, unsigned int length) {
  char subtopic[20];
  char mypayload[20];
  int len;

  for (int i=0;i<length;i++) {
    mypayload[i] = (char)payload[i];
  }
  mypayload[length]=0;

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("]");
  // topic should be "house/relay/push/" and payload ignored 
  relaypressButton();

}
