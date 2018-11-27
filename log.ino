int nRainIn = A1;
int nRainDigitalIn = 2;
int nRainVal;
boolean bIsRaining = false;
String strRaining;

#include "DHT.h"

#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
 
#define DHTPIN A2 // pino que estamos conectado
#define DHTTYPE DHT11 // DHT 11
DHT dht(DHTPIN, DHTTYPE);
char server[] = "sensorjardimapi.herokuapp.com";    // name address for Google (using DNS)
// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 0, 177);
IPAddress myDns(192, 168, 0, 1);
// Variables to measure the speed
unsigned long beginMicros, endMicros;
unsigned long byteCount = 0;
bool printWebData = true;  // set to false for better speed measurement

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;
int sensor = 0;      //Pino analógico em que o sensor está conectado.
int valorSensor = 0; //Usada para ler o valor do sensor em tempo real.

void setup() {
  Serial.begin(9600);
  pinMode(2,INPUT);
  dht.begin();
}
void loop() {
  nRainVal = analogRead(nRainIn);
  bIsRaining = !(digitalRead(nRainDigitalIn));
  
  if(bIsRaining){
    strRaining = "YES";
  }
  else{
    strRaining = "NO";
  }
  
  Serial.print("Raining?: ");
  Serial.print(strRaining);  
  Serial.print("\t Moisture Level: ");
  Serial.println(nRainVal);
  conectaWs(String(nRainVal), "chuva");
    
  delay(200);
  ////////////////////////////////
// A leitura da temperatura e umidade pode levar 250ms!
  // O atraso do sensor pode chegar a 2 segundos.
  // testa se retorno é valido, caso contrário algo está errado.
  
  ////////////////////////////////
  
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  // testa se retorno é valido, caso contrário algo está errado.
  if (isnan(t) || isnan(h)) 
  {
    Serial.println("Failed to read from DHT");
  } 
  else
  {
    Serial.print("Umidade: ");
    Serial.print(h);
    Serial.print(" %t");
    Serial.print("Temperatura: ");
    Serial.print(t);
    Serial.println(" *C");
    conectaWs(String(h), "umidade");  
    delay(2000);  
    conectaWs(String(t), "temperatura");

  }



  ///////////////
  int valorSensor = analogRead(sensor);
  //Luminosidade baixa.
  if (valorSensor < 750) {
    Serial.println("luminosidade baixa");
  }
   
  //Luminosidade média.
  if (valorSensor >= 750 && valorSensor <= 800) {
    Serial.println("luminosidade média");
  }
   
  //Luminosidade alta.
  if (valorSensor > 800) {
    Serial.println("luminosidade alta");
  }
   
  //Exibindo o valor do sensor no serial monitor.
  //Serial.println(valorSensor);
  conectaWs(String(valorSensor), "luminosidade");
  delay(2000);
}

void conectaWs(String valor, String tipo){

// start the Ethernet connection:
  //Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, myDns);
  } else {
    //Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.print("connecting to ");
  Serial.print(server);
  Serial.println("...");

  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.print("connected to ");
    // Make a HTTP request:
    client.println("POST /leitura?tipo=" + tipo + "&valor=" + valor+ " HTTP/1.1");
    client.println("Host: sensorjardimapi.herokuapp.com");
    client.println("Connection: close");
    client.println("Content-Type: application/x-www-form-urlencoded;");
    client.print("Content-Length: ");
    client.println(2);
    client.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
  beginMicros = micros();

  int len = client.available();
  if (len > 0) {
    byte buffer[80];
    if (len > 80) len = 80;
    client.read(buffer, len);
    if (printWebData) {
      //Serial.write(buffer, len); // show in the serial monitor (slows some boards)
    }
    byteCount = byteCount + len;
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    endMicros = micros();
    //Serial.println();
    //Serial.println("disconnecting.");
    client.stop();
    //Serial.print("Received ");
    //Serial.print(byteCount);
    //Serial.print(" bytes in ");
    //float seconds = (float)(endMicros - beginMicros) / 1000000.0;
    //Serial.print(seconds, 4);
    //float rate = (float)byteCount / seconds / 1000.0;
    //Serial.print(", rate = ");
    //Serial.print(rate);
    //Serial.print(" kbytes/second");
    //Serial.println();

    // do nothing forevermore:
    /*while (true) {
      delay(1);
      Serial.print(" aqui");
    }*/
  }

}



