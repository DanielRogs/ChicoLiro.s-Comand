#include <Arduino.h>
#define pinSensorMeio 15 // Deve estar de acordo com o pino de output
#define pinSensorEsq 14 // Sensor da Esquerda
#define pinSensorDir 12 // Sensor da direita


void setup() {
  pinMode(pinSensorMeio, INPUT);
  pinMode(pinSensorEsq, INPUT);
  pinMode(pinSensorDir, INPUT);
  Serial.begin(9600); // Configuração da taxa de comunicação em bits por segundo (baud rate)
}

void loop() {
  // Se detectar no do meio e os da esquerda e direita não, segue reto
  if (digitalRead(pinSensorMeio) == HIGH && digitalRead(pinSensorEsq) == LOW && digitalRead(pinSensorDir) == LOW) {  
    Serial.println("Reta");
  }
  // Se detectar na esquerda e não na direita, vire a esquerda
  else if (digitalRead(digitalRead(pinSensorEsq) == HIGH && digitalRead(pinSensorDir) == LOW)) {
    Serial.println("Virar a Esquerda");
  }
  // Se detectar na direita e não na esquerda, vire a direita
  else if (digitalRead(digitalRead(pinSensorEsq) == LOW && digitalRead(pinSensorDir) == HIGH)) {
    Serial.println("Virar a Direita");
  }
  // Caso todos os pinos estejam ativos, fim do percurso
  else if (digitalRead(pinSensorMeio) == HIGH && digitalRead(pinSensorEsq) == HIGH && digitalRead(pinSensorDir) == HIGH) {
    Serial.println("Fim do Percurso");
  }
  // Linha não detectada
  else{
    Serial.println("Não detectado");
  }
  delay(200);


  // int valorAnalogico = analogRead(pinSensor);
  // Serial.print("Valor analógico:");
  // Serial.println(valorAnalogico);
  // delay(200);
}