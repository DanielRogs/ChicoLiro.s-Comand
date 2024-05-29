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
  // Ler todos os sensores uma única vez
  bool sensorMeio = digitalRead(pinSensorMeio);
  bool sensorEsq = digitalRead(pinSensorEsq);
  bool sensorDir = digitalRead(pinSensorDir);

  if (sensorMeio && !sensorEsq && !sensorDir) {
    Serial.println("Reta");
  }
  else if (sensorEsq && !sensorDir) {
    Serial.println("Virar a Esquerda");
  }
  else if (!sensorEsq && sensorDir) {
    Serial.println("Virar a Direita");
  }
  else if (sensorMeio && sensorEsq && sensorDir) {
    Serial.println("Fim do Percurso");
  }
  else {
    Serial.println("Não detectado");
  }


  // int valorAnalogico = analogRead(pinSensor);
  // Serial.print("Valor analógico:");
  // Serial.println(valorAnalogico);
  // delay(200);
}