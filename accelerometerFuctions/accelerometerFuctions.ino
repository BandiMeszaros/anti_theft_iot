const int value_threshold = 200;
const int xpin = 23; // x-axis of the accelerometer
const int ypin = 24; // y-axis
const int zpin = 25; // z-axis (only on 3-axis models)
const int redLED= 39; // assign red LED to Pin #9
const int greenLED= 38;// assign green LED to Pin #10
const int blueLED= 37; // assign blueLED to Pin #12
static int xpin_rest = 0;
static int ypin_rest = 0;
static int zpin_rest = 0;
static int measureResults[3];
static int diffResults[3];
int buzzerPin = 40;


bool isActive = true;
bool isMoving = false;


#ifndef __CC3200R1M1RGC__
// Do not include SPI for CC3200 LaunchPad
#include <SPI.h>
#endif
#include <WiFi.h>

#include <string.h>

// Core library for code-sense
#include "Energia.h"

// Include application, user and local libraries
#include "SPI.h"

// Screen selection
#define HX8353E // HX8353E K35_SPI

#include "Screen_HX8353E.h"
Screen_HX8353E myScreen;


void activatedText(){
  myScreen.gText(35,60,"Activated!");
}


void deactivatedText(){
  myScreen.gText(30,60,"Deactivated!");
}

// your network name also called SSID
char ssid[] = "apt-get";
// your network password
char password[] = "aspettando";

// Initialize the Wifi client library
WiFiClient client;

// server address:
IPAddress server(192,168,43,34);

void disarm(){
  if(isActive == true | isMoving == true){
    myScreen.clear();
    deactivatedText();
    digitalWrite(redLED, LOW);
    digitalWrite(blueLED, HIGH);
    digitalWrite(greenLED, LOW);
    Serial.print("DISARMED\n");
    noTone(buzzerPin);
  }
  isActive = false;
  isMoving = false;
}

void arm(){
  if(isActive==false){
    myScreen.clear();
    activatedText();
    digitalWrite(redLED, LOW);
    digitalWrite(blueLED, LOW);
    digitalWrite(greenLED, HIGH);
  }
  
  isActive = true;
}


void alarm(){
  if(isMoving == false){
    digitalWrite(redLED, HIGH);
    digitalWrite(greenLED, LOW);
    digitalWrite(blueLED, LOW);
    myScreen.clear();
    myScreen.gText(35,60,"Movement!");
    Serial.print("Aaaaaaaaaaaaalarm\n");
    tone(buzzerPin, 175);
  }
  
  isMoving = true;
}


void warningMovement(){
  //changes the LED color green-->red
  //sends a warning message to the computer
  digitalWrite(redLED, HIGH);
  digitalWrite(greenLED, LOW);
  Serial.print("\nWARNING, WARNING, movement detected\n");

  httpRequestMovement();
  alarm();
}


void measureAnalogValues()
{
  //Measures the analogValues of the x,y,z axis, and returns them in an array

  int analogValueX;// CHECK X AXIS (red)
  analogValueX = analogRead(xpin);// read X axis
  int analogValueY;
  analogValueY = analogRead(ypin); //read y axis
  int analogValueZ;
  analogValueZ = analogRead(zpin); // read Z axis
  measureResults[0] = analogValueX;
  measureResults[1] = analogValueY;
  measureResults[2] = analogValueZ;
}


void calculateChange(int analogVX, int analogVY, int analogVZ)
{
  //calculates the difference between the measured x,y,z values, and the resting position values

    int xDiff = abs(analogVX-xpin_rest);
    int yDiff = abs(analogVY-ypin_rest);
    int zDiff = abs(analogVZ-zpin_rest);

    diffResults[0] = xDiff;
    diffResults[1] = yDiff;
    diffResults[2] = zDiff;
}


int movementDetected(int axisDiffs[])
{
  //it checks if the detected accelerometer value is in the threshold

  for(int i=0; i<3;i++)
  {
    if (value_threshold-axisDiffs[i] < 0)
     {
       Serial.print("Movement detected on axis nr: ");
       Serial.print(i);
       Serial.print("\nvalue: \n");
       Serial.println(value_threshold-axisDiffs[i]);
       return 1;
     }
  }
  return 0;
}

void calculateRestValues()
{//calculates the rest values of the axis, use only at stratup(setup loop)

    int loopCounter = 0;
    int xpin_sum = 0;
    int ypin_sum = 0;
    int zpin_sum = 0;
    while(loopCounter <= 200000)
    { // 200000 is a randomly selected large number, for the loopCounter, so the rest average calculated with the minimum error
      int valueX = analogRead(xpin);
      int valueY = analogRead(ypin);
      int valueZ = analogRead(zpin);
      xpin_sum += valueX;
      ypin_sum +=  valueY;
      zpin_sum += valueZ;
      loopCounter += 1;
    }
    xpin_rest = xpin_sum/loopCounter;
    ypin_rest = ypin_sum/loopCounter;
    zpin_rest = zpin_sum/loopCounter;
}


void httpRequestMovement() {
  
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();

  // if there's a successful connection:
  if (client.connect(server, 8080)) {
    Serial.println("connecting...");
    // send the HTTP GET request:
    client.println("GET /BBS/MovementDetected HTTP/1.1");
    //change the IP address here
    client.println("Host: 192.168.43.34:8080");
    client.println("User-Agent: Energia/1.1");
    client.println("Connection: close");
    client.println();
    Serial.println("done");

  }
  else{
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }
}



void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}


void httpRequestPolling() {
  
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();

  // if there's a successful connection:
  if (client.connect(server, 8080)) {
    Serial.println("connecting...");
    // send the HTTP GET request:
    client.println("GET /BBS/State HTTP/1.1");
    //change the IP address here
    client.println("Host: 192.168.43.34:8080");
    client.println("User-Agent: Energia/1.1");  
    client.println("Connection: close");
    client.println();
    

  }
  else{
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }


  
  // first line of response: HTTP/1.1 200
  String line = client.readStringUntil('\n');


  //find the right line containing status value
  const char *riga = line.c_str();
  char *ret_status;
  char *ret_movement;
  ret_status = strstr(riga, "status");
  ret_movement = strstr(riga, "movement");
  char s = 'k';
  char m = 'k';

  while(s=='k' | m=='k'){
    if(ret_status){
      s = line[8];  
    }
    if(ret_movement){
      m = line[10];
    }
    line = client.readStringUntil('\n');
    riga = line.c_str();
    ret_status = strstr(riga, "status");
    ret_movement = strstr(riga, "movement");
    }
  Serial.println("--------------");
  Serial.print("Stato è: ");
  Serial.println(s);
  Serial.print("Movimento è: ");
  Serial.println(m);
  Serial.println("--------------");

  if(s == 'T'){
    Serial.println("Activated");
    if(m == 'T'){
      alarm();
    }else{
      arm();  
    }
  }
  
  if(s == 'F'){
    Serial.println("Deactivated");
    disarm();
  }
  
  if((s!='T') && (s!='F')){
    Serial.println("error, status not setted");
  }
  
}



void setup() {
    // By default MSP432 has analogRead() set to 10 bits.
    // to 12 bit resolution for MSP432.

    myScreen.begin();
    myScreen.setFontSize(myScreen.fontMax());

    // initialize serial communication
    Serial.begin(115200);      

    // attempt to connect to Wifi network:
    Serial.print("Attempting to connect to Network named: ");
    // print the network name (SSID);
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    WiFi.begin(ssid, password);
    while ( WiFi.status() != WL_CONNECTED) {
      // print dots while we wait to connect
      Serial.print(".");
      delay(300);
    }

    Serial.println("\nYou're connected to the network");
    Serial.println("Waiting for an ip address");

    while (WiFi.localIP() == INADDR_NONE) {
      // print dots while we wait for an ip addresss
      Serial.print(".");
      delay(300);
    }

    Serial.println("\nIP Address obtained");

    // you're connected now, so print out the status
    printWifiStatus();

    analogReadResolution(12);
    pinMode(greenLED, OUTPUT);
    pinMode(redLED, OUTPUT);
    pinMode(blueLED, OUTPUT);
    digitalWrite(redLED, HIGH);

    calculateRestValues();

    pinMode(buzzerPin,OUTPUT);

    isActive = false;
    arm();
}


void loop() {
    Serial.print("isActive: ");
    Serial.println(isActive);
    Serial.print("isMoving: ");
    Serial.println(isMoving);
    if(isActive && !isMoving){
      measureAnalogValues();
    
      int analogValueX = measureResults[0];
      int analogValueY = measureResults[1];
      int analogValueZ = measureResults[2];
    
      calculateChange(analogValueX,analogValueY,analogValueZ);
    
      int moving = movementDetected(diffResults);
    
      if(moving){
        warningMovement();
      }
    }
    
    httpRequestPolling();

}