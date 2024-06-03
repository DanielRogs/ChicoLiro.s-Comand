#include <Arduino.h>
#define pinEncoder 4 

// Pinos do driver de motor L298N
const int motorPin1 = 5;   // Pino de controle do motor 1 no driver L298N
const int motorPin2 = 18;  // Pino de controle do motor 2 no driver L298N
const int enablePin = 19;  // Pino de habilitação PWM

volatile int cont = 0; 
volatile float rpm = 0;
float oscilacoes_disco = 40; // 20 "paredes" e 20 "buracos"
unsigned long timeold;
unsigned int pulsos_por_volta =13;
float distancia_percorrida = 0;     

// Variável de conversão de pulsos para distância
const float perimetro_roda = 0.2; // Perímetro da roda em metros (ajustar)

void ligarMotor(int velocidade);
void desligarMotor();
void interrupcao();

void setupMotor() {
  Serial.begin(115200);
  
  // Configuração dos pinos do driver de motor
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(enablePin, OUTPUT);
  
  // Configuração dos pinos do encoder
  pinMode(pinEncoder, INPUT);
 
  // Interrupção para contar pulsos do encoder
  attachInterrupt(digitalPinToInterrupt(pinEncoder), interrupcao, CHANGE);
  
  rpm = 0;
  timeold = millis(); // Inicializa o tempo
}

void loopMotor() {
  // Oscilação da velocidade do motor
  for (int speed = 0; speed <= 127; speed++) {
    ligarMotor(speed); // Liga o motor com a velocidade atual
    delay(10); // intervalo
  }

  for (int speed = 255; speed >= 10; speed--) {
    ligarMotor(speed); // Oscila até 10
    delay(10); 
    if (speed == 10) {
      for (int speed = 10; speed <= 127; speed++) {
        ligarMotor(speed); // Oscila até 255 novamente
        delay(10); 
      }
    }
  }

   // Calcula o RPM e a distância percorrida a cada segundo
  unsigned long tempo_atual = millis();
  unsigned long tempo_decorrido = tempo_atual - timeold;

  if (tempo_decorrido >= 1000) {
    detachInterrupt(digitalPinToInterrupt(pinEncoder));
    rpm = (cont / oscilacoes_disco) * 60.0; // Estimativa do RPM medido a cada 1 segundo

    // Calcula a distância percorrida
    float revolucoes = (cont / oscilacoes_disco) / pulsos_por_volta;
    distancia_percorrida += revolucoes * perimetro_roda; // Incrementa a distância percorrida

    cont = 0; // Zera o contador
    timeold = tempo_atual; // Atualiza o tempo antigo
    attachInterrupt(digitalPinToInterrupt(pinEncoder), interrupcao, CHANGE);

    // Mostra os valores de RPM e distância no monitor serial
    Serial.print("RPM = ");
    Serial.print(rpm);
    Serial.print(" | Distância percorrida = ");
    Serial.print(distancia_percorrida);
    Serial.println(" metros");
  }
}

void ligarMotor(int velocidade) {
  // Define o sentido de rotação do motor
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, LOW);

  // Controla a velocidade do motor
  int pwmValor = map(velocidade, 0, 255, 0, 127); // Mapeia para metade do valor máximo para 6V em vez de 12V
  analogWrite(enablePin, velocidade); // controla o velocidade do motor através do pino  que envia o sinal PWM
}

void desligarMotor() {
  analogWrite(enablePin, 0); 
}

void interrupcao() {
  cont++;
}