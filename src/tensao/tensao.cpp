// Fatores que podem afetar no teste de leitura de tensão:
//  * Alimentação inadequada do microcontrolador 
//  * A não medição dos valores reais de resistência
#include <Arduino.h>


// Declaração de variáveis globais
double vadc = 3.3; // Tensão de referência do conversor analógico-digital (VADC)
double voltage_in; // Variável para armazenar a tensão lida
double real_voltage; // Variável para armazenar a tensão real
double R1 = 81.6; // Valor da resistência R1
double R2 = 26.5; // Valor da resistência R2

// Configuração inicial
void setup() {
  Serial.begin(9600); // Inicia a comunicação serial com a velocidade de 9600 bps
}

// Loop principal
void loop() {
  // Realiza a leitura do pino analógico 2, converte para tensão e calcula a tensão lida
  voltage_in = ((double)analogRead(2)/1024) * vadc * 0.2909;
  
  // Calcula a tensão real com base na relação de resistência R1 e R2
  real_voltage = voltage_in * ((R1+R2)/R2) * 2.97330697; // Relação ((R1+R2)/R2) = 4,079245283
  
  // Imprime a tensão lida e a tensão real na porta serial
  Serial.print(voltage_in,3);
  Serial.println(" tensão lida");
  Serial.print(real_voltage,3);
  Serial.println(" tensão real");
  
  delay(1000); // Aguarda 1 segundo antes de repetir o loop
}
