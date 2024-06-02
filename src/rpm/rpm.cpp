#include <Arduino.h> 
#define pinEncoder 4 

volatile int cont = 0; 
volatile float rpm = 0;
float oscilacoes_disco = 40; // 20 "paredes" e 20 "buracos"
void interrupcao();

void setup() {
  pinMode(pinEncoder, INPUT);
  attachInterrupt(digitalPinToInterrupt(pinEncoder), interrupcao, CHANGE);
  Serial.begin(115200);
}

void loop() {
  delay(1000);
  detachInterrupt(digitalPinToInterrupt(pinEncoder));
  rpm = (cont/oscilacoes_disco)*60.0; // Estimativa do RPM medido a cada 1 segundo
  cont = 0;
  attachInterrupt(digitalPinToInterrupt(pinEncoder), interrupcao, CHANGE);
  Serial.print("RPM: ");
  Serial.println(rpm);  
}

void interrupcao(){
  cont++;
}