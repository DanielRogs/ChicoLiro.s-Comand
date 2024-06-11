/*

  Título: Códigos teste da equipe Hardware.
  Autores: Arthur Miranda, Harleny Angellica, Leandro Almeida, Luiz Amaral, Mariana Letícia e Renan Araújo.
  Descrição: é um código para teste dos componentes: disco encoder; sensor infravermelho; e PWM.
  Data de criação: 06/06/2024

*/

/* MAPEAMENTO DE HARDWARE */

// Conexões do motor (Os pinos de Enable EN_A e EN_B devem ser pinos de PWM para podermos controlar a velocidade dos motores)
// Esses pinos são as

#include <Arduino.h>
// Motor A
#define IN1 12  // Os pinos com o prefixo "IN" são pinos da Ponte H que determinam as entradas nos terminais dos motores, e consequentemente,
#define IN2 14  // a direção de rotação deles
#define EN_A 13 // Os pinos com o prefixo "EN" são pinos da Ponte H que vão determinar quanto de tensão os motores irão receber, assim determi-
                // nando a velocidade com a qual eles vão girar. Esses pinos irão receber um sinal PWM, que é um sinal digital que emula um
                // sinal analógico
// Motor B
#define IN3 27
#define IN4 26
#define EN_B 25

// Conexões do infravermelho
#define pinSensorMeio 15
#define pinSensorEsq 2
#define pinSensorDir 4

// Enconder
#define enc1 22
#define enc2 23

// LED RGB --> serão usados nesse código apenas como exemplo por enquanto para
//  indicar em lógica se o robô está indo para frente ou para trás
#define RED 5
#define GREEN 18
#define BLUE 19

// Botao pra acionar fonte de alimentacao
#define botaoAlimetacao 33

// Botão para ligar o robô
#define botaoLiga 21

// Variáveis de controle de velocidade e RPM
int rpm1 = 0, rpm2 = 0;
volatile byte pulsos1 = 0, pulsos2 = 0;
unsigned long timeold = 0;
unsigned int pulsos_por_volta = 20; // Ajuste de acordo com seu disco encoder

// setup
void setup()
{
  // Declarar os pinos a serem usados como OUTPUTs
  pinMode(EN_A, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(EN_B, OUTPUT);

  // LED RGB
  pinMode(BLUE, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);

  // Encoders e botoes
  pinMode(botaoLiga, INPUT);
  pinMode(botaoAlimetacao, INPUT);
  pinMode(enc1, INPUT);
  pinMode(enc2, INPUT);

  // sensores infravermelhos
  pinMode(pinSensorMeio, INPUT);
  pinMode(pinSensorEsq, INPUT);
  pinMode(pinSensorDir, INPUT);

  // Começar colocando todos os pinos em nível lógico baixo
  // Esses pinos abaixo determinam o sentido dos motores na ponte H
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  digitalWrite(BLUE, LOW);
  digitalWrite(GREEN, LOW);
  digitalWrite(RED, LOW);

  // Esses pinos abaixo determinam a velocidade na ponte H
  analogWrite(EN_A, 0); // Os pinos de saída analógicos do Arduino tem 8 bits de definição, logo variam entre 0 e 255 (2^8 = 256)
  analogWrite(EN_B, 0); // Mas no caso do motor, ela só pode variar de 0 até 100, pois se não, ele QUEIMA. Isso porque o que tá sendo controlado é a tensao.
                        // 255 significa 12v e o motor aguenta até 6v

  attachInterrupt(digitalPinToInterrupt(IN1), contador, FALLING); // Usando o pino motorPin1 para simular contador
  attachInterrupt(digitalPinToInterrupt(IN2), contador, FALLING); // Usando o pino motorPin3 para simular contador

  Serial.begin(9600);
}

// Loop
void loop()
{

  /*
   Esse trecho de codigo, está sendo utilizado pra visualizar no proprio hardware o sentido de rotacao do motor
   A gente tinha colocado 4 leds pra verificar se o motor giraria em um sentido, ou em outro dependendo da configuracao
   Cada motor tinha 1 led pra simbolizar que ta acionado pra ir pra frente ou pra tras
   Quando o "botaoLiga" é pressionado, o sentido muda, e consequentemente, os leds tambem
  */

  if (!digitalRead(botaoLiga))
  {
    digitalWrite(BLUE, HIGH);
    digitalWrite(GREEN, HIGH);
    digitalWrite(RED, HIGH);

    analogWrite(EN_A, 90);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(EN_B, 100);
  }
  else
  {
    digitalWrite(BLUE, LOW);
    digitalWrite(GREEN, LOW);
    digitalWrite(RED, LOW);

    analogWrite(EN_A, 100);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(EN_B, 90);
  }

  // Fazer os motores girarem para trás
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  // Acelerar e desacelerar os motores para trás
  motorAcel();
  motorDecel();

  // acionar o infravermelho
  infravermelho();

  // Fazer os motores girarem para frente
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  // Acelerar e desacelerar os motores para frente
  motorAcel();
  motorDecel();

  // acionar o infravermelho
  infravermelho();
}

// Função para acelerar o motor
void motorAcel()
{ // Acelera o motor com um incremento de velocidade a cada 20 milissegundos
  for (int i = 0; i < 100; i++)
  {
    // o argumento i para os EN_A e EN_B NÃO PODE ULTRAPASSAR 100 pois esse é o limite do PWM. Se ultrapassar IRÁ QUEIMAR O MOTOR
    analogWrite(EN_A, i);
    analogWrite(EN_B, i);
    calculaRPM();
    delay(200);
  }
}

// Função para desacelerar o motor
void motorDecel()
{ // Desacelera o motor com um decremento de velocidade a cada 20 milissegundos
  for (int i = 100; i >= 0; i--)
  {
    // o argumento i para os EN_A e EN_B NÃO PODE ULTRAPASSAR 100 pois esse é o limite do PWM. Se ultrapassar IRÁ QUEIMAR O MOTOR
    analogWrite(EN_A, i);
    analogWrite(EN_B, i);
    calculaRPM();
    delay(200);
  }
}

/*
A funcao abaixo faz o carrinho girar pra direita. Ela faz isso, girando uma roda pra frente, enquanto gira outra pra traz
A ponte H possui 6 pinos. Enable A, pinA1, pinA2, Enable B, pinB1, pinB2. Os enables controlam a velocidade
e os outros dois definem o sentido pra qual o motor vai girar. Por exemplo:

pinA1 = HIGH
pinA2 = LOW

Assim o motor vai pra frente. Se inverter:

pinA1 = LOW
pinA2 = HIGH

o motor vai girar pra trás. Se o dois estiverem iguais, o motor não funciona.
Nesse código, pinA1, pinA2, pinB1, pinB2, correspondem respectivamente,
aos IN1, IN2, IN3, IN4.

*/
// essa função considera que o robô está parado. Então ela é só um exemplo, e não considera todas as formas que o robô pode girar.
void girarDireita()
{

  // abaixo, mudamos a configuração padrão do motor B para que o robô gire para tras
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  for (int i = 0; i < 100; i++)
  {
    // o argumento i para os EN_A e EN_B NÃO PODE ULTRAPASSAR 100 pois esse é o limite do PWM. Se ultrapassar IRÁ QUEIMAR O MOTOR
    analogWrite(EN_A, i);
    analogWrite(EN_B, i);
    calculaRPM();
    delay(200);
  }

  // abaixo, mudamos a configuração alterada do motor B de volta ao padrão
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

// Função para calcular o RPM
// nessa função, estamos calculando o RPM para ambas as rodas e mostrando isso no terminal serial
void calculaRPM()
{
  // Calcula o RPM a cada segundo em cada um dos motores
  if (millis() - timeold >= 1000)
  {
    detachInterrupt(digitalPinToInterrupt(IN1)); // Detach interrupt
    detachInterrupt(digitalPinToInterrupt(IN3)); // Detach interrupt

    rpm1 = (60 * 1000 / pulsos_por_volta) / (millis() - timeold) * pulsos1;
    rpm2 = (60 * 1000 / pulsos_por_volta) / (millis() - timeold) * pulsos2;

    timeold = millis();
    Serial.print("Pulso motor 1 = ");
    Serial.println(pulsos1, DEC);
    Serial.print("Pulso motor 2 = ");
    Serial.println(pulsos2, DEC);

    pulsos1 = 0;
    pulsos2 = 0;

    // Mostra o valor de RPM no serial monitor
    Serial.print("RPM1 = ");
    Serial.println(rpm1, DEC);
    Serial.print("RPM2 = ");
    Serial.println(rpm2, DEC);

    attachInterrupt(digitalPinToInterrupt(IN1), contador, FALLING); // Attach interrupt
    attachInterrupt(digitalPinToInterrupt(IN3), contador, FALLING); // Attach interrupt
  }
}

// Incrementação da variavel pulsos toda vez que chama o attachinterrupt
void contador()
{
  pulsos1++;
  pulsos2++;
}

void infravermelho()
{
  bool sensorMeio = digitalRead(pinSensorMeio);
  bool sensorEsq = digitalRead(pinSensorEsq);
  bool sensorDir = digitalRead(pinSensorDir);

  if (sensorMeio && !sensorEsq && !sensorDir)
  {
    Serial.println("Reta");
  }

  else if (sensorEsq && !sensorDir)
  {
    Serial.println("Virar a Esquerda");
  }

  else if (!sensorEsq && sensorDir)
  {
    Serial.println("Virar a Direita");
  }

  else if (sensorMeio && sensorEsq && sensorDir)
  {
    Serial.println("Fim do Percurso");
  }

  else
  {
    Serial.println("Não detectado");
  }

  Serial.println(analogRead(pinSensorMeio));
  Serial.print(" ");
  Serial.println(analogRead(pinSensorEsq)); // esse aqui pode dar um valor um pouco diferente deles por conta da sombra que s1 e s3 dão
  Serial.print(" ");
  Serial.println(analogRead(pinSensorDir));
  Serial.println(" ");
  delay(100);
}