//created by kdragon

#include <ESP8266WiFi.h>
#include <PubSubClient.h>



//EDIT THESE LINES TO MATCH YOUR SETUP
#define MQTT_SERVER "192.168.1.100" //for raspberrypi change the ip accordingly , see github pubsubclient          //   "ws://192.168.43.175:9001/mqtt"; 
const char* ssid = "sadlec_wifi";
const char* password = "password";

//LED on ESP8266 GPIO2
const int lightPin = 2;
const int tlight = 5;
const int fan = 4;

char* lightTopic = "killer/l";


WiFiClient wifiClient;
void callback(char* topic, byte* payload, unsigned int length);

PubSubClient client(MQTT_SERVER, 1883, callback, wifiClient);   //for raspberrypi change the ip/port accordingly 

void setup() {
  //initialize the light as an output and set to LOW (off)
  pinMode(lightPin, OUTPUT);
  pinMode(tlight, OUTPUT);
  pinMode(fan, OUTPUT);
  digitalWrite(lightPin, LOW);
  digitalWrite(tlight, LOW);

  client.setCallback(callback);
  //start the serial line for debugging
  Serial.begin(115200);
  delay(100);


  //start wifi subsystem
  WiFi.begin(ssid, password);
  //attempt to connect to the WIFI network and then connect to the MQTT server
  reconnect();

  //wait a bit before starting the main loop
      delay(2000);
}



void loop(){

  //reconnect if connection is lost
  if (!client.connected() && WiFi.status() == 3) {reconnect();}

  //maintain MQTT connection
  client.loop();

  //MUST delay to allow ESP8266 WIFI functions to run
  delay(10); 
}


void callback(char* topic, byte* payload, unsigned int length) {

    client.setCallback(callback);
  //convert topic to string to make it easier to work with
  String topicStr = topic; 

  //Print out some debugging info
  Serial.println("Callback update.");
  Serial.print("Topic: ");
  Serial.println(topicStr);

  //turn the light on if the payload is '1' and publish to the MQTT server a confirmation message
  if(payload[0] == '0'){
    digitalWrite(lightPin, HIGH);
    digitalWrite(tlight, LOW);
    client.publish("killer/l", "Light Off");

  }

  //turn the light off if the payload is '0' and publish to the MQTT server a confirmation message
  else if (payload[0] == '1'){
    digitalWrite(lightPin, LOW);
    digitalWrite(tlight, HIGH);
    client.publish("killer/l", "Light On");
  }

else if (payload[0] == '2'){
    digitalWrite(lightPin, LOW);
    digitalWrite(fan, HIGH);
    client.publish("killer/l", "Fan On");
  }

  else if (payload[0] == '3'){
    digitalWrite(lightPin, HIGH);
    digitalWrite(fan, LOW);
    client.publish("killer/l", "Fan Off");
  }

}




void reconnect() {

  //attempt to connect to the wifi if connection is lost
  if(WiFi.status() != WL_CONNECTED){
    //debug printing
    Serial.print("Connecting to ");
    Serial.println(ssid);

    //loop while we wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    //print out some more debug once connected
    Serial.println("");
    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }

  //make sure we are connected to WIFI before attemping to reconnect to MQTT
  if(WiFi.status() == WL_CONNECTED){
  // Loop until we're reconnected to the MQTT server
    while (!client.connected()) {
      Serial.print("Attempting MQTT connection...");
      
      String clientName;
      clientName = "clientidhaimeraye";

      //if connected, subscribe to the topic(s) we want to be notified about
      if (client.connect((char*) clientName.c_str())) {
        Serial.print("\tMTQQ Connected");
        client.subscribe(lightTopic);
      }

      //otherwise print failed for debugging
      else{Serial.println("\tFailed."); abort();}
    }
  }
}

