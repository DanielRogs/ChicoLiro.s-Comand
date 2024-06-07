#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <HTTPClient.h>
#include "config.h"

// Definindo as credenciais WiFi
const char* ssid = "UNB Wireless";

// Definindo o URL do servidor para onde os dados serão enviados
const char* serverUrl = "https://chicoliro.xobengala.com.br/api/dados/receive-data";

// Criando uma instância do servidor
AsyncWebServer server(80);

// Variáveis para armazenar os dados dos sensores
volatile int rpmMotorDireito = 0;
volatile int rpmMotorEsquerdo = 0;
volatile int tensao = 0;
volatile bool isMoving = false;

// Função Exemplo para ler os sensores e calcular as RPMs
void lerSensores() {
    rpmMotorDireito = 100;
    rpmMotorEsquerdo = 100;
    isMoving = true;
    tensao = 9;
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
    WiFi.begin(ssid, wifi_username, wifi_password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Conectando ao WiFi...");
    }
    Serial.println("Conectado ao WiFi");

    // Inicie o servidor
    server.begin();

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
    // Nada a fazer aqui, tudo está sendo feito na tarefa
}
