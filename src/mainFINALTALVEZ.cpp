/*
  Título: SISTEMA CONTROLADOR DO CARRINHO
  Autores: Daniel Rodrigues da Rocha
  Descrição: Código para controlar um carrinho seguidor de linha utilizando sensores infravermelhos e controle PWM.
  Data de criação: 19/06/2024
*/

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>

// Definindo as credenciais WiFi
const char* ssid = "iPhone de Alana";
const char* wifi_password = "alanaaa123";

// Definindo o URL do servidor
const char* serverUrl = "https://chicoliro.xobengala.com.br/api/dados/receive-data";
const char* serverFailUrl = "https://chicoliro.xobengala.com.br/api/trilha/failed";

// Motor A
#define IN1 12
#define IN2 14
#define EN_A 13

// Motor B
#define IN3 27
#define IN4 26
#define EN_B 25

// Conexões do infravermelho
#define pinSensorMeio 2
#define pinSensorEsq 15
#define pinSensorDir 4

// Variáveis de controle de velocidade
const int velocidadeBase = 75;
const int velocidadeMaxima = 90;
const int ajusteVelocidade = 5;
const int limitePreto = 20;

// Variáveis de leitura de tensão
double vadc = 3.3; // Tensão de referência do conversor analógico-digital (VADC)
double voltage_in; // Variável para armazenar a tensão lida
double real_voltage; // Variável para armazenar a tensão real
double R1 = 81.6; // Valor da resistência R1
double R2 = 26.5; // Valor da resistência R2

// Variáveis para a requisição
volatile bool isMoving = false;
volatile bool connected = false;

// Variáveis para aperfeiçoar o controle:
int naoDetectadoCounter = 0; // Contador para condição de não detectado
const int naoDetectadoLimite = 10; // Limite para parar os motores

WebServer server(80);
HTTPClient http;  // Declaração da variável global
void startRequests(){
  http.begin(serverUrl);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
}

void lerSensoresTensao() {
  voltage_in = ((double)analogRead(2) / 1024) * vadc * 0.2909;
  real_voltage = voltage_in * ((R1 + R2) / R2) * 2.97330697;
}

void enviarDados() {
  Serial.println(millis());
  String jsonData = "{\"rpmMotorDir\": " + String(analogRead(EN_A)) +
                      ", \"rpmMotorEsq\": " + String(analogRead(EN_B)) +
                      ", \"tensao\": " + String(real_voltage) +
                      ", \"isMoving\": " + String(isMoving) + "}"; 
  
  Serial.println(jsonData);
  int httpResponseCode = http.POST(jsonData);
  Serial.println(millis());
}

void enviarFalha() {
  http.begin(serverFailUrl);

  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String jsonData = "{}"; 
  
  int httpResponseCode = http.PATCH(jsonData);
  http.end();
}

// Tarefa para enviar dados periodicamente
void enviarDadosTask(void* parameter) {
  for (;;) {
    lerSensoresTensao();
    enviarDados();
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

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

  // Configuração do sensor de tensão
  pinMode(33, INPUT);

  // Inicializa todos os pinos dos motores em nível lógico baixo
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  analogWrite(EN_A, 0);
  analogWrite(EN_B, 0);

  Serial.begin(9600);

  // Conecte-se ao WiFi
  WiFi.begin(ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }
  Serial.println("Conectado ao WiFi");
  connected = true;

  // Inicie o servidor
  server.begin();

  // Cria uma tarefa para enviar dados periodicamente
  xTaskCreate(
    enviarDadosTask,    // Função que implementa a tarefa
    "EnviarDadosTask",  // Nome da tarefa
    4096,               // Tamanho da pilha da tarefa
    NULL,               // Parâmetro passado para a tarefa
    1,                  // Prioridade da tarefa
    NULL                // Handle da tarefa
  );
}

void loop() {
  // Leitura dos sensores infravermelhos
  
  int sensorMeio = analogRead(pinSensorMeio);
  int sensorEsq = analogRead(pinSensorEsq);
  int sensorDir = analogRead(pinSensorDir);
  Serial.println(sensorDir);

  if (sensorMeio > limitePreto && sensorEsq <= limitePreto && sensorDir <= limitePreto) {
    // Linha reta - ambos os motores com velocidade base
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(EN_A, velocidadeBase);

    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(EN_B, velocidadeBase);
    isMoving = true;
    naoDetectadoCounter = 0; // Resetar contador
  }
  else if (sensorEsq > limitePreto && sensorDir <= limitePreto) {
    // Virar à esquerda - reduzir velocidade do motor esquerdo
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(EN_A, velocidadeMaxima - ajusteVelocidade);

    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(EN_B, velocidadeMaxima);
    isMoving = true;
    naoDetectadoCounter = 0; // Resetar contador
  }
  else if (sensorDir > limitePreto && sensorEsq <= limitePreto) {
    // Virar à direita - reduzir velocidade do motor direito
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(EN_A, velocidadeMaxima);

    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(EN_B, velocidadeMaxima - ajusteVelocidade);
    isMoving = true;
    naoDetectadoCounter = 0; // Resetar contador
  }
  else if (sensorDir > limitePreto && sensorEsq <= limitePreto && sensorMeio > limitePreto) {
    // Virar à direita - reduzir velocidade do motor direito
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(EN_A, velocidadeBase);

    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(EN_B, velocidadeBase - ajusteVelocidade);
    isMoving = true;
    naoDetectadoCounter = 0; // Resetar contador
  }
  else if (sensorDir <= limitePreto && sensorEsq > limitePreto && sensorMeio > limitePreto) {
    // Virar à esquerda - reduzir velocidade do motor esquerdo
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(EN_A, velocidadeBase - ajusteVelocidade);

    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(EN_B, velocidadeBase);
    isMoving = true;
    naoDetectadoCounter = 0; // Resetar contador
  }
  // else if (sensorMeio > limitePreto && sensorEsq > limitePreto && sensorDir > limitePreto) {
  //   // Fim do percurso - parar o carrinho
  //   pararMotores();
  //   isMoving = false;
  //   enviarDados(); // Enviar dados de parada
  //   naoDetectadoCounter = 0; // Resetar contador
  // }
  // else {
  //   // Não detectado - Continuar andando com velocidade reduzida
  //   moverFrente(velocidadeBase - ajusteVelocidade, velocidadeBase - ajusteVelocidade);
  //   isMoving = true;
  //   naoDetectadoCounter++;
  //   if (naoDetectadoCounter >= naoDetectadoLimite) {
  //     pararMotores(); // Parar os motores se o limite for alcançado
  //     isMoving = false;
  //     enviarFalha();
  //   }
  // }

  //

  // Serial.println(sensorMeio);
  // Serial.print(" ");
  // Serial.println(sensorEsq); // esse aqui pode dar um valor um pouco diferente deles por conta da sombra que s1 e s3 dão
  // Serial.print(" ");
  // Serial.println(sensorDir);
  // Serial.println(" ");
}
/*
A funcao abaixo faz o carrinho girar pra direita ou esquerda. Ela faz isso, girando uma roda pra frente, enquanto gira outra pra traz
A ponte H possui 6 pinos. Enable A, pinA1, pinA2, Enable B, pinB1, pinB2. Os enables controlam a velocidade
e os outros dois definem o sentido pra qual o motor vai girar. Por exemplo:

pinA1 = HIGH
pinA2 = LOW

Assim o motor vai pra FRENTE. Se inverter:

pinA1 = LOW
pinA2 = HIGH

o motor vai girar pra TRÁS. Se o dois estiverem iguais, o motor não funciona.
Nesse código, pinA1, pinA2, pinB1, pinB2, correspondem respectivamente,
aos IN1, IN2, IN3, IN4.

*/

// Função para mover o carrinho para frente com controle de velocidade
void moverFrente(int velocidadeA, int velocidadeB) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(EN_A, velocidadeA);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(EN_B, velocidadeB);
}

// Função para parar os motores
void pararMotores() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(EN_A, 0);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(EN_B, 0);
}
