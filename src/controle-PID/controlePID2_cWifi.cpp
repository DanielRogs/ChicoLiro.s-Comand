/*

  Título: Código de PID da equipe de Software.
  Autores: Daniel Rodrigues, Leandro Almeida e Luiz Amaral.
  Descrição: Código para integrar todo o sistema controlador do carro.
  Data de criação: 11/06/2024

*/

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <HTTPClient.h>

// Definição de pinos dos sensores, encoders e motores
const int pinSensorEsq = 15;
const int pinSensorMeio = 2;
const int pinSensorDir = 4;

const int pinEncoderEsq = 0;  // VERIFICAR PINAGEM
const int pinEncoderDir = 0;  // VERIFICAR PINAGEM

const int pinMotorEsq = 0;   // VERIFICAR PINAGEM
const int pinMotorDir = 0;  // VERIFICAR PINAGEM

// Definindo as credenciais WiFi
const char* ssid = "UNB Wireless";
const char* wifi_password = "SenhaAqui";

// Definindo o URL do servidor para onde os dados serão enviados
const char* serverUrl = "https://chicoliro.xobengala.com.br/api/dados/receive-data";

// Criando uma instância do servidor
AsyncWebServer server(80);

// Variáveis para armazenar os dados dos sensores
volatile int rpmMotorDireito = 0;
volatile int rpmMotorEsquerdo = 0;
volatile int tensao = 0;
volatile bool isMoving = false;

// Variáveis para leitura dos sensores
int sensorEsq;
int sensorMeio;
int sensorDir;

volatile int encoderEsq = 0;
volatile int encoderDir = 0;

// Variáveis para controle PID
float erro;
float proporcional;
float integral;
float derivativo;
float erroAnterior = 0;
float Kp = 1.0;
float Ki = 0.5;
float Kd = 0.1;
float PID;

// Funções
int lerSensor(int sensorPin) {
  return analogRead(sensorPin);
}

float calcularErro(int sensorEsq, int sensorMeio, int sensorDir) {
  const int leituraLinha = 3000; // Necessário calibragem
  
  if (sensorEsq < leituraLinha && sensorMeio >= leituraLinha && sensorDir >= leituraLinha) {
        return -2; // Vira à esquerda
    } else if (sensorEsq < leituraLinha && sensorMeio < leituraLinha && sensorDir >= leituraLinha) {
        return -1; // Esquerda-meio
    } else if (sensorEsq >= leituraLinha && sensorMeio < leituraLinha && sensorDir >= leituraLinha) {
        return 0.0;  // Centralizado
    } else if (sensorEsq >= leituraLinha && sensorMeio >= leituraLinha && sensorDir < leituraLinha) {
        return 1;  // Direita-meio
    } else if (sensorEsq >= leituraLinha && sensorMeio >= leituraLinha && sensorDir < leituraLinha) {
        return 2;  // Vira à direita
    } else {
        return 0.0;  // Caso padrão (sem linha detectada ou linha detectada em todos os sensores)
  }
}

void controlarMotor(int PID) {
  int velocidadeBase = 100;  // Velocidade base dos motores (ajustável)
  int velocidadeMax = 200;   // Valor máximo para PWM (200 é o máximo)

  // Cálculo da velocidade dos motores ajustando pelo PID
  int velocidadeEsq = velocidadeBase + PID;
  int velocidadeDir = velocidadeBase - PID;

  // Garantir que a velocidade esteja dentro do limite seguro
  velocidadeEsq = constrain(velocidadeEsq, 0, velocidadeMax);
  velocidadeDir = constrain(velocidadeDir, 0, velocidadeMax);

  analogWrite(pinMotorEsq, velocidadeEsq);
  analogWrite(pinMotorDir, velocidadeDir);
}

void controlePID() {
  sensorEsq = lerSensor(pinSensorEsq);
  sensorMeio = lerSensor(pinSensorMeio);
  sensorDir = lerSensor(pinSensorDir);

  erro = calcularErro(sensorEsq, sensorMeio, sensorDir);

  if (erro == 0){
    integral = 0;
  }

  proporcional = erro;
  integral += erro;

  // ajustar condicional conforme limite superior e inferior do PWM
  if (integral > 120){ 
    integral = 120;
  } else if (integral < -120) {
    integral = -120;
  }

  derivativo = erro - erroAnterior;

  PID = (Kp*proporcional) + (Ki*integral) + (Kd*derivativo);
  erroAnterior = erro;

  controlarMotor(PID);
}

void encoderEsqISR() {
  encoderEsq++;
}

void encoderDirISR() {
  encoderDir++;
}

void lerSensores() {
    // Aqui você pode atualizar as variáveis rpmMotorDireito, rpmMotorEsquerdo e tensao com os dados reais dos sensores
    rpmMotorDireito = encoderDir;
    rpmMotorEsquerdo = encoderEsq;
    isMoving = true;
    // tensao = analogRead(A0);
    tensao = 9;
}

void enviarDados() {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");
    
    String jsonData = "{\"rpmMotorDir\": " + String(rpmMotorDireito) +
                      ", \"rpmMotorEsq\": " + String(rpmMotorEsquerdo) +
                      ", \"tensao\": " + String(tensao) +
                      ", \"isMoving\": " + String(isMoving) + "}"; 

    int httpResponseCode = http.POST(jsonData);
    Serial.println(jsonData);
    Serial.println(httpResponseCode);

    if (httpResponseCode >= 200 && httpResponseCode < 400) {
        Serial.println("Dados enviados com sucesso");
    } else {
        Serial.println("Falha ao enviar dados");
    }

    http.end();
}

// Configuração inicial
void setup() {
    Serial.begin(115200);

    // Conecte-se ao WiFi
    WiFi.begin(ssid, wifi_password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Conectando ao WiFi...");
    }
    Serial.println("Conectado ao WiFi");

    // Inicie o servidor
    server.begin();

    pinMode(pinSensorEsq, INPUT);
    pinMode(pinSensorMeio, INPUT);
    pinMode(pinSensorDir, INPUT);
    pinMode(pinEncoderEsq, INPUT);
    pinMode(pinEncoderDir, INPUT);
    pinMode(pinMotorEsq, OUTPUT);
    pinMode(pinMotorDir, OUTPUT);

    attachInterrupt(digitalPinToInterrupt(pinEncoderEsq), encoderEsqISR, RISING);
    attachInterrupt(digitalPinToInterrupt(pinEncoderDir), encoderDirISR, RISING);

    xTaskCreate([](void*){
        for (;;) {
            lerSensores();
            enviarDados();
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }
    }, "EnviarDadosTask", 4096, NULL, 1, NULL);
}

// Loop principal
void loop() {
    controlePID();
}
