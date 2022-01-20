/*
 The circuit utilizes the LEDs on the Educational BoosterPack MK II:
 * RED LED attached from digital pin 39 to ground.
 * GREEN LED attached from digital pin 38 to ground.
 * BLUE LED attached from digital pin 37 to ground.
 Analog accelerometer channels:
 * analog 25: z-axis
 * analog 24: y-axis
 * analog 23: x-axis


 Absolute rating/conversion can be determined from the ADXL3xx datasheet.
 As a quick reference, for LaunchPads with 12-bit ADC (MSP430F5529, Tiva C, etc.),
 the entire analog  range is [0,4096]. For simple tilting calculation
 [-1g,1g] ~ = [mid-800, mid + 800] = [2048-800,2048+800] = [1248,2848]

*/

#define DEBUGMODE 1

const int value_threshold = 700;
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
static int movmentFlag = 0;
const int pushbuttonBooster1 = 32;
const int pushbuttonBooster2 = 33;


#ifndef __CC3200R1M1RGC__
// Do not include SPI for CC3200 LaunchPad
#include <SPI.h>
#endif
#include <WiFi.h>

// your network name also called SSID
char ssid[] = "mywifi";
// your network password
char password[] = "wifijelszo";
// your network key Index number (needed only for WEP)
int keyIndex = 0;

WiFiServer server(80);


void alarm()
{
  //someone is stealing your shit
  if(movmentFlag)
  {
    digitalWrite(redLED, LOW);
    digitalWrite(greenLED, LOW);
    digitalWrite(blueLED, LOW);
    Serial.print("\nALARM!!!, someone is stealing your shit, call the police!!!\n");

    //this is just a demo, if other modules are ready it can be deleted
    while(1)
    {
      Serial.print("ALARM!!!!!!\n");
      digitalWrite(redLED,HIGH);
      delay(250);
      digitalWrite(redLED, LOW);
      delay(250);
    }
  }
}
void warning_movment()
{
  //changes the LED color green-->red
  //sends a warning message to the computer
  digitalWrite(redLED, HIGH);
  digitalWrite(greenLED, LOW);
  Serial.print("\nWARNING, WARNING, movement detected\n");
  movmentFlag = 1;
}

void printDiffData(int diffX, int diffY, int diffZ)
{
  Serial.print(" -------------\n");
  Serial.print("Xdiff: ");
  Serial.print(diffX);
  Serial.print("\n");
  Serial.print("Ydiff: "); // print to serial monitor
  Serial.print(diffY);
  Serial.print("\n");
  Serial.print("Zdiff: "); // print to serial monitor
  Serial.print(diffZ);
  Serial.print("\n");
}
void printAccelerometerData(int xpinV, int ypinV, int zpinV)
{

  Serial.print(" -------------\n");
  Serial.print("RED (X): ");
  Serial.print(xpinV);
  Serial.print("\n");
  Serial.print("GREEN (Y): "); // print to serial monitor
  Serial.print(ypinV);
  Serial.print("\n");
  Serial.print("BLUE (Z): "); // print to serial monitor
  Serial.print(zpinV);
  Serial.print("\n");
}

void printRestValues()
{
    Serial.print("----------");
    Serial.print("\nx axis rest value: ");
    Serial.print(xpin_rest);
    Serial.print("\n");
    Serial.print("----------");

    Serial.print("\ny axis rest value: ");
    Serial.print(ypin_rest);
    Serial.print("\n");
    Serial.print("----------");

    Serial.print("\nz axis rest value: ");
    Serial.print(zpin_rest);
    Serial.print("\n");
    Serial.print("----------");
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

  if(DEBUGMODE)
  {
    Serial.print("These are the measured values:\n");
    printAccelerometerData(analogValueX,analogValueY,analogValueZ);
    delay(1000);
  }
}

void calculateChange(int analogVX, int analogVY, int analogVZ)
{
  //calculates the difference between the measured x,y,z values, and the resting position values

    int xDiff = abs(analogVX-xpin_rest);
    int yDiff = abs(analogVY-ypin_rest);
    int zDiff = abs(analogVZ-zpin_rest);

    if(DEBUGMODE)
    {
      Serial.print("\nThese are the values after substraction:\n");
      printDiffData(xDiff, yDiff, zDiff);
      Serial.print("And these are the measrued values and the rest values:\n\n\n");
      printAccelerometerData(analogVX, analogVY, analogVZ);
      printRestValues();
    }

    diffResults[0] = xDiff;
    diffResults[1] = yDiff;
    diffResults[2] = zDiff;
}

int movementDetected(int axisDiffs[])
{
  //it checks if the detected accelerometer value is in the threshold

  for(int i=0; i<3;i++)
  {
    if (0 > value_threshold-axisDiffs[i])
     {
       if(DEBUGMODE)
       {
         Serial.print("\n\n---------\nmovement detector thinks that this is the threshlod-axisDiffs[i] value:\t---->\t");
         Serial.print(value_threshold-axisDiffs[i]);
         Serial.print("\n");
       }
        return 1;
     }
     Serial.print("\n\n---------\nmovement detector thinks that this is the threshlod-axisDiffs[i] value:\t---->\t");
     Serial.print(value_threshold-axisDiffs[i]);
     Serial.print("\nwhere i is:");
     Serial.print(i);
     Serial.print("\n");
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
void setup() {
    // By default MSP432 has analogRead() set to 10 bits.
    // to 12 bit resolution for MSP432.

    Serial.begin(115200);      // initialize serial communication

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

    Serial.println("Starting webserver on port 80");
    server.begin();                           // start the web server on port 80
    Serial.println("Webserver started!");
    analogReadResolution(12);
    pinMode(greenLED, OUTPUT);
    pinMode(redLED, OUTPUT);
    pinMode(blueLED, OUTPUT);
    pinMode(pushbuttonBooster1, INPUT);
    pinMode(pushbuttonBooster2, INPUT);
    digitalWrite(redLED, HIGH);

    calculateRestValues();


    digitalWrite(redLED, LOW);
    digitalWrite(greenLED, HIGH);
    digitalWrite(blueLED, LOW);

    if(DEBUGMODE == true)
    { //this code only runs if the DEBUGMODE constanst is true
    printRestValues();
    }

}

void loop() {

    measureAnalogValues();
    int analogValueX = measureResults[0];
    int analogValueY = measureResults[1];
    int analogValueZ = measureResults[2];
    calculateChange(analogValueX,analogValueY,analogValueZ);

    //for DEBUGMODE
    int diffX = diffResults[0];
    int diffY = diffResults[1];
    int diffZ = diffResults[2];

    int moving = movementDetected(diffResults);
    if(moving)
    {
      warning_movment();
    }
    if(movmentFlag)
    {
      int i1 = 0;
      WiFiClient client = server.available();   // listen for incoming clients

      if (client) {                             // if you get a client,
        Serial.println("new client");           // print a message out the serial port
        char buffer[150] = {0};                 // make a buffer to hold incoming data
        while (client.connected()) {            // loop while the client's connected
          if (client.available()) {             // if there's bytes to read from the client,
            char c = client.read();             // read a byte, then
            Serial.write(c);                    // print it out the serial monitor
            if (c == '\n') {                    // if the byte is a newline character

              // if the current line is blank, you got two newline characters in a row.
              // that's the end of the client HTTP request, so send a response:
              if (strlen(buffer) == 0) {
                // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                // and a content-type so the client knows what's coming, then a blank line:
                client.println("HTTP/1.1 200 OK");
                client.println("Content-type:text/html");
                client.println();

                // the content of the HTTP response follows the header:
                client.println("<html><head><title>Supersecure WiFi Web Server</title></head><body align=center>");
                client.println("<h1 align=center><font color=\"red\">Welcome to the supersecure car monitor</font></h1>");
                client.print("RED LED <button onclick=\"location.href='/H'\">Chill it is me</button>");
                client.println(" <button onclick=\"location.href='/L'\">ALARM the police</button><br>");

                // The HTTP response ends with another blank line:
                client.println();
                // break out of the while loop:
                break;
              }
              else {      // if you got a newline, then clear the buffer:
                memset(buffer, 0, 150);
                i1 = 0;
              }
            }
            else if (c != '\r') {    // if you got anything else but a carriage return character,
              buffer[i1++] = c;      // add it to the end of the currentLine
            }

            // Check to see if the client request was "GET /H" or "GET /L":
            if (endsWith(buffer, "GET /H")) {
              movmentFlag = 0;
              digitalWrite(blueLED, HIGH); //drive mode
              digitalWrite(redLED, LOW);
            }
            if (endsWith(buffer, "GET /L")) {
              alarm();
            }
          }
        }
        // close the connection:
        client.stop();
        Serial.println("client disonnected");
      }


      /* manual disalarm
      int timer = 0;
      int button2 = 0;
      int button3 = 0;
      for(timer; timer<2000; timer++)
      {//2000 loops until it alarms the police
        button2 = digitalRead(pushbuttonBooster1);
        button3 = digitalRead(pushbuttonBooster2);
        Serial.print("\nTimer value:  ");
        Serial.print(timer);
        if(DEBUGMODE)
        {
          Serial.print("\n This is the values of the buttons, during warning phase:\n");
          Serial.print(button2);
          Serial.print("\n");
          Serial.print(button3);
          Serial.print("\n");
        }
        if (!button2 && !button3)
        {
          //disalarm
          movmentFlag = 0;
          digitalWrite(blueLED, HIGH); //drive mode
          digitalWrite(redLED, LOW);
          break;*/
        }
    if (DEBUGMODE == true)
    //this code only runs if the DEBUGMODE constanst is true
    {
      delay(1000);
      printAccelerometerData(analogValueX, analogValueY, analogValueZ);
      printDiffData(diffX, diffY, diffZ);
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
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

boolean endsWith(char* inString, const char* compString) {
  int compLength = strlen(compString);
  int strLength = strlen(inString);

  //compare the last "compLength" values of the inString
  int i;
  for (i = 0; i < compLength; i++) {
    char a = inString[(strLength - 1) - i];
    char b = compString[(compLength - 1) - i];
    if (a != b) {
      return false;
    }
  }
  return true;
}
