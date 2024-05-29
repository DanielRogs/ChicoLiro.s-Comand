#include <Arduino.h>
#include "motor.h"  

void setupComando() {
    Serial.begin(9600);
    setupMotor();
}

void loopComando() {
    loopMotor();

    // Desliga o motor após 50 segundos para o teste
    if (millis() > 50000) {
        desligarMotor();
    }

}
