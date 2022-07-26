#if ESP32
  #include <WiFi.h>
#elif ESP8266
  #include <ESP8266WiFi.h>
#endif

#include <WiFiClient.h>
#include <PubSubClient.h>

#include <SPI.h>
#include <MFRC522.h>

const char *ssid = "kebaratan"; 
const char *password = "Wifinyalemot";

#define MQTT_BROKER "broker.hivemq.com"
#define MQTT_PORT 1883

#define SS_PIN D2
#define RST_PIN D1


MFRC522 rfid(SS_PIN, RST_PIN);

WiFiClient espClient;
PubSubClient client(espClient);


void setup() {
  Serial.begin(9600);

  // rfid
  SPI.begin(); // init SPI bus
  rfid.PCD_Init(); // init MFRC522 
  
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

  if (rfid.PICC_IsNewCardPresent()) { // new tag is available
    if (rfid.PICC_ReadCardSerial()) { // NUID has been readed
      String rfidTag = readRFID();
      
      Serial.print("Tag: ");
      Serial.println(rfidTag);
      Serial.println("-----------------------");
      
      client.publish("webinar123/rfid", rfidTag.c_str());
    }
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


String readRFID(){
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);

  String tag = "";
  for (int i = 0; i < rfid.uid.size; i++) {
    tag += rfid.uid.uidByte[i] < 0x10 ? "0" : ":";
    tag += String(rfid.uid.uidByte[i], HEX);
  }

  rfid.PICC_HaltA(); // halt PICC
  rfid.PCD_StopCrypto1(); // stop encryption on PCD

  return tag;
}
