/*
 WiFi Web Server LED Blink

 A simple web server that lets you blink an LED via the web.
 This sketch will print the IP address of your WiFi Shield (once connected)
 to the Serial monitor. From there, you can open that address in a web browser
 to turn on and off the LED on pin 5.

 If the IP address of your shield is yourAddress:
 http://yourAddress/H turns the LED on
 http://yourAddress/L turns it off

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the Wifi.begin() call accordingly.

 Circuit:
 * WiFi shield attached
 * LED attached to pin 5

 created for arduino 25 Nov 2012
 by Tom Igoe

ported for sparkfun esp32 
31.01.2017 by Jan Hendrik Berlin
 
 */

#include <WiFi.h>

#define CONNECTION_TIMEOUT 10

const char* ssid     = "teotonio_2.4G_ext";
const char* password = "teotonio382";

WiFiServer server(80);

void serialAtiva();

void setup()
{
    Serial.begin(115200);
    pinMode(22, OUTPUT); //motor
    pinMode(23, OUTPUT); //motor
    pinMode(18, OUTPUT); //bomba
    pinMode(19, OUTPUT); //bomba

    delay(10);

    // We start by connecting to a WiFi network

    Serial.println();
    Serial.print("Connecting to ");
    int timeout_counter = 0;
    Serial.println(ssid);

    delay(1000);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
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

int value = 0;

void loop(){
  WiFiClient client = server.available();   // listen for incoming clients

  serialAtiva();

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("Click <a href=\"/LM\">here</a> to turn the LED on pin 5 on.<br>");
            client.print("Click <a href=\"/DM\">here</a> to turn the LED on pin 5 off.<br>");
            client.print("Click <a href=\"/LB\">here</a> to turn the LED on pin 5 on.<br>");
            client.print("Click <a href=\"/DB\">here</a> to turn the LED on pin 5 off.<br>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

  
        
        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /LM")) {   //ativa motor
          digitalWrite(22, HIGH);
          digitalWrite(23, LOW);
        }
        if (currentLine.endsWith("GET /DM")) {   //desativa motor
          digitalWrite(23, LOW);
          digitalWrite(22, LOW);                
        }
        if (currentLine.endsWith("GET /LB")) {   //ativa motor
          digitalWrite(18, HIGH);
          digitalWrite(19, LOW);
        }
        if (currentLine.endsWith("GET /DB")) {   //desativa motor
          digitalWrite(18, LOW);
          digitalWrite(19, LOW);                
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}

void serialAtiva(){
    if(Serial.available()){
      char letra = Serial.read();

      Serial.print(letra);

      if (letra == 'M') {   //ativa motor
          digitalWrite(22, HIGH);
          digitalWrite(23, LOW);
      }
      if (letra == 'D') {   //desativa motor
        digitalWrite(23, LOW);
        digitalWrite(22, LOW);                
      }
      if (letra == 'b') {   //ativa motor
        digitalWrite(18, HIGH);
        digitalWrite(19, LOW);
      }
      if (letra == 'd') {   //desativa motor
        digitalWrite(18, LOW);
        digitalWrite(19, LOW);                
      }
  }
}
