#include <SPI.h>
#include <Adafruit_NeoPixel.h>

#include <Ethernet2.h>
#include <PubSubClient.h>

#define DEBUG
#include "Debug.h"
#include "Topic_Map.h"

#define NEO_PIXEL_PIN 8
#define ENET_SS_PIN 10
#define MQTT_PORT 1883

IPAddress device_ip_addr;
IPAddress broker_ip_addr(192, 168, 1, 2);
uint8_t mac[6];

EthernetClient ethClient;
PubSubClient mqttClient(ethClient);

const char device_name[256] = "lightX"; // change X to be your number
const char * mqtt_topic_f_string = "%s/%s";
char temp_string[256];

void mqtt_callback(char * topic, byte * payload, unsigned int length);
void mqtt_reconnect();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while(!Serial){ delay(1); }
  DEBUG_PRINTLN("Started Serial Monitor");

  DEBUG_PRINTLN("Setting up the Mqtt Client.");
  mqttClient.setServer(broker_ip_addr, MQTT_PORT);
  mqttClient.setCallback(mqtt_callback);
  
  DEBUG_PRINTLN("Setting up the Ethernet Hardware, trying to setup via DHCP");
 
  w5500.getMACAddress((uint8_t*)&mac);
  
  DEBUG_PRINT("W5500 MAC: "); 
  DEBUG_PRINT(mac[0]); DEBUG_PRINT(":"); DEBUG_PRINT(mac[1]); DEBUG_PRINT(":"); 
  DEBUG_PRINT(mac[2]); DEBUG_PRINT(":"); DEBUG_PRINT(mac[3]); DEBUG_PRINT(":"); 
  DEBUG_PRINT(mac[4]); DEBUG_PRINT(":"); DEBUG_PRINTLN(mac[5]);

  while(!Ethernet.begin((uint8_t*)&mac)){
    DEBUG_PRINTLN("Could not setup DHCP, trying again");
    delay(250);
  }

  device_ip_addr = Ethernet.localIP();
  DEBUG_PRINT("DHCP SuccessfuL: IP: "); DEBUG_PRINTLN(device_ip_addr);

}

void loop() {
  // put your main code here, to run repeatedly:
    if(!mqttClient.connected()){
      mqtt_reconnect();
    }
    mqttClient.loop();
}

void mqtt_callback(char * topic, byte * payload, unsigned int length){

}

void mqtt_reconnect(){
  while(!mqttClient.connected()){
    DEBUG_PRINTLN("Attempting an MQTT Connection");

    if(mqttClient.connect(device_name)){
      DEBUG_PRINTLN("MQTT Connected to Broker");
      DEBUG_PRINT("Broker Addr: "); DEBUG_PRINTLN(broker_ip_addr);

      DEBUG_PRINTLN("Subbing to topics");

      for(int i = 0; i < 6; i++){
        sprintf(temp_string, mqtt_topic_f_string, device_name, sub_topics[i]);
        
        DEBUG_PRINT("Subbing to topic: "); DEBUG_PRINTLN(temp_string);
        
        mqttClient.subscribe(temp_string);
        memset(temp_string, 0, sizeof(temp_string));
      }
    } else {
      DEBUG_PRINT("Failed, rc= "); DEBUG_PRINTLN(mqttClient.state());
      delay(5000);
    }


  }
}
