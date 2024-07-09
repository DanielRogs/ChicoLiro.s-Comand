#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <HTTPClient.h>

// Definindo as credenciais WiFi
const char* ssid = "iPhone de Alana";
const char* wifi_password = "alanaaa123";

// Definindo o URL do servidor para onde os dados serão enviados
const char* serverUrl = "https://chicoliro.xobengala.com.br/api/dados/receive-data";
const char* serverFailUrl = "https://chicoliro.xobengala.com.br/api/trilha/failed";

// Criando uma instância do servidor
AsyncWebServer server(80);

// Variáveis para armazenar os dados dos sensores
volatile int rpmMotorDireito = 0;
volatile int rpmMotorEsquerdo = 0;
volatile int tensao = 0;
volatile bool isMoving = false;

// Variáveis de controle de velocidade
const int velocidadeBase = 75;
const int velocidadeMaxima = 90;
const int ajusteVelocidade = 5;
const int limitePreto = 50;

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
#define pinSensorMeio 2
#define pinSensorEsq 15
#define pinSensorDir 4

// Controle de tempo para envio de dados
unsigned long previousMillis = 0;
const long interval = 500; // Intervalo de tempo em milissegundos para envio de dados
const int limitePerdido = 10;
int tempoPerdido = 0;

// Função Exemplo para ler os sensores e calcular as RPMs
void lerSensores() {
  // Leitura dos sensores infravermelhos
  int sensorMeio = analogRead(pinSensorMeio);
  int sensorEsq = analogRead(pinSensorEsq);
  int sensorDir = analogRead(pinSensorDir);

  // A lógica do movimento do carrinho
  if (sensorMeio > limitePreto && sensorEsq <= limitePreto && sensorDir <= limitePreto) {
    // Linha reta - ambos os motores com velocidade base
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(EN_A, velocidadeBase);

    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(EN_B, velocidadeBase);
    isMoving = true;
    tempoPerdido = 0;
  } else if (sensorEsq > limitePreto && sensorDir <= limitePreto) {
    // Virar à esquerda - reduzir velocidade do motor esquerdo
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(EN_A, velocidadeMaxima - ajusteVelocidade);

    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(EN_B, velocidadeMaxima);
    isMoving = true;
    tempoPerdido = 0;
  } else if (sensorDir > limitePreto && sensorEsq <= limitePreto) {
    // Virar à direita - reduzir velocidade do motor direito
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(EN_A, velocidadeMaxima);

    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(EN_B, velocidadeMaxima - ajusteVelocidade);
    isMoving = true;
    tempoPerdido = 0;
  } else if (sensorDir > limitePreto && sensorEsq <= limitePreto && sensorMeio > limitePreto) {
    // Virar à direita - reduzir velocidade do motor direito
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(EN_A, velocidadeBase);

    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(EN_B, velocidadeBase - ajusteVelocidade);
    isMoving = true;
    tempoPerdido = 0;
  } else if (sensorDir <= limitePreto && sensorEsq > limitePreto && sensorMeio > limitePreto) {
    // Virar à esquerda - reduzir velocidade do motor esquerdo
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(EN_A, velocidadeBase - ajusteVelocidade);

    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(EN_B, velocidadeBase);
    isMoving = true;
    tempoPerdido = 0;
  } else if (sensorMeio > limitePreto && sensorEsq > limitePreto && sensorDir > limitePreto) {
    // Fim do percurso - parar o carrinho
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    analogWrite(EN_A, 0);

    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    analogWrite(EN_B, 0);
    isMoving = false;
    tempoPerdido = 0;
  } else {
    // Não detectado - analisar caso
    if (tempoPerdido >= limitePerdido){
      // Carro está perdido - parar o carrinho
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
      analogWrite(EN_A, 0);

      digitalWrite(IN3, LOW);
      digitalWrite(IN4, LOW);
      analogWrite(EN_B, 0);
      isMoving = false;
    }
    else {
      // Carro está perdido, mas talvez consiga se encontrar - ande devagar
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      analogWrite(EN_A, velocidadeBase - ajusteVelocidade);

      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
      analogWrite(EN_B, velocidadeBase - ajusteVelocidade);
      isMoving = true;
      tempoPerdido++;
    }
  }

  rpmMotorDireito = 200; // Substitua por cálculo real se necessário
  rpmMotorEsquerdo = 400; // Substitua por cálculo real se necessário
  tensao = 9; // Substitua por leitura de tensão real se necessário
}

// Função para enviar os dados para a API REST
void enviarDados() {
  HTTPClient http;
  http.begin(serverUrl);
  http.addHeader("Content-Type", "application/json");

  String jsonData = "{\"rpmMotorDir\": " + String(rpmMotorDireito) +
                    ", \"rpmMotorEsq\": " + String(rpmMotorEsquerdo) +
                    ", \"tensao\": " + String(tensao) +
                    ", \"isMoving\": " + String(isMoving) + "}";

  int httpResponseCode = http.POST(jsonData);
  http.end();
}

// Função para enviar os dados para a API REST DE FALHA
void enviarFalha() {
  HTTPClient http;
  http.begin(serverFailUrl);
  http.addHeader("Content-Type", "application/json");

  String jsonData = "{}";

  int httpResponseCode = http.PATCH(jsonData);
  http.end();
}

// Configuração inicial
void setup() {
  Serial.begin(9600);

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

  // Inicializa todos os pinos dos motores em nível lógico baixo
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  // Esses pinos abaixo determinam a velocidade na ponte H
  analogWrite(EN_A, 0);
  analogWrite(EN_B, 0);

  // Conecte-se ao WiFi
  WiFi.begin(ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    // Serial.println("Conectando ao WiFi...");
  }
  // Serial.println("Conectado ao WiFi");
  // Inicie o servidor
  server.begin();
}

// Loop principal
void loop() {
  // Ler sensores e controlar motores
  lerSensores();
  
  // Controle baseado em tempo para envio de dados
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (tempoPerdido >= limitePerdido){
      enviarFalha();
    }
    else {
      enviarDados();
    }
  }
}
