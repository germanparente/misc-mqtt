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

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

 #include <Adafruit_GFX.h>
 #include <Adafruit_SSD1306.h>

#include "config.h"

// constants, variables period

unsigned long startMillis=0,currentMillis=0;

/*************************** custom config *****************************************/

/* period to publish data */
#define PERIOD 300000 

/* identifier for MQTT payload + for id broker clientid */
const char *mytemphumid = "five";
 

/* broker client id ( will be concatenated with MQTT identifier */
const char* broker_id_generic  = "TempHumid";

char broker_id[20];


// each device subscribes get/# but it will send temp when  get/all or get/<id>

const char* topic_subscribe = "house/temphumid/request/#";

// BME + Wire
#define MYSDAPIN 5
#define MYSCLPIN 4
Adafruit_BME280 bme;

// SSD1306
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels


#define NODEMCU
// the other possibility is esp32 mini

#ifdef NODEMCU
#define OLED_MOSI  23
#define OLED_CLK   18
#define OLED_DC    19
#define OLED_CS    2
#define OLED_RESET 13
#else
// esp32 MINI
#define OLED_MOSI  15 // 9
#define OLED_CLK   0  // 10
#define OLED_DC    14 // 11
#define OLED_CS    2
#define OLED_RESET 13
#endif


// Declaration for SSD1306 display connected using software SPI (default case):
 Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
   OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

void setup_ssd1306() {
   // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
   if(!display.begin(SSD1306_SWITCHCAPVCC)) {
     Serial.println(F("SSD1306 allocation failed"));
     for(;;); // Don't proceed, loop forever
   } 
   display.display();
   // Clear the buffer
   delay(500);
   display.clearDisplay();
   display.display();
}

void displayTemperatureAndHumid() {
  display.clearDisplay();
  display.display();
  delay(200);
  display.setTextColor(WHITE);
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setCursor(10,0);             // Start at top-left corner
  display.println(F("Temperature: "));
  display.setTextSize(3);
  display.setCursor(10,10);
  display.println(bme.readTemperature());  
  display.setCursor(10,32); 
  display.setTextSize(1);
  display.println(F("Humidity: "));
  display.setTextSize(3);
  display.setCursor(10,42);
  display.println(bme.readHumidity());
  display.display();
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


void setup_bme() {

    boolean status;
    Wire.begin(MYSDAPIN,MYSCLPIN);
    status = bme.begin(0x76,&Wire);  
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1);
    }
}

void publishTempHumid() {
    char buff[10];
    char payload[25];
    char topic[30];
    float x = bme.readTemperature();
    Serial.print(x);
    Serial.println(" *C");
    dtostrf(x,4,2,buff);
    strcpy(payload,buff);
    x = bme.readHumidity();
    Serial.print(x);
    Serial.println(" %");
    dtostrf(x,4,2,buff);
    strcat(payload,"-");
    strcat(payload,buff);
    strcpy(topic,"house/temphumid/publish/");
    strcat(topic,mytemphumid);
    client.publish(topic,payload);
}


void setup()
{
    Serial.begin(115200);
    setup_bme();
    setup_ssd1306();
    displayTemperatureAndHumid();
    // We start by connecting to a WiFi network
    check_and_connect_wifi();
    strcpy(broker_id,broker_id_generic);
    strcat(broker_id,mytemphumid);
    client.setServer( broker_host,broker_port);
    client.setCallback(callback);
}

boolean checkPeriod() {
  boolean result = false;
  currentMillis = millis();
  if ( currentMillis - startMillis >= PERIOD ) {
     startMillis = currentMillis;
     result = true;
  }
  return result;
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

  if (checkPeriod()) {
     publishTempHumid();
     displayTemperatureAndHumid();
  }

  
}

void callback(char* topic, uint8_t* payload, unsigned int length) {
  char subtopic[20];
  char mypayload[20];
  int len;


  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");
  // topic should be "house/temphumid/request/xxx"
  len = strlen("house/temphumid/request/");
  if (strncmp(topic,"house/temphumid/request/",len)==0) {
    strcpy(subtopic,&topic[len]);
    Serial.print("Subtopic is [");
    Serial.print(subtopic);
    Serial.print("]\n");
  }

   if (strcmp(subtopic,mytemphumid)==0 || strcmp(subtopic,"all")==0) {
     publishTempHumid();
     displayTemperatureAndHumid();
   } 
}
