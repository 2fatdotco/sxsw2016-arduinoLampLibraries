/*
 * WebSocketServer.ino
 *
 *  Created on: 22.05.2015
 *
 */

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsServer.h>
#include <Hash.h>
#include <ArduinoOTA.h>

ESP8266WiFiMulti WiFiMulti;

WebSocketsServer webSocket = WebSocketsServer(81);

#define USE_SERIAL Serial1

int led_pin = 13;
int level = 50;

void blink() {
  digitalWrite(LED_BUILTIN, LOW);
  delay(200);
  digitalWrite(LED_BUILTIN, HIGH);
}


void stepLED(){
  if (level >= 900){
    level = 0;
  }
  else {
    level = level + 50;
  }
  analogWrite(led_pin,level);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {

    switch(type) {
        case WStype_DISCONNECTED:
            USE_SERIAL.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED:
            {
                IPAddress ip = webSocket.remoteIP(num);
                USE_SERIAL.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
				
				    // send message to client
		    		webSocket.sendTXT(num, "Youre connected");
            stepLED();

            }
            break;
        case WStype_TEXT:
            USE_SERIAL.printf("[%u] get Text: %s\n", num, payload);

            // send message to client
            stepLED();
             webSocket.sendTXT(num, ("level:"+String(level)));

            // send data to all connected clients
            // webSocket.broadcastTXT("message here");
            break;
        case WStype_BIN:
            USE_SERIAL.printf("[%u] get binary lenght: %u\n", num, lenght);
            hexdump(payload, lenght);

            // send message to client
            // webSocket.sendBIN(num, payload, lenght);
            break;
    }

}


void setup() {

    
    USE_SERIAL.begin(9600);

    analogWriteRange(1000);

    //Serial.setDebugOutput(true);
    USE_SERIAL.setDebugOutput(true);

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    WiFiMulti.addAP("myNetworkName", "myWifiPassword");

    while(WiFiMulti.run() != WL_CONNECTED) {
        delay(100);
    }

    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
}

void loop() {
    USE_SERIAL.println("Lets do this!\n");
    webSocket.loop();
}

