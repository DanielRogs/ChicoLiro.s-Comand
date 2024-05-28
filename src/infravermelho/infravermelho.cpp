#define pinSensor 15 // Deve estar de acordo com o pino de output

void setup() {
  pinMode(pinSensor, INPUT);
  // Configuração da taxa de comunicação em bits por segundo (baud rate)
  Serial.begin(115200); //Se for omitido esse parâmetro o padrão será 8 bits, sem paridade e 1 stop bit.
}

void loop() {

  if (digitalRead(pinSensor) == HIGH) {
    Serial.println("Está na fita"); // Pode testar falso positivo se estiver longe de uma superfície.
  }
  else{
    Serial.println("Não está na fita");
  }
  delay(200);


  // int valorAnalogico = analogRead(pinSensor);
  // Serial.print("Valor analógico:");
  // Serial.println(valorAnalogico);
  // delay(200);
}