#include <SPI.h>
#include <UIPEthernet.h>
#include <utility/logging.h>
#include <PubSubClient.h>
#include <Ultrasonic.h>
#include <LiquidCrystal.h>

byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xF1, 0x63 };
void callback(char* topic, byte* payload, unsigned int length) {

}
EthernetClient ethClient;
PubSubClient client("m14.cloudmqtt.com", 12719, callback, ethClient);
long lastReconnectAttempt = 0;
boolean reconnect() {
  Serial.println("reconectando...");
  if (client.connect("arduinoClient", "arduino", "123")) {
    Serial.println("conectado");
    // client.publish("vaga/1","hello world");
    client.subscribe("vaga/1");
  }
  return client.connected();
}
Ultrasonic ultrasonic(A1, A0);

const int rs = 9, en = 8, d4 = 6, d5 = 5, d6 = 4, d7 = 3;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int estadoVaga = 1;

void setup() {
  Serial.begin(9600);
  Ethernet.begin(mac);
  lcd.begin(16, 2);
  lastReconnectAttempt = 0;
}

void loop() {
  int distancia = ultrasonic.distanceRead();

  if (!client.connected()) {
    estadoVaga = 1;
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
    delay(500);

    if (distancia < 25) {
      lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 0);
      lcd.print("VAGA OCUPADA");
      if (estadoVaga == 1) {
        Serial.println(0);
        client.publish("vaga/1", "0");
        estadoVaga = 0;
      }
    }
    else {
      lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 0);
      lcd.print("VAGA LIBERADA");
      if (estadoVaga == 0) {
        Serial.println(1);
        client.publish("vaga/1", "1");
        estadoVaga = 1;
      }
    }
  }
}
