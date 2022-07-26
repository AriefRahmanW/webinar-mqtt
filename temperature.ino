#if ESP32
  #include <WiFi.h>
#elif ESP8266
  #include <ESP8266WiFi.h>
#endif

#include <WiFiClient.h>
#include <PubSubClient.h>

const char *ssid = "kebaratan"; 
const char *password = "Wifinyalemot";

#define MQTT_BROKER "broker.hivemq.com"
#define MQTT_PORT 1883

WiFiClient espClient;
PubSubClient client(espClient);

// sending data every 2 sec
const int period = 2000;
unsigned long lastTime = 0;

void setup() {
  Serial.begin(9600);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Max address: ");
  Serial.println(WiFi.macAddress());
  Serial.println("Connected to the WiFi network");
  Serial.println("-----------------------");

  client.setServer(MQTT_BROKER, MQTT_PORT);

  reconnect();
}

void loop() {
  client.loop();

  unsigned long currentMilis = millis();

  if(currentMilis - lastTime >= period){

    String payload = String(random(1, 100));
    
    bool isSuccess = client.publish("webinar123/temperature", payload.c_str());

    if(!isSuccess){
      reconnect();
    }

    Serial.print("payload: ");
    Serial.print(payload);
    Serial.print(", success: ");
    Serial.println(isSuccess);
    
    lastTime = currentMilis;
  }
}

void reconnect(){
  while (!client.connected()) {
      String clientId = String(WiFi.macAddress());
      Serial.printf("The client %s connects to the public mqtt broker\n", clientId.c_str());
      if (client.connect(clientId.c_str())) {
          Serial.println("Connected to broker");
      } else {
          Serial.print("failed with state ");
          Serial.print(client.state());
          delay(2000);
      }
  }
}
