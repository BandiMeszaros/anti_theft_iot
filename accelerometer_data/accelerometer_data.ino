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
const int xpin = 23; // x-axis of the accelerometer
const int ypin = 24; // y-axis
const int zpin = 25; // z-axis (only on 3-axis models)
const int redLED= 39; // assign red LED to Pin #9
const int greenLED= 38;// assign green LED to Pin #10
const int blueLED= 37; // assign blueLED to Pin #12
static int xpin_rest = 0;
static int ypin_rest = 0;
static int zpin_rest = 0;

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
void setup() {
    // By default MSP432 has analogRead() set to 10 bits. 
    // to 12 bit resolution for MSP432.
    analogReadResolution(12);
    Serial.begin(9600); // initialize for sending diagnostic info to computer
    pinMode(greenLED, OUTPUT);
    pinMode(redLED, OUTPUT);
    digitalWrite(greenLED, HIGH);
    digitalWrite(redLED, HIGH);
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

    printRestValues();


    
}

int counter = 0;
void loop() {
    delay(500);
    int analogValue;// CHECK X AXIS (red) 
    analogValue = analogRead(xpin);// read X axis
    Serial.print("-----------Counter value: ");
    Serial.print(counter);
    Serial.print(" -------------\n");
    Serial.print("RED (X): ");      // print to serial monitor 
    Serial.print(analogValue); 
    Serial.print("\n"); 

    //analog write for the red LED
    /*if(analogValue >2048){ // check if tilting on x axis in positive direction
        analogValue = map(analogValue, 2048, 2850, 0, 150); // map to RGB range 
        analogWrite(redLED, analogValue); // output color 
    } 
    else if(analogValue<2048){ // check if tilting on x axis in negative direction 
        analogValue = map(analogValue, 1250, 2048, 150, 0); // map to RGB range 
        analogWrite(redLED, analogValue);  // output color 
    } 
    else{ 
        analogWrite(redLED, 0); // analogValue is b/w 450 & 490, X axis is ~flat 
    }*/
    // CHECK Y AXIS (green) 
    analogValue = analogRead(ypin); // read Y axis 
    Serial.print("GREEN (Y): "); // print to serial monitor 
    Serial.print(analogValue); 
    Serial.print("\n"); 

    //analog write for the green LED
    /*if(analogValue >2048){ // check if tilting on Y axis in positive direction 
        analogValue = map(analogValue, 2048, 2850, 0, 150); // map to RGB range 
        analogWrite(greenLED, analogValue); // output color 
    } 
    else if(analogValue<2048){ // check if tilting on Y axis in negative direction 
        analogValue = map(analogValue, 1250, 2048, 150, 0); // map to RGB range
        analogWrite(greenLED, analogValue); // output color 
    } 
    else{ 
        analogWrite(greenLED, 0);// analogValue is b/w 450 & 490, Y axis is ~flat 
    }*/
    
    // CHECK Z AXIS (blue) 
    analogValue = analogRead(zpin); // read Z axis 
    Serial.print("BLUE (Z): "); // print to serial monitor 
    Serial.print(analogValue); 
    Serial.print("\n"); 
    //analog write for the blue LED
    /*
    if(analogValue >2048){ // check if tilting on Z axis in positive direction 
        analogValue = map(analogValue, 2048, 2850, 0, 150); // map to RGB range 
        analogWrite(blueLED, analogValue); // output color 
    }
    else if(analogValue<2048){ // check if tilting on Z axis in negative direction 
        analogValue = map(analogValue, 1250, 2048, 150, 0); // map to RGB range 
        analogWrite(blueLED, analogValue); // output color 
    } 
    else{ 
        analogWrite(blueLED, 0); // analogValue is b/w 450 & 490, Z axis is ~flat 
    }*/
    counter = counter+1;
}
