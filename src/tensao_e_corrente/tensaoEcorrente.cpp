/*
  Código para leitura de corrente fornecida pela bateria do robô seguidor de linha, com 
  o uso de um módulo sensor de corrente ACS712ELCTR-05A-T e média móvel para filtrar
  eventuais ruídos que afetem a leitura do sensor

  Autor: Luiz Henrique da Silva Amaral - 6º Semestre de Engenharia Eletrônica - UnB
  Matrícula: 211062160
*/

#include <Arduino.h>

// ---------- Constantes Auxiliares ---------- //

#define n 25          // Número de pontos da média móvel

// // Motor A
// #define IN1 12  // Os pinos com o prefixo "IN" são pinos da Ponte H que determinam as entradas nos terminais dos motores, e consequentemente,
// #define IN2 14  // a direção de rotação deles
// #define EN_A 13 // Os pinos com o prefixo "EN" são pinos da Ponte H que vão determinar quanto de tensão os motores irão receber, assim determi-
//                 // nando a velocidade com a qual eles vão girar. Esses pinos irão receber um sinal PWM, que é um sinal digital que emula um
//                 // sinal analógico
     
// // Motor B
// #define IN3 27
// #define IN4 26
// #define EN_B 25

// ---------- Variáveis Globais ---------- // 

int adc_pin35;   // Variável que guardará o valor de tensão lido no pino D35 da Esp32, em bits
float voltage;            // Variável que guardará o valor real da tensão lida no pino D35 da Esp32
float current;            // Variável que guardará o valor da corrente que está passando pelo módulo sensor de corrente
float current_filtered;   // Variável que guardará o valor da corrente após passar pelo filtro de média móvel para diminuir os ruídos sendo lidos
float voltage_filtered;
float numbers[n];          // Vetor com os valores da média móvel
float numbers_current[n];

// Variáveis de controle de velocidade
// const int velocidadeBase = 75;
// const int velocidadeMaxima = 90;
// const int ajusteVelocidade = 5;
// const int limitePreto = 50;

// // Conexões do infravermelho
// #define pinSensorMeio 2
// #define pinSensorEsq 15
// #define pinSensorDir 4


// ---------- Função para retornar o valor de corrente sendo lida pelo sensor ACS712ELCTR-05A-T ---------- //

void amperimetro(){
  voltage = ((((float)analogRead(35) * 5) / 4096) * 0.70700) - 0.03;
  // voltage = (((float)analogRead(35) * 5) / 4096) * 0.72062;
                                                                          // Leitura do pino ADC 35 da Esp, capaz de converter sinais analógicos para digitais
                                                                           /* Cálculo para converter o valor lido (0-4096) em um valor de tensão útil para nossos cálculos.
                                                 /* O cinco se deve ao fato do sensor de corrente medir um valor máximo de 5A, e o valor '4096'
                                                  se deve ao fato dos pinos ADC da Esp32 terem uma resolução de 12 bits (2^12 = 4096) */ 
  current = ((voltage - 2.5) / 0.185) * 1.78;           /* Transforma o valor de tensão lido no pino 35 da Esp no valor de corrente que está passando
                                                 pelo módulo. A tensão (voltage) lida é subtraída por 2.5, pois a saída do chip ACS712 presente
                                                 no módulo usado é 2.5 quando há 0A sendo lido. Divide por 0.185, pois o sensor utilizado tem 
                                                 uma razão de saída de 185mV/A */
  //Serial.print("Corrente: ");
  //Serial.println(current);
  delay(300);
}

void setup() {
  // Configuração dos pinos dos motores
//   pinMode(EN_A, OUTPUT);
//   pinMode(IN1, OUTPUT);
//   pinMode(IN2, OUTPUT);
//   pinMode(IN3, OUTPUT);
//   pinMode(IN4, OUTPUT);
//   pinMode(EN_B, OUTPUT);

//   // Configuração dos pinos dos sensores infravermelhos
//   pinMode(pinSensorMeio, INPUT);
//   pinMode(pinSensorEsq, INPUT);
//   pinMode(pinSensorDir, INPUT);

//   // Inicializa todos os pinos dos motores em nível lógico baixo
//   digitalWrite(IN1, LOW);
//   digitalWrite(IN2, LOW);
//   digitalWrite(IN3, LOW);
//   digitalWrite(IN4, LOW);

// // Esses pinos abaixo determinam a velocidade na ponte H
//   analogWrite(EN_A, 0); // Os pinos de saída analógicos do Arduino tem 8 bits de definição, logo variam entre 0 e 255 (2^8 = 256)
//   analogWrite(EN_B, 0); // Mas no caso do motor, ela só pode variar de 0 até 100, pois se não, ele QUEIMA. Isso porque o que tá sendo controlado é a tensao.
//                         // 255 significa 12v e o motor aguenta até 6v

  Serial.begin(9600);

}

void loop() {
  // // Leitura dos sensores infravermelhos
  // int sensorMeio = analogRead(pinSensorMeio);
  // int sensorEsq = analogRead(pinSensorEsq);
  // int sensorDir = analogRead(pinSensorDir);

  amperimetro();                                // Peço para ele chamar a função 'amperimetro' no loop para calcular a corrente lida no sensor
  voltage_filtered = moving_average_voltage(); 
  current_filtered = moving_average_current(); 
   // Faço uso da função de média móvel e coloco seu resultado na variável gloval 'current_filtered'
  Serial.print ("Corrente Lida:");
  Serial.print (current, 5);                       // Printo o valor de corrente sendo lida antes de ser filtrada pela média móvel
  Serial.print (" || ");
  Serial.print ("Corrente Filtrada:");
  Serial.print (current_filtered, 5);                       // Printo o valor de corrente sendo lida antes de ser filtrada pela média móvel
  Serial.print (" || ");
  Serial.print ("Tensão Filtrada:");
  Serial.print (voltage_filtered, 5);            // Printo o valor de corrente sendo lido após ser tratada pela função de média móvel
  Serial.print (" || ");
  Serial.print ("Tensão Lida:");
  Serial.println(voltage, 5);

  // if (sensorMeio > limitePreto && sensorEsq <= limitePreto && sensorDir <= limitePreto) {
  //   // Linha reta - ambos os motores com velocidade base
  //   // moverFrente(velocidadeBase, velocidadeBase);
  //   digitalWrite(IN1, LOW);
  //   digitalWrite(IN2, HIGH);
  //   analogWrite(EN_A, velocidadeBase);

  //   digitalWrite(IN3, HIGH);
  //   digitalWrite(IN4, LOW);
  //   analogWrite(EN_B, velocidadeBase);
  // }
  // else if (sensorEsq > limitePreto && sensorDir <= limitePreto) {
  //   // Virar à esquerda - reduzir velocidade do motor esquerdo
  //   // moverFrente(0, velocidadeMaxima);
  //   digitalWrite(IN1, HIGH);
  //   digitalWrite(IN2, LOW);
  //   analogWrite(EN_A, velocidadeMaxima - ajusteVelocidade);

  //   digitalWrite(IN3, HIGH);
  //   digitalWrite(IN4, LOW);
  //   analogWrite(EN_B, velocidadeMaxima);
  // }
  // else if (sensorDir > limitePreto && sensorEsq <= limitePreto) {
  //   // Virar à direita - reduzir velocidade do motor direito
  //   // moverFrente(velocidadeMaxima, 0);
  //   digitalWrite(IN1, LOW);
  //   digitalWrite(IN2, HIGH);
  //   analogWrite(EN_A, velocidadeMaxima);

  //   digitalWrite(IN3, LOW);
  //   digitalWrite(IN4, HIGH);
  //   analogWrite(EN_B, velocidadeMaxima - ajusteVelocidade);
  // }
  // else if (sensorDir > limitePreto && sensorEsq <= limitePreto && sensorMeio > limitePreto) {
  //   // Virar à direita - reduzir velocidade do motor direito
  //   // moverFrente(velocidadeBase, 0);
  //   digitalWrite(IN1, LOW);
  //   digitalWrite(IN2, HIGH);
  //   analogWrite(EN_A, velocidadeBase);

  //   digitalWrite(IN3, LOW);
  //   digitalWrite(IN4, HIGH);
  //   analogWrite(EN_B, velocidadeBase - ajusteVelocidade);
  // }
  // else if (sensorDir <= limitePreto && sensorEsq > limitePreto && sensorMeio > limitePreto) {
  //   // Virar à direita - reduzir velocidade do motor esquerdo
  //   // moverFrente(0, velocidadeBase);
  //   digitalWrite(IN1, HIGH);
  //   digitalWrite(IN2, LOW);
  //   analogWrite(EN_A, velocidadeBase - ajusteVelocidade);

  //   digitalWrite(IN3, HIGH);
  //   digitalWrite(IN4, LOW);
  //   analogWrite(EN_B, velocidadeBase);
  // }
  // else if (sensorMeio > limitePreto && sensorEsq > limitePreto && sensorDir > limitePreto) {
  //   // Fim do percurso - parar o carrinho
  //   pararMotores();
  // }
  // else {
  //   // Não detectado - parar o carrinho
  //   moverFrente(velocidadeBase - ajusteVelocidade, velocidadeBase - ajusteVelocidade);
  // }
  
}

// ---------- Função para calcular a média móvel da corrente fornecida pela função 'amperimetro' ---------- //

float moving_average_voltage(){
  // Desloca os elementos do vetor de acordo com o número de pontos
  for (int i = n-1; i > 0; i--) numbers[i] = numbers[i-1]; 
  // Posição inicial do vetor recebe a leitura original
  numbers[0] = voltage;
  // Acumulador para somar os pontos da média móvel
  float acc = 0;
  // Faz a somatória do número de pontos
  for (int i = 0; i<n; i++) acc+= numbers[i];
  // Retorna a média móvel
  return acc/n;
}

float moving_average_current(){
  // Desloca os elementos do vetor de acordo com o número de pontos
  for (int i = n-1; i > 0; i--) numbers_current[i] = numbers_current[i-1]; 
  // Posição inicial do vetor recebe a leitura original
  numbers_current[0] = current;
  // Acumulador para somar os pontos da média móvel
  float acc = 0;
  // Faz a somatória do número de pontos
  for (int i = 0; i<n; i++) acc+= numbers_current[i];
  // Retorna a média móvel
  return acc/n;
}

// void pararMotores() {
//   digitalWrite(IN1, LOW);
//   digitalWrite(IN2, LOW);
//   analogWrite(EN_A, 0);

//   digitalWrite(IN3, LOW);
//   digitalWrite(IN4, LOW);
//   analogWrite(EN_B, 0);
// }

// void moverFrente(int velocidadeA, int velocidadeB) {
//   digitalWrite(IN1, LOW);
//   digitalWrite(IN2, HIGH);
//   analogWrite(EN_A, velocidadeA);

//   digitalWrite(IN3, HIGH);
//   digitalWrite(IN4, LOW);
//   analogWrite(EN_B, velocidadeB);
// }
