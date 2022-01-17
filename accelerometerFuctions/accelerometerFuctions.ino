/*
 The circuit utilizes the LEDs on the Educational BoosterPack MK II:
 * RED LED attached from digital pin 39 to ground.
 * GREEN LED attached from digital pin 38 to ground.
 * BLUE LED attached from digital pin 37 to ground.
 Analog accelerometer channels:
 * analog 25: z-axis
 * analog 24: y-axis
 * analog 23: x-axis
 * test
 
 
 Absolute rating/conversion can be determined from the ADXL3xx datasheet.
 As a quick reference, for LaunchPads with 12-bit ADC (MSP430F5529, Tiva C, etc.), 
 the entire analog  range is [0,4096]. For simple tilting calculation 
 [-1g,1g] ~ = [mid-800, mid + 800] = [2048-800,2048+800] = [1248,2848]

*/
#include<stdbool.h>  
#define DEBUGMODE true

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

void warning_movment()
{
  //changes the LED color green-->red
  //sends a warning message to the computer
  digitalWrite(redLED, HIGH);
  digitalWrite(greenLED, LOW);
  Serial.print("\nWARNING!!!, someone is stealing your shit\n");
  delay(1000);
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

int * measureAnalogValues()
{
  //Measures the analogValues of the x,y,z axis, and returns them in an array

  int analogValueX;// CHECK X AXIS (red) 
  analogValueX = analogRead(xpin);// read X axis 
  int analogValueY;
  analogValueY = analogRead(ypin); //read y axis
  int analogValueZ;
  analogValueZ = analogRead(zpin); // read Z axis

  // todo: array cannot be returned this way
  int results[3] = {analogValueX, analogValueY, analogValueZ};

  if(DEBUGMODE)
  {
    Serial.print("These are the measured values:\n");
    printAccelerometerData(analogValueX,analogValueY,analogValueZ);
    delay(5000);
  }
  
  return results;
}

int * calculateChange(int analogVX, int analogVY, int analogVZ)
{
  //calculates the difference between the measured x,y,z values, and the resting position values
    
    int xDiff = abs(analogVX-xpin_rest);
    int yDiff = abs(analogVY-ypin_rest);
    int zDiff = abs(analogVZ-zpin_rest);

    //todo array cannot be returned this way
    int results[3] = {xDiff, yDiff, zDiff};

    return results;
}

bool movementDetected(int axisDiffs[])
{
  //it checks if the detected accelerometer value is in the threshold

  for(int i=0; i<3;i++)
  {
    if (axisDiffs[i]-value_threshold)
     {
        return true;
     }
  }
  return false;
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
    analogReadResolution(12);
    Serial.begin(9600); // initialize for sending diagnostic info to computer
    pinMode(greenLED, OUTPUT);
    pinMode(redLED, OUTPUT);
    digitalWrite(redLED, HIGH);
    
    calculateRestValues();

    digitalWrite(redLED, LOW);
    digitalWrite(greenLED, HIGH);

    if(DEBUGMODE == true)
    { //this code only runs if the DEBUGMODE constanst is true
    printRestValues();
    
    }
  
}

void loop() {

    int* measuredValues;
    measuredValues = measureAnalogValues();
     
    if (DEBUGMODE == true)
    //this code only runs if the DEBUGMODE constanst is true
    {
      delay(1000);

      //todo there is a little problem with this, fix it the array cannot be returned the way it is done here 
      int analogValueX = measuredValues[0];
      int analogValueY = measuredValues[1];
      int analogValueZ = measuredValues[2];
      printAccelerometerData(analogValueX, analogValueY, analogValueZ);
      
    }
}
