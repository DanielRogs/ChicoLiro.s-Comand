#include <Arduino.h>

void ligarMotor(int velocidade);
void desligarMotor();
void contador();
void setupMotor();
void loopMotor();

// Pinos do driver de motor L298N
extern const int motorPin1 = 5; // Pino de controle do motor 1 no driver L298N
extern const int motorPin2 = 18; // Pino de controle do motor 2 no driver L298N
extern const int enablePin = 19; // Pino de habilitação PWM

//  Ajustar Pino do sensor de velocidade (encoder)
extern const int sensorPin = 26; // Pino ligado ao pino de sinal do encoder

// Variáveis de controle de velocidade e RPM
extern int rpm;
extern volatile byte pulsos;
extern unsigned long timeold;
extern unsigned int pulsos_por_volta; // Ajuste de acordo com seu disco encoder

void setup() {
  Serial.begin(9600);
  
  // Configuração dos pinos do driver de motor
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(enablePin, OUTPUT);

  // Configuração do pino do sensor de velocidade
  pinMode(sensorPin, INPUT);
  
  // Interrupção para contar pulsos do encoder
  attachInterrupt(digitalPinToInterrupt(sensorPin), contador, FALLING);
  
  pulsos = 0;
  rpm = 0;
  timeold = 0;
}

void loop() {
  // Liga o motor
  ligarMotor(128); // Define a velocidade do motor (0-255)

  // Calcula o RPM a cada segundo
  if (millis() - timeold >= 1000) {
    detachInterrupt(digitalPinToInterrupt(sensorPin));
    rpm = (60 * 1000 / pulsos_por_volta) / (millis() - timeold) * pulsos;
    timeold = millis();
    pulsos = 0;

    // Mostra o valor de RPM no serial monitor
    Serial.print("RPM = ");
    Serial.println(rpm, DEC);
    
    attachInterrupt(digitalPinToInterrupt(sensorPin), contador, FALLING);
  }

  // Desliga o motor após 10 segundos para o teste
  if (millis() > 10000) {
    desligarMotor();
  }

}