#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>

// Declaração dos pinos do driver de motor L298N
extern const int motorPin1;
extern const int motorPin2;
extern const int enablePin;

// Declaração do pino do sensor de velocidade (encoder)
extern const int sensorPin;

// Declaração das variáveis de controle de velocidade e RPM
extern int rpm;
extern volatile byte pulsos;
extern unsigned long timeold;
extern unsigned int pulsos_por_volta;

// Declaração das funções
void ligarMotor(int velocidade);
void desligarMotor();
void contador();
void setupMotor();
void loopMotor();

#endif
