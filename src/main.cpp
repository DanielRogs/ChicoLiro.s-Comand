/*
  Título: SISTEMA CONTROLADOR DO CARRINHO
  Autores: Daniel Rodrigues da Rocha
  Descrição: Código para controlar um carrinho seguidor de linha utilizando sensores infravermelhos e controle PWM.
  Data de criação: 19/06/2024
*/

/* MAPEAMENTO DE HARDWARE */
// Conexões do motor (Os pinos de Enable EN_A e EN_B devem ser pinos de PWM para podermos controlar a velocidade dos motores)

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <HTTPClient.h>

// Definindo as credenciais WiFi
const char* ssid = "iPhone de Alana";
const char* wifi_password = "alanaaa123";

// Definindo o URL do servidor
const char* serverUrl = "https://chicoliro.xobengala.com.br/api/dados/receive-data";
const char* serverFailUrl = "https://chicoliro.xobengala.com.br/api/trilha/failed";

// Motor A - CORRIGIR
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

// Conexões dos encoders
// const byte MOTOR_A = 23;
// const byte MOTOR_B = 22;
// volatile int counter_A, counter_B = 0;
// unsigned long previousMillis = 0;
// const unsigned long interval = 1000; // Intervalo de 1 segundo
// const float diskslots = 20.00;
// const float wheeldiameter = 6.50;

// Variáveis dos encoders:
// float rotation_A, rotation_B = 0.0;

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

AsyncWebServer server(80);

// void calculateSpeed() {

//   // CALCULO DO RPM DO MOTOR A
//   rotation_A = ((counter_A / diskslots) * 60.00) / 2;
//   // float speed_A = (rotation_A * wheeldiameter * 3.14) / 60;

//   // CALCULO DO RPM DO MOTOR B
//   rotation_B = ((counter_B / diskslots) * 60.00) / 2;
//   // float speed_B = (rotation_B * wheeldiameter * 3.14) / 60;

//   counter_A = 0;
//   counter_B = 0;
// }

// Função de interrupção para contar pulsos do motor A
// void ISR_count1() {
//   counter_A++;
// }

// // Função de interrupção para contar pulsos do motor B
// void ISR_count2() {
//   counter_B++;
// }

void lerSensoresTensao() {
  voltage_in = ((double)analogRead(2) / 1024) * vadc * 0.2909;
  real_voltage = voltage_in * ((R1 + R2) / R2) * 2.97330697;
}

void enviarDados() {
  HTTPClient http;
  http.begin(serverUrl);

  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String jsonData = "{\"rpmMotorDir\": " + String(200) +
                      ", \"rpmMotorEsq\": " + String(100) +
                      ", \"tensao\": " + String(9) +
                      ", \"isMoving\": " + String(isMoving) + "}"; 
  
  int httpResponseCode = http.POST(jsonData);
  http.end();
}

void enviarFalha() {
  HTTPClient http;
  http.begin(serverFailUrl);

  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String jsonData = "{}"; 
  
  int httpResponseCode = http.PATCH(jsonData);
  http.end();
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

  // Configuração do encoder
  // attachInterrupt(digitalPinToInterrupt(MOTOR_A), ISR_count1, RISING);
  // attachInterrupt(digitalPinToInterrupt(MOTOR_B), ISR_count2, RISING);

  // Conecte-se ao WiFi
  WiFi.begin(ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    // Serial.println("Conectando ao WiFi...");
  }
  // Serial.println("Conectado ao WiFi");
  connected = true;

  // Inicie o servidor
  server.begin();

  xTaskCreate([](void*) {
    for (;;) {
      lerSensoresTensao();
      if (isMoving) {
        enviarDados();
      }
      vTaskDelay(500 / portTICK_PERIOD_MS);
    }
  }, "EnviarDadosTask", 4096, NULL, 1, NULL);
}

void loop() {
  // Leitura dos sensores infravermelhos
  int sensorMeio = analogRead(pinSensorMeio);
  int sensorEsq = analogRead(pinSensorEsq);
  int sensorDir = analogRead(pinSensorDir);

  // LEITURA DOS ENCODERS
  // if (millis() - previousMillis >= interval) {
  //   calculateSpeed();
  //   previousMillis = millis();
  // }

  if (sensorMeio > limitePreto && sensorEsq <= limitePreto && sensorDir <= limitePreto) {
    // Linha reta - ambos os motores com velocidade base
    // moverFrente(velocidadeBase, velocidadeBase);
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
    // moverFrente(0, velocidadeMaxima);
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
    // moverFrente(velocidadeMaxima, 0);
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
    // moverFrente(velocidadeBase, 0);
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
    // Virar à direita - reduzir velocidade do motor esquerdo
    // moverFrente(0, velocidadeBase);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(EN_A, velocidadeBase - ajusteVelocidade);

    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(EN_B, velocidadeBase);
    isMoving = true;
    naoDetectadoCounter = 0; // Resetar contador
  }
  else if (sensorMeio > limitePreto && sensorEsq > limitePreto && sensorDir > limitePreto) {
    // Fim do percurso - parar o carrinho
    pararMotores();
    isMoving = false;
    naoDetectadoCounter = 0; // Resetar contador
    enviarDados(); // Enviar dados de parada
  }
  else {
    // Não detectado - Continuar andando com velocidade reduzida
    moverFrente(velocidadeBase - ajusteVelocidade, velocidadeBase - ajusteVelocidade);
    isMoving = true;
    naoDetectadoCounter++;
    if (naoDetectadoCounter >= naoDetectadoLimite) {
      pararMotores(); // Parar os motores se o limite for alcançado
      isMoving = false;
      enviarFalha();
    }
  }

}

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
