#include <SPI.h>
#include <UIPEthernet.h>
#include <utility/logging.h>
#include <PubSubClient.h>

byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xF1, 0x77 };

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println(topic);
}

EthernetClient ethClient;
PubSubClient client("m14.cloudmqtt.com", 12719, callback, ethClient);

long lastReconnectAttempt = 0;

boolean reconnect() {
  Serial.println("reconectando...");
  if (client.connect("projeto2", "arduino", "123")) {
    Serial.println("conectado");
    // client.publish("vaga/1","hello world");
    client.subscribe("vaga/#");
  }
  return client.connected();
}

void setup()
{
  Serial.begin(9600);
  Serial.println("iniciando...");
  Ethernet.begin(mac);
  delay(1500);
  
  lastReconnectAttempt = 0;
}


void loop()
{
  if (!client.connected()) {
    long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      Serial.println("reconectando...");
      lastReconnectAttempt = now;
      
      if (reconnect()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {
    client.loop();
  }

}
