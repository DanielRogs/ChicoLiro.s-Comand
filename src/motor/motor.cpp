#include <Arduino.h>

// Pinos do driver de motor L298N
const int motorPin1 = 5; // Pino de controle do motor 1 no driver L298N
const int motorPin2 = 18; // Pino de controle do motor 2 no driver L298N
const int enablePin = 19; // Pino de habilitação PWM

// Ajustar Pino do sensor de velocidade (encoder)
const int sensorPin = 26; // Pino ligado ao pino de sinal do encoder

// Variáveis de controle de velocidade e RPM
int rpm;
volatile byte pulsos;
unsigned long timeold;
unsigned int pulsos_por_volta = 13; // Ajuste de acordo com seu disco encoder

void ligarMotor(int velocidade) {
  analogWrite(enablePin, velocidade); // Define a velocidade do motor (0-255)
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, LOW);
}

void desligarMotor() {
  analogWrite(enablePin, 0); // Desativa o motor
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
}

void contador() {
  pulsos++;
}
