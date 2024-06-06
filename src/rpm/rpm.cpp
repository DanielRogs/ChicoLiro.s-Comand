#include <Arduino.h>

const byte MOTOR_A = 15; // Pino de interrupção do motor A
// const byte MOTOR_B = 3; // Pino de interrupção do motor B

// Variáveis para contar os pulsos lidos
volatile int counter_A, counter_B = 0;
unsigned long previousMillis = 0;
const unsigned long interval = 1000; // Intervalo de 1 segundo

// Float para o número de furos presente no disco de encoder
const float diskslots = 20.00;

// Float com o diâmetro da roda em centímetros
const float wheeldiameter = 6.50;

// Função para calcular RPM e velocidade
void calculateSpeed() {
  Serial.print("RPM do motor A: ");
  float rotation_A = ((counter_A / diskslots) * 60.00)/2;
  Serial.print(rotation_A);
  Serial.print(" RPM - ");

  Serial.print("Velocidade do motor A: ");
  float speed_A = (rotation_A * wheeldiameter * 3.14) / 60;
  Serial.print(speed_A);
  Serial.println(" cm/s");

  // Serial.print("RPM do motor B: ");
  // float rotation_B = (counter_B / diskslots) * 60.00;
  // Serial.print(rotation_B);
  // Serial.print(" RPM - ");

  // Serial.print("Velocidade do motor B: ");
  // float speed_B = (rotation_B * wheeldiameter * 3.14) / 60;
  // Serial.println(" cm/s");

  counter_A = 0;
  counter_B = 0;
}

// Função de interrupção para contar pulsos do motor A
void ISR_count1() {
  counter_A++;
}

// Função de interrupção para contar pulsos do motor B
// void ISR_count2() {
//   counter_B++;
// }

void setup() {
  Serial.begin(9600);

  attachInterrupt(digitalPinToInterrupt(MOTOR_A), ISR_count1, RISING);
  // attachInterrupt(digitalPinToInterrupt(MOTOR_B), ISR_count2, RISING);
}

void loop() {
  if (millis() - previousMillis >= interval) {
    calculateSpeed();
    previousMillis = millis();
  }
}