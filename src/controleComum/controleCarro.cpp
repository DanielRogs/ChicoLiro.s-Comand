#include <Arduino.h>

// Pinos dos sensores infravermelhos
#define pinSensorMeio 15
#define pinSensorEsq 14
#define pinSensorDir 12

// Pinos dos motores
#define IN1 9
#define IN2 8
#define EN_A 10
#define IN3 5
#define IN4 4
#define EN_B 3

// Pinos dos encoders
const byte MOTOR_A = 15;
const byte MOTOR_B = 3;

// Variáveis para contagem de pulsos
volatile int counter_A = 0;
volatile int counter_B = 0;
unsigned long previousMillis = 0;
const unsigned long interval = 1000;
const float diskslots = 20.00;
const float wheeldiameter = 6.50;

// Velocidade base dos motores
const int baseSpeed = 100;
const int turnSpeed = 50;

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

    // Controle simples baseado nos sensores
    if (sensorMeio) {
        // Seguir em frente
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        analogWrite(EN_A, baseSpeed);

        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
        analogWrite(EN_B, baseSpeed);
    } else if (sensorEsq) {
        // Virar à esquerda
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        analogWrite(EN_A, turnSpeed);

        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
        analogWrite(EN_B, baseSpeed);
    } else if (sensorDir) {
        // Virar à direita
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        analogWrite(EN_A, baseSpeed);

        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
        analogWrite(EN_B, turnSpeed);
    } else {
        // Parar se nenhum sensor detectar a linha
        analogWrite(EN_A, 0);
        analogWrite(EN_B, 0);
    }

    // Calcular velocidade a cada segundo
    if (millis() - previousMillis >= interval) {
        calculateSpeed();
        previousMillis = millis();
    }
}
