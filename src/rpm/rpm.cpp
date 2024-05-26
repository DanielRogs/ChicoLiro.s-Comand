#include <Arduino.h>

// -- Variáveis Globais --

int rpm;
volatile byte pulsos;
unsigned long timeold;

// Alterar o número de acordo com o disco do encoder 
unsigned int pulsos_por_volta = 13;

// -- Interrupção --

void contador ()
{
    // Incrementa o contador de pulsos
    pulsos++;
}

// -- Configurações iniciais --

void setup()
{
    Serial.begin(115200); // Inicializa a comunicação serial com a taxa de transmissão de 115200 bps
    
    // Configura a interrupção para ocorrer no pino digital 2 (GPIO2 no ESP32)
    // Aciona a função contador() a cada borda de subida (RISING)
    attachInterrupt(digitalPinToInterrupt(2), contador, RISING);
    
    pulsos = 0; // Inicializa a contagem de pulsos
    rpm = 0; // Inicializa a variável de rotação por minuto
    timeold = millis(); // Inicializa o tempo antigo com o tempo atual
}

// -- Loop infinito --

void loop()
{
    // Simula o tempo decorrido desde a última atualização
    unsigned long tempo_atual = millis();
    unsigned long tempo_decorrido = tempo_atual - timeold;
    
    // Simula a geração de pulsos a uma taxa constante
    if (tempo_decorrido >= 1000) {
        // Calcula a quantidade de pulsos esperados neste intervalo
        unsigned int pulsos_esperados = (pulsos_por_volta * tempo_decorrido) / 60000; // 60000 ms = 1 minuto
        
        // Calcula as rotações por minuto (RPM)
        rpm = (60 * pulsos) / (pulsos_por_volta * (tempo_decorrido / 1000)); // Tempo em segundos
        
        // Zera o contador de pulsos
        pulsos = 0;
        
        // Atualiza o tempo antigo
        timeold = tempo_atual;
        
        // Mostra o valor de RPM no monitor serial
        Serial.print("RPM = ");
        Serial.println(rpm, DEC);
    }
}
