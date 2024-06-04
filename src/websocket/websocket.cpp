#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <HTTPClient.h>

// Defina suas credenciais WiFi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Defina o URL do servidor para onde os dados serão enviados
const char* serverUrl = "http://chicoliro.xobengala.com.br/api/dados/receive-data";

// Crie uma instância do servidor
AsyncWebServer server(80);

// Variáveis para armazenar os dados dos sensores
volatile int rpmMotorDireito = 0;
volatile int rpmMotorEsquerdo = 0;
String flag = "Largada";

// Função Exemplo para ler os sensores e calcular as RPMs
void lerSensores() {
    // Substitua essas linhas com a lógica real para ler os sensores
    rpmMotorDireito = 100;
    rpmMotorEsquerdo = 100;
    flag = "Teste";
}

// Função para enviar os dados para a API REST
void enviarDados() {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    String jsonData = "{\"rpmMotorDir\": " + String(rpmMotorDireito) +
                      ", \"rpmMotorEsq\": " + String(rpmMotorEsquerdo) +
                      ", \"flag\": " + String(flag) + "}";

    int httpResponseCode = http.POST(jsonData);
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
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Conectando ao WiFi...");
    }
    Serial.println("Conectado ao WiFi");

    // Inicie o servidor
    server.begin();

    // Configure o temporizador para enviar dados a cada 1 segundo
    xTaskCreate([](void*){
        for (;;) {
            lerSensores();
            enviarDados();
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }, "EnviarDadosTask", 4096, NULL, 1, NULL);
}

// Loop principal
void loop() {
    // Nada a fazer aqui, tudo está sendo feito na tarefa
}
