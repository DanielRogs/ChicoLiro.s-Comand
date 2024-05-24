#define pinSensor 13 // Deve estar de acordo com o pino de output

void setup() {
  pinMode(pinSensor, INPUT);
  Serial.begin(9600); // Configuração da taxa de comunicação em bits por segundo (baud rate)
}

void loop() {

  // if (digitalRead(pinSensor) == HIGH) {
  //   Serial.println("BLACK");
  // }
  // else{
  //   Serial.println("WHITE");
  // }
  // delay(200);


  int valorAnalogico = analogRead(pinSensor);
  Serial.print("Valor analógico:");
  Serial.println(valorAnalogico);
  delay(200);
}