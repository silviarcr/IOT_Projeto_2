#include <SPI.h>
#include <UIPEthernet.h>
#include <utility/logging.h>
#include <PubSubClient.h>
#include <LiquidCrystal.h>

byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xF1, 0x77 };
int vagas_disponiveis = 0;
int estadoVagas = 0;
//MENU LCD

const int rs = 9, en = 8, d4 = 6, d5 = 5, d6 = 4, d7 = 3;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void callback(char* topic, byte* payload, unsigned int length) {
  int c = atoi(payload);
  Serial.println(topic);
  Serial.println(c);
  if (c == 0) {
    vagas_disponiveis--;
    
  }
  if (c == 1) {
    vagas_disponiveis++;
  }

  

}

EthernetClient ethClient;
PubSubClient client("m14.cloudmqtt.com", 12719, callback, ethClient);

long lastReconnectAttempt = 0;

boolean reconnect() {
  Serial.println("reconectando...");
  if (client.connect("projeto2", "arduino", "123")) {
    Serial.println("conectado");
    client.subscribe("vaga/#");
  }
  return client.connected();
}

void setup()
{
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.setCursor(0, 1);
  lcd.print("oi");
  
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
      lastReconnectAttempt = now;

      if (reconnect()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {
    client.loop();
    //Serial.println(vagas_disponiveis);
    

    if (estadoVagas != vagas_disponiveis) {
      estadoVagas = vagas_disponiveis;
      
      lcd.setCursor(0, 1);
      lcd.print("              ");
      
      lcd.setCursor(0, 1);
      lcd.print(vagas_disponiveis);
    }

    //delay(300);
  }

}
