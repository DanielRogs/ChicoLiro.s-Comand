// Conexões do motor (Os pinos de Enable EN_A e EN_B devem ser pinos de PWM para podermos controlar a velocidade dos motores)

#include <Arduino.h>
// Motor A
#define IN1 9
#define IN2 8
#define EN_A 10 
// Motor B
#define IN3 5
#define IN4 4
#define EN_B 3

void motorAcel () { // Acelera o motor com um incremento de velocidade a cada 20 milissegundos 
  for (int i = 0; i < 100; i++){ 
    analogWrite (EN_A, i);
    analogWrite (EN_B, i);
    delay (200);
  }
}

void motorDecel () { // Desacelera o motor com um decremento de velocidade a cada 20 milissegundos 
  for (int i = 100;i >=0; i--){ 
    analogWrite (EN_A, i);
    analogWrite (EN_B, i);
    delay (200);
  }
}


void setup() {
  // Declarar os pinos a serem usados como OUTPUTs
  pinMode (IN1, OUTPUT);
  pinMode (IN2, OUTPUT);
  pinMode (IN3, OUTPUT);
  pinMode (IN4, OUTPUT);
  pinMode (EN_A, OUTPUT);
  pinMode (EN_B, OUTPUT);

  // Começar colocando todos os pinos em nível lógico baixo 
  digitalWrite (IN1, LOW);
  digitalWrite (IN2, LOW);
  digitalWrite (IN3, LOW);
  digitalWrite (IN4, LOW);

  analogWrite (EN_A, 0); // Os pinos de saída analógicos do Arduino tem 8 bits de definição, logo variam entre 0 e 255 (2^8 = 256)
  analogWrite (EN_B, 0);

  Serial.begin(9600);
}

void loop() {
  // Fazer os motores girarem para trás
  digitalWrite (IN1, LOW);
  digitalWrite (IN2, HIGH);
  digitalWrite (IN3, LOW);
  digitalWrite (IN4, HIGH);

  // Acelerar e desacelerar os motores para trás
  motorAcel ();
  motorDecel ();
  
  // Fazer os motores girarem para frente
  digitalWrite (IN1, HIGH);
  digitalWrite (IN2, LOW);
  digitalWrite (IN3, HIGH);
  digitalWrite (IN4, LOW);

  // Acelerar e desacelerar os motores para frente
  motorAcel ();
  motorDecel ();

}