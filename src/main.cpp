#include <Arduino.h>
#include "motor/motor.h"  

void setup() {
    Serial.begin(9600);
    setupMotor();
}

void loop() {
    loopMotor();

    // Desliga o motor após 50 segundos para o teste
    if (millis() > 50000) {
        desligarMotor();
    }

}
