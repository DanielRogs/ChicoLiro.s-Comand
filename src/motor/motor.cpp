#include <Arduino.h>

// Pinos do driver de motor L298N
const int motorPin1 = 5;   // Pino de controle do motor 1 no driver L298N
const int motorPin2 = 18;  // Pino de controle do motor 2 no driver L298N
const int enablePin = 19;  // Pino de habilitação PWM

// Variáveis de controle de velocidade e RPM
int rpm;
volatile byte pulsos;
unsigned long timeold;
unsigned int pulsos_por_volta = 13; // Ajuste de acordo com seu disco encoder

void ligarMotor(int velocidade);
void desligarMotor();
void contador();

void setupMotor() {
  Serial.begin(9600);
  
  // Configuração dos pinos do driver de motor
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(enablePin, OUTPUT);
  
  // Interrupção para contar pulsos do encoder
  attachInterrupt(digitalPinToInterrupt(motorPin1), contador, FALLING); // Usando o pino motorPin1 para simular contador
  
  pulsos = 0;
  rpm = 0;
  timeold = millis(); // Inicializa o tempo
}

void loopMotor() {
  // Oscilação da velocidade do motor
  for (int speed = 0; speed <= 255; speed++) {
    ligarMotor(speed); // Liga o motor com a velocidade atual
    delay(10); // intervalo
  }

  for (int speed = 255; speed >= 10; speed--) {
    ligarMotor(speed); // Oscila até 10
    delay(10); 
    if (speed == 10) {
      for (int speed = 10; speed <= 255; speed++) {
        ligarMotor(speed); // Oscila até 255 novamente
        delay(10); 
      }
    }
  }

  // Calcula o RPM a cada segundo
  if (millis() - timeold >= 1000) {
    detachInterrupt(digitalPinToInterrupt(motorPin1)); // Detach interrupt
    rpm = (60 * 1000 / pulsos_por_volta) * pulsos / (millis() - timeold);
    timeold = millis();
    pulsos = 0;

    // Mostra o valor de RPM no serial monitor
    Serial.print("RPM = ");
    Serial.println(rpm, DEC);
    
    attachInterrupt(digitalPinToInterrupt(motorPin1), contador, FALLING); // Attach interrupt
  }
}

void ligarMotor(int velocidade) {
  // Define o sentido de rotação do motor
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, LOW);

  // Controla a velocidade do motor
  analogWrite(enablePin, velocidade);
}

void desligarMotor() {
  analogWrite(enablePin, 0); // Desliga o motor
}

void contador() {
  pulsos++;
}
