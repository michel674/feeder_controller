#include <Arduino.h>
#include "HX711.h"
#include <WiFi.h>

HX711 scale;

TaskHandle_t tarefa1;
TaskHandle_t tarefa2;
TaskHandle_t tarefa3;
TaskHandle_t tarefa4;

float offset = 0; // variável para guardar o valor bruto de offset
float kg = 0;
int distancia;
int value = 0;
int trig = 12; // Pins
int echo = 13;
int dt = 18;
int sck = 17;
#define CONNECTION_TIMEOUT 10
const char *ssid = "teotonio_2.4G_ext";
const char *password = "teotonio382";
String currentLine = ""; // make a String to hold incoming data from the client
int MaxKg = 100;         // Maximum weight of food
int nvlMax = 15;         // Level maximum of water

WiFiServer server(80);

void serialAtiva();
void connectWifi();
void sensorUS(void *pvParameters);
void sensorW(void *pvParameters);
void connection(void *pvParameters);
void action(void *pvParameters);

void setup()
{
  Serial.begin(115200);

  pinMode(22, OUTPUT); // motor
  pinMode(23, OUTPUT); // motor
  pinMode(18, OUTPUT); // bomba
  pinMode(19, OUTPUT); // bomba
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  connectWifi();

  scale.begin(dt, sck);
  offset = scale.read_average(100);

  xTaskCreatePinnedToCore(sensorUS, "tarefa1", 10000, NULL, 0, &tarefa1, 0);
  xTaskCreatePinnedToCore(sensorW, "tarefa2", 10000, NULL, 1, &tarefa2, 1);
  xTaskCreatePinnedToCore(connection, "tarefa3", 10000, NULL, 0, &tarefa3, 1);
}

void loop()
{
  if (Serial.available() > 0)
  {
    char c = Serial.read();
    if (c == 'a')
    {
      vTaskSuspend(tarefa1);
    }
  }
}

void serialAtiva()
{
  if (Serial.available())
  {
    char letra = Serial.read();

    Serial.print(letra);

    if (letra == 'M')
    { // ativa motor
      digitalWrite(22, HIGH);
      digitalWrite(23, LOW);
    }
    if (letra == 'D')
    { // desativa motor
      digitalWrite(23, LOW);
      digitalWrite(22, LOW);
    }
    if (letra == 'b')
    { // ativa motor
      digitalWrite(18, HIGH);
      digitalWrite(19, LOW);
    }
    if (letra == 'd')
    { // desativa motor
      digitalWrite(18, LOW);
      digitalWrite(19, LOW);
    }
  }
}

void connectWifi()
{
  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.print("Connecting to ");
  int timeout_counter = 0;
  Serial.println(ssid);

  delay(1000);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");

    Serial.println(WiFi.status());

    timeout_counter++;
    Serial.print(timeout_counter);
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void sensorUS(void *pvParameters)
{
  unsigned long tempSom;
  for (;;)
  {
    digitalWrite(trig, LOW);
    delayMicroseconds(2);
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);
    tempSom = pulseIn(echo, HIGH);
    distancia = tempSom / 58;
    Serial.print("Distancia: ");
    Serial.print(distancia);
    Serial.println(" cm");

    delay(300);
  }
}

void sensorW(void *pvParameters)
{
  for (;;)
  {
    // subtrai o offset
    kg = scale.read_average(3) - offset;
    // se conectar ao contrário, o valor será negativo. Pode ser invertido por software, desse modo.
    kg /= 100;
    // o peso está divido por 4 células, significando que podemos pesar até 200kg com 4 células de 50kg.
    kg *= 2;
    // está dando uma diferença pequena, mas uma função de tara resolve.
    kg = kg - 2; // 16 = 4*4

    Serial.print(kg);
    Serial.println("gr");
    delay(300);
  }
}

void connection(void *pvParameters)
{
  for (;;)
  {
    WiFiClient client = server.available(); // listen for incoming clients

    serialAtiva();

    if (client)
    {                                // if you get a client,
      Serial.println("New Client."); // print a message out the serial port
      while (client.connected())
      { // loop while the client's connected
        if (client.available())
        {                         // if there's bytes to read from the client,
          char c = client.read(); // read a byte, then
          Serial.write(c);        // print it out the serial monitor
          if (c == '\n')
          { // if the byte is a newline character

            // if the current line is blank, you got two newline characters in a row.
            // that's the end of the client HTTP request, so send a response:
            if (currentLine.length() == 0)
            {
              // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
              // and a content-type so the client knows what's coming, then a blank line:
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println();

              // the content of the HTTP response follows the header:
              client.print("Click <a href=\"/LM\">here</a> to turn the engine on.<br>");
              client.print("Click <a href=\"/DM\">here</a> to turn the engine off.<br>");
              client.print("Click <a href=\"/LB\">here</a> to turn the pump on.<br>");
              client.print("Click <a href=\"/DB\">here</a> to turn the pump off.<br>");

              // The HTTP response ends with another blank line:
              client.println();
              // break out of the while loop:
              break;
            }
            else
            { // if you got a newline, then clear currentLine:
              currentLine = "";
            }
          }
          else if (c != '\r')
          {                   // if you got anything else but a carriage return character,
            currentLine += c; // add it to the end of the currentLine
          }

          xTaskCreatePinnedToCore(action, "tarefa4", 10000, NULL, 1, &tarefa4, 0);
        }
      }
      // close the connection:
      client.stop();
      Serial.println("Client Disconnected.");
    }
  }
}

void action(void *pvParameters)
{
  for (;;)
  {
    // Check to see if the client request was "GET /H" or "GET /L":
    if (currentLine.endsWith("GET /LM"))
    { // engine on
      if (kg < MaxKg)
      {
        digitalWrite(22, HIGH);
        digitalWrite(23, LOW);
        while (kg < MaxKg)
        {
          delay(200);
        }
        digitalWrite(22, LOW);
      }
    }
    if (currentLine.endsWith("GET /DM"))
    { // engine off
      digitalWrite(23, LOW);
      digitalWrite(22, LOW);
    }
    if (currentLine.endsWith("GET /LB"))
    { // pump on
      if (distancia < nvlMax)
      {
        digitalWrite(18, HIGH);
        digitalWrite(19, LOW);
        while (distancia < nvlMax)
        {
          delay(200);
        }
        digitalWrite(18, LOW);
      }
    }
    if (currentLine.endsWith("GET /DB"))
    { // pump off
      digitalWrite(18, LOW);
      digitalWrite(19, LOW);
    }
  }
}
