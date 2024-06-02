#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsClient.h>

// Defina as credenciais do WiFi
char* ssid = "Wokwi-GUEST";
char* password = "";

// Defina o endereço do servidor WebSocket e a porta
const char* websocket_server = "chicoliro.xobengala.com.br";
const uint16_t websocket_port = 3005; // Ajuste para a porta do seu servidor
const char* websocket_path = "/socket.io/"; // Path para o Socket.IO

// Instancie o cliente WebSocket
WebSocketsClient webSocket;

// Função de callback para eventos do WebSocket
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            Serial.println("WebSocket Disconnected");
            break;
        case WStype_CONNECTED:
            Serial.println("WebSocket Connected");
            // Enviar uma mensagem ao conectar
            webSocket.sendTXT("{"event":"receive-data", "data":"Hello Server"}");
            break;
        case WStype_TEXT:
            Serial.printf("Received Text: %s\n", payload);
            break;
        case WStype_BIN:
            Serial.println("Received Binary");
            break;
    }
}
void setup() {
    Serial.begin(115200);

    // Conectar ao WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("WiFi Connected");

    // Configurar o WebSocket

    // String url = String("https://"/) + websocket_server + ":" + websocket_port + websocket_path;
    webSocket.begin(websocket_server, websocket_port, websocket_path);
    webSocket.onEvent(webSocketEvent);
}

void loop() {
    // Chamar o loop do WebSocket para manter a conexão
    webSocket.loop();

    // Exemplo de envio de dados periodicamente
    static unsigned long lastTime = 0;
    if (millis() - lastTime > 2000) { // A cada 2 segundos
        lastTime = millis();
        webSocket.sendTXT("{"event":"receive-data", "data":"Hello from ESP32"}");
    }
}