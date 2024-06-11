#include <Arduino.h>

// Definição de pinos dos sensores, eoconders e motores 
const int pinSensorEsq = 15;
const int pinSensorMeio = 2;
const int pinSensorDir = 4;

const int pinEncoderEsq = ;
const int pinEncoderDir = ;

const int pinMotorEsq = ;
const int pinMotorDir = ;

// Variáveis para leitura dos sensores
int sensorEsq;
int sensorMeio;
int sensorDir;

int encoderEsq;
int encoderDir;

// Variáveis para controle PID
float erro;
float proporcional;
float integral;
float derivativo;
float erroAnterior = 0;
float Kp = 0.0;
float Ki = 0.0;
float Kd = 0.0;
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

void controlarMotor() {}

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
}

void loop() {
    controlePID();
    controlarMotor();
}