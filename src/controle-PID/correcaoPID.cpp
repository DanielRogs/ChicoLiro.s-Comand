/*

  Título: Código de PID da equipe de Software.
  Autores: Daniel Rodrigues, Leandro Almeida e Luiz Amaral.
  Descrição: Código para integrar todo o sistema controlador do carro.
  Data de criação: 11/06/2024

  ATENÇÃO, CÓDIGO NÃO TESTADO!
*/

#include <Arduino.h>

// Definição de pinos dos sensores, encoders e motores
const int pinSensorEsq = 15;
const int pinSensorMeio = 2;
const int pinSensorDir = 4; 

const int pinEncoderEsq = 5;  // AJUSTAR O PINO
const int pinEncoderDir = 6;  // AJUSTAR O PINO

// Motor A
#define IN1 9 // AJUSTAR O PINO
#define IN2 8 // AJUSTAR O PINO
#define EN_A 10 // AJUSTAR O PINO

// Motor B
#define IN3 5 // AJUSTAR O PINO
#define IN4 4 // AJUSTAR O PINO
#define EN_B 3  // AJUSTAR O PINO

// Variáveis para leitura dos sensores
int sensorEsq;
int sensorMeio;
int sensorDir;

volatile int encoderEsq = 0;
volatile int encoderDir = 0;

// Variáveis para controle PID
float erro;
float proporcional;
float integral;
float derivativo;
float erroAnterior = 0;
float Kp = 1.0;  // Ganho Proporcional
float Ki = 0.5;  // Ganho Integral
float Kd = 0.1;  // Ganho Derivativo
float PID;

// Funções
int lerSensor(int sensorPin) {
  return analogRead(sensorPin);
}

float calcularErro(int sensorEsq, int sensorMeio, int sensorDir) {
  const int leituraLinha = 3000; // Necessário calibragem
  
  if (sensorEsq < leituraLinha && sensorMeio >= leituraLinha && sensorDir >= leituraLinha) {
    return -2; // Vira à esquerda
  } else if (sensorEsq < leituraLinha && sensorMeio < leituraLinha && sensorDir >= leituraLinha) {
    return -1; // Esquerda-meio
  } else if (sensorEsq >= leituraLinha && sensorMeio < leituraLinha && sensorDir >= leituraLinha) {
    return 0.0;  // Centralizado
  } else if (sensorEsq >= leituraLinha && sensorMeio >= leituraLinha && sensorDir < leituraLinha) {
    return 1;  // Direita-meio
  } else if (sensorEsq >= leituraLinha && sensorMeio >= leituraLinha && sensorDir < leituraLinha) {
    return 2;  // Vira à direita
  } else {
    return 0.0;  // Caso padrão (sem linha detectada ou linha detectada em todos os sensores)
  }
}

void controlarMotor(int PID) {
  int velocidadeBase = 100;  // Velocidade base dos motores (ajustável)
  int velocidadeMax = 255;   // Valor máximo para PWM (assumindo que 255 é o máximo)

  // Cálculo da velocidade dos motores ajustando pelo PID
  int velocidadeEsq = velocidadeBase + PID;
  int velocidadeDir = velocidadeBase - PID;

  // Garantir que a velocidade esteja dentro do limite seguro
  velocidadeEsq = constrain(velocidadeEsq, 0, velocidadeMax);
  velocidadeDir = constrain(velocidadeDir, 0, velocidadeMax);

  analogWrite(EN_A, velocidadeEsq);
  analogWrite(EN_B, velocidadeDir);

  // Ajustar a direção dos motores
  if (velocidadeEsq > velocidadeDir) {
    // Virar à esquerda
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else if (velocidadeDir > velocidadeEsq) {
    // Virar à direita
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  } else {
    // Andar para frente
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  }
}

void controlePID() {
  sensorEsq = lerSensor(pinSensorEsq);
  sensorMeio = lerSensor(pinSensorMeio);
  sensorDir = lerSensor(pinSensorDir);

  erro = calcularErro(sensorEsq, sensorMeio, sensorDir);

  if (erro == 0){
    integral = 0;
  }

  proporcional = erro;
  integral += erro;

  // ajustar condicional conforme limite superior e inferior do PWM
  if (integral > 120){ 
    integral = 120;
  } else if (integral < -120) {
    integral = -120;
  }

  derivativo = erro - erroAnterior;

  PID = (Kp * proporcional) + (Ki * integral) + (Kd * derivativo);
  erroAnterior = erro;

  controlarMotor(PID);
}

void encoderEsqISR() {
  encoderEsq++;
}

void encoderDirISR() {
  encoderDir++;
}

void setup() {
  Serial.begin(9600);

  pinMode(pinSensorEsq, INPUT);
  pinMode(pinSensorMeio, INPUT);
  pinMode(pinSensorDir, INPUT);
  pinMode(pinEncoderEsq, INPUT);
  pinMode(pinEncoderDir, INPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(EN_A, OUTPUT);
  pinMode(EN_B, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(pinEncoderEsq), encoderEsqISR, RISING);
  attachInterrupt(digitalPinToInterrupt(pinEncoderDir), encoderDirISR, RISING);

  // Inicializar motores em estado parado
  analogWrite(EN_A, 0);
  analogWrite(EN_B, 0);
}

void loop() {
  controlePID();
}
