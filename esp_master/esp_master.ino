#include <Arduino_JSON.h>
#include <Wire.h>
#include <SocketIoClient.h>
#include <WebSockets2_Generic.h>
#include <WebSocketsServer.h>
#include <WebSockets.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>

#define USER_SERIAL Serial


const char* ssid = "FIT";
const char* pass = "fit@vimaru";
const char* host ="172.26.0.40";

int port = 3000;
char * sts="q";
SocketIoClient webSocket;



void setup() {
	
	USER_SERIAL.begin(9600);

	searchWiFi();
	connectWiFi();

  //ket noi esp voi server qua socket
	webSocket.begin(host,port);
	Wire.begin(D1, D2);

	webSocket.on("status-change", handlerOn);

}

void loop() {
	webSocket.loop();

	Wire.beginTransmission(8); /* begin with device address 8 */
	Wire.write('j');
	Wire.endTransmission();

	delay(200);
}

void handlerOn(const char * payload, size_t length){

	USER_SERIAL.println((String)payload);

	JSONVar st = JSON.parse((String)payload);
	
	if (JSON.typeof(st) == "undefined") {
		Serial.println("Parsing input failed!");
		return;
	}

	JSONVar keys = st.keys();

	for (int i = 0; i < keys.length(); i++) {
		JSONVar value = st[keys[i]];
		int temp=atoi(value);
		if(temp==1)
		{
			sts="1";
		}else if(temp==0)
		{
			sts="0";
		}else if(temp==3)
		{
			sts="3";
		}
		else if(temp == 4) {
			sts="4";
		}
		
	}

	Wire.beginTransmission(8); /* begin with device address 8 */
	Wire.write(sts);
	Wire.endTransmission();
	
}

void searchWiFi(){
	int numberOfNetwork = WiFi.scanNetworks();
	USER_SERIAL.println("----");

	for(int i = 0; i < numberOfNetwork; i++ ){
		USER_SERIAL.print("Network name: ");
		USER_SERIAL.println(WiFi.SSID(i));
		USER_SERIAL.print("Signal strength: ");
		USER_SERIAL.println(WiFi.RSSI(i));
		USER_SERIAL.println("--------------");
	}
}


void connectWiFi(){
	WiFi.begin(ssid, pass);
	while(WiFi.status() != WL_CONNECTED){
		USER_SERIAL.print(".");
		delay(1000);
	}

	USER_SERIAL.print("");
	USER_SERIAL.println("WiFi connected");
	USER_SERIAL.print("IP Address : ");
	USER_SERIAL.println(WiFi.localIP());
}
