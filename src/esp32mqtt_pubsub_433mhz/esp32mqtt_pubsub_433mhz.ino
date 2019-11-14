/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */

#include <WiFi.h>
#include <PubSubClient.h>

#include "config.h"

const char* topic_subscribe = "house/plug/#";
const char *broker_id = "Plugs";


#define DATA_PIN 2

#define PLUG_ONE_ON 1807141117
#define PLUG_ONE_OFF 1807140443
#define PLUG_TWO_ON 1807141435
#define PLUG_TWO_OFF 1807140897
#define PLUG_THREE_ON 1807141567
#define PLUG_THREE_OFF 1807140449
#define PLUG_ALL_ON 1807141334
#define PLUG_ALL_OFF 1807141555


void plugOneON() {
  sendCode(PLUG_ONE_ON);
}

void plugOneOFF() {
  sendCode(PLUG_ONE_OFF);
}

void plugTwoON() {
  sendCode(PLUG_TWO_ON);
}

void plugTwoOFF() {
  sendCode(PLUG_TWO_OFF);
}

void plugThreeON() {
  sendCode(PLUG_THREE_ON);
}

void plugThreeOFF() {
  sendCode(PLUG_THREE_OFF);
}

void plugAllON() {
  sendCode(PLUG_ALL_ON);
}

void plugAllOFF() {
  sendCode(PLUG_ALL_OFF);
}


void sendCode(unsigned long code) {
  for (int i=0;i<5;i++) {
    sendOneCode(code);
  }
}

void sendOneCode(unsigned long shiftedCode) {
 //Envois du code à la plug
 const unsigned int THIGH = 220, TSHORT = 350, TLONG=1400; // Temps des états
 digitalWrite(DATA_PIN, HIGH);
 delayMicroseconds(THIGH);
 digitalWrite(DATA_PIN, LOW); 
 delayMicroseconds(2675);
 for (int i = 0; i < 32; i++) {
  if (shiftedCode & 0x80000000L) {
     digitalWrite(DATA_PIN, HIGH);
     delayMicroseconds(THIGH);
     digitalWrite(DATA_PIN, LOW);
     delayMicroseconds(TLONG);
     digitalWrite(DATA_PIN, HIGH);
     delayMicroseconds(THIGH);
     digitalWrite(DATA_PIN, LOW); 
     delayMicroseconds(TSHORT);
   } else {
     digitalWrite(DATA_PIN, HIGH);
     delayMicroseconds(THIGH);
     digitalWrite(DATA_PIN, LOW);
     delayMicroseconds(TSHORT);
     digitalWrite(DATA_PIN, HIGH);
     delayMicroseconds(THIGH);
     digitalWrite(DATA_PIN, LOW);
     delayMicroseconds(TLONG);
   }
   shiftedCode <<= 1;
 }
 digitalWrite(DATA_PIN, HIGH);
 delayMicroseconds(THIGH);
 digitalWrite(DATA_PIN, LOW);
 delayMicroseconds(10600);
 digitalWrite(DATA_PIN, HIGH);
 delayMicroseconds(THIGH);
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
    delay(10);
    pinMode(DATA_PIN,OUTPUT);
    // We start by connecting to a WiFi network
    check_and_connect_wifi();
    client.setServer( broker_host,broker_port );
    client.setCallback(callback);    
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
  int i;


  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");
  // topic should be "house/plug/xxx"
  if (strncmp(topic,"house/plug/",11)==0) {
    strcpy(subtopic,&topic[11]);
    Serial.print("Subtopic is [");
    Serial.print(subtopic);
    Serial.print("]\n");
  }
  for (i=0;i<length && i<20;i++) {
    mypayload[i] = (char)payload[i];
  }
  mypayload[i]=0;
  Serial.print("my payload is [");
  Serial.print(mypayload);
  Serial.println("]\n");

   if (strcmp(subtopic,"one")==0) {
    if (strcmp(mypayload,"on")==0) {
       plugOneON();
    } else if (strcmp(mypayload,"off")==0) {
       plugOneOFF();
    }
  } else if (strcmp(subtopic,"two")==0) {
    if (strcmp(mypayload,"on")==0) {
       plugTwoON();
    } else if (strcmp(mypayload,"off")==0) {
       plugTwoOFF();
    }    
  } else if (strcmp(subtopic,"three")==0) {
    if (strcmp(mypayload,"on")==0) {
       plugThreeON();
    } else if (strcmp(mypayload,"off")==0) {
       plugThreeOFF();
    }    
  } else if (strcmp(subtopic,"all")==0) {
    if (strcmp(mypayload,"on")==0) {
       plugAllON();
    } else if (strcmp(mypayload,"off")==0) {
       plugAllOFF();
    }    
  }   
  
}
