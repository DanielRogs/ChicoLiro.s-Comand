/*

  Título: Código de PID da equipe de Software.
  Autores: Daniel Rodrigues, Leandro Almeida e Luiz Amaral.
  Descrição: Código para integrar todo o sistema controlador do carro.
  Data de criação: 13/06/2024

*/

#include <Arduino.h>

// Pinos dos sensores infravermelhos
#define pinSensorMeio 15
#define pinSensorEsq 2
#define pinSensorDir 4

// Pinos dos motores
// MOTOR A
#define IN1 9 // CORRIGIR
#define IN2 8 // CORRIGIR
#define EN_A 10 // CORRIGIR
// MOTOR B
#define IN3 5 // CORRIGIR
#define IN4 4 // CORRIGIR
#define EN_B 3 // CORRIGIR

// Pinos dos encoders
const byte MOTOR_A = 15; // CORRIGIR
const byte MOTOR_B = 3; // CORRIGIR

// Variáveis para contagem de pulsos
volatile int counter_A = 0;
volatile int counter_B = 0;
unsigned long previousMillis = 0;
const unsigned long interval = 1000;
const float diskslots = 20.00;
const float wheeldiameter = 6.50;

// Parâmetros do PID
float Kp = 2.0;
float Ki = 5.0;
float Kd = 1.0;
float error = 0, previous_error = 0, integral = 0, derivative = 0;
float baseSpeed = 50;
float correction = 0;

// Função para calcular RPM e velocidade
void calculateSpeed() {
    float rotation_A = ((counter_A / diskslots) * 60.00) / 2;
    float rotation_B = ((counter_B / diskslots) * 60.00) / 2;

    counter_A = 0;
    counter_B = 0;
}

// Função de interrupção para contar pulsos do motor A
void ISR_count1() {
    counter_A++;
}

// Função de interrupção para contar pulsos do motor B
void ISR_count2() {
    counter_B++;
}

void setup() {
    Serial.begin(9600);

    // Configuração dos pinos dos sensores
    pinMode(pinSensorMeio, INPUT);
    pinMode(pinSensorEsq, INPUT);
    pinMode(pinSensorDir, INPUT);

    // Configuração dos pinos dos motores
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    pinMode(EN_A, OUTPUT);
    pinMode(EN_B, OUTPUT);

    // Inicializar os motores
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    analogWrite(EN_A, 0);
    analogWrite(EN_B, 0);

    // Configuração das interrupções dos encoders
    attachInterrupt(digitalPinToInterrupt(MOTOR_A), ISR_count1, RISING);
    attachInterrupt(digitalPinToInterrupt(MOTOR_B), ISR_count2, RISING);
}

void loop() {
    // Ler sensores
    bool sensorEsq = digitalRead(pinSensorEsq);
    bool sensorMeio = digitalRead(pinSensorMeio);
    bool sensorDir = digitalRead(pinSensorDir);

    // Controle PID
    if (sensorMeio) {
        error = 0;
    } else if (sensorEsq) {
        error = -1;
    } else if (sensorDir) {
        error = 1;
    }

    integral = integral + error;
    derivative = error - previous_error;
    correction = Kp * error + Ki * integral + Kd * derivative;
    previous_error = error;

    // Calcular velocidade dos motores
    int speedA = baseSpeed + correction;
    int speedB = baseSpeed - correction;

    // Limitar a velocidade dos motores
    speedA = constrain(speedA, 0, 100);
    speedB = constrain(speedB, 0, 100);

    // Definir direção e velocidade dos motores
    if (speedA >= 0) {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
    } else {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        speedA = -speedA;
    }
    if (speedB >= 0) {
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
    } else {
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
        speedB = -speedB;
    }

    analogWrite(EN_A, speedA);
    analogWrite(EN_B, speedB);

    // Calcular velocidade a cada segundo
    if (millis() - previousMillis >= interval) {
        calculateSpeed();
        previousMillis = millis();
    }
}
