/*
  Título: Carrinho Seguidor de Linha com PID
  Autores: Arthur Miranda, Harleny Angellica, Leandro Almeida, Luiz Amaral, Mariana Letícia e Renan Araújo.
  Descrição: Código para controlar um carrinho seguidor de linha utilizando sensores infravermelhos, encoders e controle PID.
  Data de criação: 06/06/2024
*/

/* MAPEAMENTO DE HARDWARE */

#include <Arduino.h>

/*
// Motor A - CORRIGIR
#define IN1 12  // Os pinos com o prefixo "IN" são pinos da Ponte H que determinam as entradas nos terminais dos motores, e consequentemente,
#define IN2 14  // a direção de rotação deles
#define EN_A 13 // Os pinos com o prefixo "EN" são pinos da Ponte H que vão determinar quanto de tensão os motores irão receber, assim determi-
                // nando a velocidade com a qual eles vão girar. Esses pinos irão receber um sinal PWM, que é um sinal digital que emula um
                // sinal analógico      
// Motor B
#define IN3 27
#define IN4 26
#define EN_B 25
*/

// Conexões dos sensores infravermelhos (Portas analógicas)
#define pinSensorMeio 2
#define pinSensorEsq 15
#define pinSensorDir 4

// Conexões dos encoders
//#define encA 22
//#define encB 23

// Variáveis de controle de velocidade
const int velocidadeBase = 80; // Velocidade base dos motores
const int velocidadeMaxima = 80; // Velocidade máxima dos motores
const int limitePreto = 80; // Valor limite para detectar a linha preta (ajuste conforme necessário)

// Variáveis PID
float kp = 0.0; // Ganho proporcional
float ki = 0.0; // Ganho integral
float kd = 0.0; // Ganho derivativo
float erro = 0.0;
float erroAnterior = 0.0;
float integral = 0.0;
float derivativo = 0.0;
float ajuste = 0.0;
unsigned long tempoAnterior = 0;

/*
// Variáveis dos encoders
volatile int pulsosA = 0;
volatile int pulsosB = 0;
unsigned long tempoAtualA = 0;
unsigned long tempoAtualB = 0;
float rpmA = 0;
float rpmB = 0;
const int pulsosPorVolta = 20;
*/

void setup() {
  // Configuração dos pinos dos motores
  /*
  pinMode(EN_A, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(EN_B, OUTPUT);
  */

  // Configuração dos pinos dos sensores infravermelhos
  pinMode(pinSensorMeio, INPUT);
  pinMode(pinSensorEsq, INPUT);
  pinMode(pinSensorDir, INPUT);

/*
  // Configuração dos encoders
  pinMode(encA, INPUT);
  pinMode(encB, INPUT);

  attachInterrupt(digitalPinToInterrupt(encA), encoderA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encB), encoderB, CHANGE);
*/

  /*
  // Inicializa todos os pinos dos motores em nível lógico baixo
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  analogWrite(EN_A, 0);
  analogWrite(EN_B, 0);
  */

  Serial.begin(9600);
}

void loop() {
  // Leitura dos sensores infravermelhos
  int sensorMeio = analogRead(pinSensorMeio);
  int sensorEsq = analogRead(pinSensorEsq);
  int sensorDir = analogRead(pinSensorDir);

  // Calcular erro
  if (sensorMeio > limitePreto && sensorEsq <= limitePreto && sensorDir <= limitePreto) {
    erro = 0;
  }
  else if (sensorEsq > limitePreto && sensorDir <= limitePreto) {
    erro = -1;
  }
  else if (sensorDir > limitePreto && sensorEsq <= limitePreto) {
    erro = 1;
  }
  else if (sensorDir > limitePreto && sensorEsq <= limitePreto && sensorMeio > limitePreto) {
    // Virar à direita - reduzir velocidade do motor direito
    erro = -2;
  }
  else if (sensorDir <= limitePreto && sensorEsq > limitePreto && sensorMeio > limitePreto) {
    // Virar à direita - reduzir velocidade do motor direito
    erro = -2;
  }
  else {
    erro = 0;
  }

  // Calcular PID
  unsigned long tempoAtual = millis();
  unsigned long deltaTempo = tempoAtual - tempoAnterior;
  if (deltaTempo > 0) {  // Prevenir divisão por zero
    integral += erro * deltaTempo;
    derivativo = (erro - erroAnterior) / deltaTempo;

    ajuste = (kp * erro) + (ki * integral) + (kd * derivativo);

    erroAnterior = erro;
    tempoAnterior = tempoAtual;
  }

  Serial.print(sensorMeio);
  Serial.print(" // ");
  Serial.print(sensorEsq); // esse aqui pode dar um valor um pouco diferente deles por conta da sombra que s1 e s3 dão
  Serial.print(" // ");
  Serial.print(sensorDir);
  Serial.println("");

  Serial.print("Ajuste = ");
  Serial.print(ajuste);


  // Calcular RPM dos motores
  //calculaRPM();

  // Ajustar velocidades dos motores com base no RPM e no ajuste PID
  // int velocidadeMotorA = velocidadeBase + ajuste;
  // int velocidadeMotorB = velocidadeBase - ajuste;

  // // Garantir que as velocidades estejam dentro dos limites permitidos
  // velocidadeMotorA = constrain(velocidadeMotorA, 0, velocidadeMaxima);
  // velocidadeMotorB = constrain(velocidadeMotorB, 0, velocidadeMaxima);

  // moverFrente(velocidadeMotorA, velocidadeMotorB);

  delay(500);
}

// Função para mover o carrinho para frente com controle de velocidade
// void moverFrente(int velocidadeA, int velocidadeB) {
//   digitalWrite(IN1, LOW);
//   digitalWrite(IN2, HIGH);
//   analogWrite(EN_A, velocidadeA);

//   digitalWrite(IN3, HIGH);
//   digitalWrite(IN4, LOW);
//   analogWrite(EN_B, velocidadeB);
// }

// Função para parar os motores
/*
void pararMotores() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(EN_A, 0);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(EN_B, 0);
}
*/

/*
// Função de interrupção para o encoder do motor A
void encoderA() {
  pulsosA++;
}

// Função de interrupção para o encoder do motor B
void encoderB() {
  pulsosB++;
}

// Função para calcular o RPM dos motores

void calculaRPM() {
  unsigned long tempoAgora = millis();

  // Calcular RPM do motor A
  if (tempoAgora - tempoAtualA >= 1000) {
    rpmA = (float)(pulsosA * 60) / (float)(pulsosPorVolta);
    pulsosA = 0;
    tempoAtualA = tempoAgora;
  }

  // Calcular RPM do motor B
  if (tempoAgora - tempoAtualB >= 1000) {
    rpmB = (float)(pulsosB * 60) / (float)(pulsosPorVolta);
    pulsosB = 0;
    tempoAtualB = tempoAgora;
  }
}
*/