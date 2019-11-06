#include <MeMCore.h>
#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#define LDRWait 10
#define LDR A6
#define RGBWait 200

//floats to hold colour arrays
float colourArray[] = {0,0,0};
float blackArray[] = {346,339,297};
float greyDiff[] = {635,644,680};

MeRGBLed rgbled_7(7, 2);
MeUltrasonicSensor ultrasonic_3(3);
MeDCMotor motor_9(9);
MeDCMotor motor_10(10);
MeLineFollower lineFinder(PORT_2);

void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT); // Left IR
  pinMode(A1, INPUT); // Right IR
}

void loop() {
  int sensorState, color;
  int n;
  while(1){ // No black line encountered   
    sensorState = lineFinder.readSensors();
    Serial.print("A0 = \n");
    Serial.print(analogRead(A0));

    // Encountered black line
    if(sensorState == S1_IN_S2_IN){
      move(0, 0);
      color = colourSensor();
      Serial.print("Action = ");
      Serial.print(color);
      // Function to detect color
         
         // If green is detected, turn right
         if(color == 1){
            move(4, 100);
            delay(950);
         }
         
         // If red is detected, turn left
         else if(color == 2){
            move(3, 100);
            delay(950);
         }
         
         // If light blue is detected, two successive right-turns in two grids
         else if(color == 4){
            move(4, 100);
            delay(950);
            while(ultrasonic_3.distanceCm() > 7){
              avoid_obstacle();
            }
            move(4, 100);
            delay(950);
         }
         // If purple is detected, two successive left-turns in two grids
         else if(color == 5){
            move(3, 100);
            delay(950);
            while(ultrasonic_3.distanceCm() > 7){
              avoid_obstacle();
            }
            move(3, 100);
            delay(950);
         }
         // If yellow is detected, U-turn in the same grid
         else if(color == 3){
            move(3, 100);
            delay(1600);
         }
         // If black is detected, fire microphone
         else if(color == 0){
            // If sound is detected, check frequency
              // If frequency is between 100 and 300Hz, turn left
              // If frequency is 3000Hz and above, turn right
            // If no sound detected, end of maze.
            move(0,0);
         }
    }
    else{
        avoid_obstacle();
    }
    
  }
}

// This function moves the mBot
void move(int direction, int speed){
  int leftSpeed = 0;
  int rightSpeed = 0;

  if(direction == 1){ // Moving forward
    leftSpeed = speed;
    rightSpeed = speed;
  }
  else if(direction == 2){ // Reversing
    leftSpeed = -speed;
    rightSpeed = -speed;
  }
  else if(direction == 3){ // Turning left
    leftSpeed = -speed;
    rightSpeed = speed;
  }
  else if(direction == 4){ // Turning right
    leftSpeed = speed;
    rightSpeed = -speed;
  }
  motor_9.run((9) == M1 ? -(leftSpeed) : (leftSpeed));
  motor_10.run((10) == M1 ? -(rightSpeed) : (rightSpeed));
}

void avoid_obstacle(){
    if(analogRead(A0) < 450){ // Left obstacle, turn right
      move(4, 100);
    }
    else if(analogRead(A1) < 350){ // Right obstacle, turn left
      move(3, 100);
    }
    else if(ultrasonic_3.distanceCm() < 7){
      move(0, 0);
    }
    else{
      move(1, 150);
    }
}


void lightred() {
  rgbled_7.setColor(0,255,0,0);
  rgbled_7.show();
}
void lightgreen() {
  rgbled_7.setColor(0, 0,255,0);
  rgbled_7.show();
}
void lightblue() {
  rgbled_7.setColor(0, 0,0,255);
  rgbled_7.show();
}
void lightoff() {
  rgbled_7.setColor(0, 0, 0, 0);
  rgbled_7.show();
}

int getAvgReading(int times){      
//find the average reading for the requested number of times of scanning LDR
  int reading;
  int total =0;
//take the reading as many times as requested and add them up
  for(int i = 0;i < times;i++){
     reading = analogRead(LDR);
     total = reading + total;
     delay(LDRWait);
  }
//calculate the average and return it
  return total/times;
}

int colourSensor() {
  
     lightred();
     delay(RGBWait);
     colourArray[0] = getAvgReading(5);         //scan 5 times to take the average, 
     lightoff();
     delay(RGBWait);
     lightblue();
     delay(RGBWait);
     colourArray[1] = getAvgReading(5);         //scan 5 times to take the average, 
     lightoff();
     delay(RGBWait);
     lightgreen();
     delay(RGBWait);
     colourArray[2] = getAvgReading(5);         //scan 5 times to take the average, 
     lightoff();
     delay(RGBWait);
     for(int c=0;c<=2;c++){ 
      Serial.println("Colour = ");
      //Serial.println(int(colourArray[c]));
      colourArray[c] = (colourArray[c] - blackArray[c])/(greyDiff[c])*255;
      lightoff(); //turn off the current LED colour
      Serial.println(int(colourArray[c])); //show the value for the current colour LED, which corresponds to either the R, G or B of the RGB code
     }
     if (colourArray[2]>colourArray[1]&& colourArray[2]>colourArray[0]&& colourArray[0]<100 && colourArray[1]<100 && colourArray[2]>50) //Green=1
     {
     return 1;
     }
     else if (colourArray[0]>colourArray[1]&& colourArray[0]>colourArray[2]&& colourArray[0]>100 && colourArray[2]<100 && colourArray[1]<100) //RED = 2
     {
      return 2;
     }
    else if (colourArray[0]>colourArray[1]&& colourArray[0]>colourArray[2]&& colourArray[0]>200) //Yellow = 3
    {
      return 3;
    }
    else if (colourArray[1]>colourArray[0]&& colourArray[1]>colourArray[2]&& colourArray[1]>150) // Light blue= 4
    {
      return 4;
    }
    else if (colourArray[1]>colourArray[0]&& colourArray[1]>colourArray[2]&& colourArray[1]<200 && colourArray[0]>100 && colourArray[2]>100)//purple=5
    {
      return 5;
    }
    else if (colourArray[2]<25&& colourArray[1]<25&& colourArray[0]<25)//Black=0
    {
      return 0;
    }
   
}
