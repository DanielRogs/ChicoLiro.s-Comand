#include <Arduino.h>

// Motor A
#define IN1 12
#define IN2 14
#define EN_A 13

// Motor B
#define IN3 27
#define IN4 26
#define EN_B 25

// Conexões dos sensores infravermelhos (Portas analógicas)
#define pinSensorMeio 2
#define pinSensorEsq 15
#define pinSensorDir 4

// Variáveis de controle de velocidade
const int velocidadeBase = 70; // Velocidade base dos motores
const int velocidadeMaxima = 100; // Velocidade máxima dos motores

// Variáveis para controle PID
float erro;
float proporcional;
float integral;
float derivativo;
float erroAnterior = 0;
float Kp = 0.1;
float Ki = 0.0;
float Kd = 0.0;
float PID;
float baseSpeed = 80.0;

void setup() {
  // Configuração dos pinos dos motores
  pinMode(EN_A, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(EN_B, OUTPUT);

  // Configuração dos pinos dos sensores infravermelhos
  pinMode(pinSensorMeio, INPUT);
  pinMode(pinSensorEsq, INPUT);
  pinMode(pinSensorDir, INPUT);

  // Inicializa todos os pinos dos motores em nível lógico baixo
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  Serial.begin(9600);
}

int lerSensor(int sensorPin) {
  return analogRead(sensorPin);
}

float calcularErro(int sensorEsq, int sensorMeio, int sensorDir) {
  const int leituraLinha = 80; // Necessário calibragem
  
  if (sensorEsq > leituraLinha && sensorMeio <= leituraLinha && sensorDir <= leituraLinha) {
        return -2; // Vira à esquerda
    } else if (sensorEsq > leituraLinha && sensorMeio > leituraLinha && sensorDir <= leituraLinha) {
        return -1; // Esquerda-meio
    } else if (sensorEsq <= leituraLinha && sensorMeio > leituraLinha && sensorDir <= leituraLinha) {
        return 0.0;  // Centralizado
    } else if (sensorEsq <= leituraLinha && sensorMeio > leituraLinha && sensorDir > leituraLinha) {
        return 1;  // Direita-meio
    } else if (sensorEsq <= leituraLinha && sensorMeio <= leituraLinha && sensorDir > leituraLinha) {
        return 2;  // Vira à direita
    } else {
        return 0.0;  // Caso padrão (sem linha detectada ou linha detectada em todos os sensores)
  }
}

int calcularPID(int erro) {
    if (erro == 0){
    integral = 0;
  }

  proporcional = erro;
  integral += erro;

  // ajustar condicional conforme limite superior e inferior do PWM
  if (integral > 100){ 
    integral = 100;
  } else if (integral < -100) {
    integral = -100;
  }

  derivativo = erro - erroAnterior;

  float valuePID = (Kp*proporcional) + (Ki*integral) + (Kd*derivativo);
  erroAnterior = erro;

  return valuePID;
}

void controlarMotor(float valuePID) {
  if (valuePID > 0) {
  // Direita
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(EN_A, velocidadeBase);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(EN_B, velocidadeBase - valuePID);
  } else if (valuePID<0){
  // Esquerda
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(EN_A, velocidadeBase - valuePID);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(EN_B, velocidadeBase);
  } else {
  // Reto
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(EN_A, velocidadeBase);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(EN_B, velocidadeBase);
  }

}

void loop (){
  int sensorEsq = lerSensor(pinSensorEsq);
  int sensorMeio = lerSensor(pinSensorMeio);
  int sensorDir = lerSensor(pinSensorDir);

  erro = calcularErro(sensorEsq, sensorMeio, sensorDir);
  PID = calcularPID(erro);

  controlarMotor(PID);
}