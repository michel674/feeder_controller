TaskHandle_t tarefa1;

int led = 5;
int trig = 4;
int echo = 2;

void setup() {
  Serial.begin(115200);

  pinMode(led, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  xTaskCreatePinnedToCore(sensorHC, "tarefa1", 10000, NULL, 0, &tarefa1, 1);

}

void loop() {
 if (Serial.available() > 0)  {
  char c = Serial.read();
  if (c == 'a') {
    vTaskSuspend(tarefa1);
  }
 }
}

void sensorHC(void *pvParameters) {
  for(;;) {
    int distancia;
    digitalWrite(trig, LOW);
    delayMicroseconds(2);
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);
    unsigned long tempSom = pulseIn(echo, HIGH);
    distancia = tempSom/58;

    Serial.print("Distancia: ");
    Serial.print(distancia);
    Serial.println(" cm");

    delay(1000);
    
  }
}
