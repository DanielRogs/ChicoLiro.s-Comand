#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>

// Configurações da rede Wi-Fi
const char* ssid = "SEU_SSID"; // Nome da rede
const char* password = "SUA_SENHA"; // Senha da rede

// Criação do servidor e do WebSocket
AsyncWebServer server(80); // Porta para o WS
AsyncWebSocket ws("/ws");

int RPM = 100; // Exemplo de RPM

// Função para enviar dados dos sensores
void sendSensorData() {
    String sensorData = "{\"RPM\": %d}", RPM; // Exemplo de dados dos sensores
    ws.textAll(sensorData);
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.printf("Cliente conectado %u\n", client->id());
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.printf("Cliente desconectado %u\n", client->id());
  } else if (type == WS_EVT_DATA) {
    // Tratar dados recebidos do cliente, se necessário
  }
}

void setup() {
  // Inicialização serial
  Serial.begin(115200);

  // Conexão à rede Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando à rede Wi-Fi...");
  }
  Serial.println("Conectado à rede Wi-Fi");

  // Configuração do WebSocket
  ws.onEvent(onEvent);
  server.addHandler(&ws);

  // Inicia o servidor
  server.begin();

  // Exemplo de envio periódico de dados dos sensores
  TimerHandle_t sensorDataTimer = xTimerCreate(
    "SensorDataTimer",
    pdMS_TO_TICKS(1000), // Enviar dados a cada 1 segundo
    pdTRUE,
    (void*)0,
    [](TimerHandle_t xTimer) {
      sendSensorData();
    }
  );
  xTimerStart(sensorDataTimer, 0);
}

void loop() {
  // O loop pode estar vazio, pois o envio de dados é tratado pelo Timer
}
