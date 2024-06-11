/*

  Título: Código de PID da equipe de Software.
  Autores: Daniel Rodrigues, Leandro Almeida e Luiz Amaral.
  Descrição: Código para integrar todo o sistema controlador do carro.
  Data de criação: 11/06/2024

*/

#include <Arduino.h>

// Definição de pinos dos sensores, encoders e motores
const int pinSensorEsq = 15;
const int pinSensorMeio = 2;
const int pinSensorDir = 4;

const int pinEncoderEsq = 5;  // Defina os pinos corretos dos encoders
const int pinEncoderDir = 6;

const int pinMotorEsq = 9;    // Defina os pinos corretos dos motores
const int pinMotorDir = 10;

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
float Kp = 1.0;
float Ki = 0.5;
float Kd = 0.1;
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
  int velocidadeBase = 50;  // Velocidade base dos motores (ajustável)
  int velocidadeMax = 100;   // Valor máximo para PWM (assumindo que 255 é o máximo)

  // Cálculo da velocidade dos motores ajustando pelo PID
  int velocidadeEsq = velocidadeBase + PID;
  int velocidadeDir = velocidadeBase - PID;

  // Garantir que a velocidade esteja dentro do limite seguro
  velocidadeEsq = constrain(velocidadeEsq, 0, velocidadeMax);
  velocidadeDir = constrain(velocidadeDir, 0, velocidadeMax);

  analogWrite(pinMotorEsq, velocidadeEsq);
  analogWrite(pinMotorDir, velocidadeDir);
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

  PID = (Kp*proporcional) + (Ki*integral) + (Kd*derivativo);
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
  pinMode(pinMotorEsq, OUTPUT);
  pinMode(pinMotorDir, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(pinEncoderEsq), encoderEsqISR, RISING);
  attachInterrupt(digitalPinToInterrupt(pinEncoderDir), encoderDirISR, RISING);
}

void loop() {
    controlePID();
}
#include <Arduino.h>

// Definição de pinos dos sensores, encoders e motores
const int pinSensorEsq = 15;
const int pinSensorMeio = 2;
const int pinSensorDir = 4;

const int pinEncoderEsq = 5;  // Defina os pinos corretos dos encoders
const int pinEncoderDir = 6;

const int pinMotorEsq = 9;    // Defina os pinos corretos dos motores
const int pinMotorDir = 10;

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
float Kp = 1.0;
float Ki = 0.5;
float Kd = 0.1;
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
  int velocidadeBase = 50;  // Velocidade base dos motores (ajustável)
  int velocidadeMax = 100;   // Valor máximo para PWM 

  // Cálculo da velocidade dos motores ajustando pelo PID
  int velocidadeEsq = velocidadeBase + PID;
  int velocidadeDir = velocidadeBase - PID;

  // Garantir que a velocidade esteja dentro do limite seguro
  velocidadeEsq = constrain(velocidadeEsq, 0, velocidadeMax);
  velocidadeDir = constrain(velocidadeDir, 0, velocidadeMax);

  analogWrite(pinMotorEsq, velocidadeEsq);
  analogWrite(pinMotorDir, velocidadeDir);
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

  PID = (Kp*proporcional) + (Ki*integral) + (Kd*derivativo);
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
  pinMode(pinMotorEsq, OUTPUT);
  pinMode(pinMotorDir, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(pinEncoderEsq), encoderEsqISR, RISING);
  attachInterrupt(digitalPinToInterrupt(pinEncoderDir), encoderDirISR, RISING);
}

void loop() {
    controlePID();
}