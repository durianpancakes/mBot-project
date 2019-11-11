#include <MeMCore.h>
#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#define LDRWait 10
#define LDR A6
#define RGBWait 200
#define DCTurnWait 530
#define DCUTurnWait 1231
#define debug 1
#define buzzer 8
#define HIGH_PASS A0
#define BAND_PASS A1
#define LEFT_IR A2
#define RIGHT_IR A3


// Floats to hold colour and sound arrays
float colourArray[] = {0,0,0};
float blackArray[] = {121,116,109};
float greyDiff[] = {547,563,488};
float soundArray[] = {0,0};
float baseSoundArray[] = {0,0};

MeRGBLed rgbled_7(7, 2);
MeUltrasonicSensor ultrasonic_1(1);
MeDCMotor motor_9(9);
MeDCMotor motor_10(10);
MeLineFollower lineFinder(PORT_2);

void setup() {
  Serial.begin(9600);
  pinMode(LEFT_IR, INPUT); // Left IR
  pinMode(RIGHT_IR, INPUT); // Right IR
  pinMode(HIGH_PASS, INPUT); // High-pass filter
  pinMode(BAND_PASS, INPUT); // Band-pass filter
  baseSoundArray[0] = getAvgReading_sound(5,A2);
  baseSoundArray[1] = getAvgReading_sound(5,A3);
}

void loop() {
  int sensorState, color, sound;
  int n;
  while(1){ // No black line encountered   
    sensorState = lineFinder.readSensors(); 
    Serial.print(analogRead(HIGH_PASS));
    Serial.print("\n");

    // Encountered black line
    if(sensorState == S1_IN_S2_IN){
      move(0, 0);
      color = colourSensor();
      // Function to detect color
         // If green is detected, turn right
         if(color == 1){
            move(4, 100);
            delay(DCTurnWait);
         }
         
         // If red is detected, turn left
         else if(color == 2){
            move(3, 100);
            delay(DCTurnWait);
         }
         
         // If light blue is detected, two successive right-turns in two grids
         else if(color == 4){
            move(4, 100);
            delay(DCTurnWait);
            while(ultrasonic_1.distanceCm() > 7){
              avoid_obstacle();
            }
            move(4, 100);
            delay(DCTurnWait);
         }
         // If purple is detected, two successive left-turns in two grids
         else if(color == 5){
            move(3, 100);
            delay(DCTurnWait);
            while(ultrasonic_1.distanceCm() > 7){
              avoid_obstacle();
            }
            move(3, 100);
            delay(DCTurnWait);
         }
         // If yellow is detected, U-turn in the same grid
         else if(color == 3){
            move(4, 100);
            delay(DCUTurnWait);
         }
         
         // If black is detected, fire microphone
         else if(color == 0){
            move(0,0);
            // If sound is detected, check frequency
            sound = read_sound();
            // If frequency is between 100 and 300Hz, turn left
            if(sound == 1){
              move(3, 100);
              delay(DCTurnWait);
            }
            // If frequency is 3000Hz and above, turn right
            else if(sound == 2){
              move(4, 100);
              delay(DCTurnWait);
            }
            // If no sound detected, end of maze.
            else{
              move(0, 0);
              // playtone();
            } 
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
    if(analogRead(LEFT_IR) < 100){ // Left obstacle, turn right
      move(4, 100);
    }
    else if(analogRead(RIGHT_IR) < 100){ // Right obstacle, turn left
      move(3, 100);
    }
    else if(ultrasonic_1.distanceCm() < 7){
      move(0, 0);
    }
    else{
      move(1, 180);
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
void lightyellow(){
  rgbled_7.setColor(0, 220, 112, 175);
}
void lightoff() {
  rgbled_7.setColor(0, 0, 0, 0);
  rgbled_7.show();
}

int getAvgReading(int times){      
//find the average reading for the requested number of times of scanning LDR
  int reading;
  int total = 0;
//take the reading as many times as requested and add them up
  for(int i = 0; i < times; i++){
     reading = analogRead(LDR);
     total = reading + total;
     delay(LDRWait);
  }
//calculate the average and return it
  return total/times;
}

int getAvgReading_sound(int times, int sound){      
//find the average reading for the requested number of times of scanning LDR
  int reading;
  int total = 0;

  if(sound == 1){
    for(int i = 0; i < times; i++){
     reading = analogRead(BAND_PASS);
     total = reading + total;
     delay(LDRWait);
    }
  }
  else if(sound == 2){
     for(int i = 0; i < times; i++){
        reading = analogRead(HIGH_PASS);
        total = reading + total;
        delay(LDRWait);
     }
  }
//take the reading as many times as requested and add them up
  
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
      //Serial.println(int(colourArray[c]));
      colourArray[c] = (colourArray[c] - blackArray[c])/(greyDiff[c])*255;
      lightoff(); //turn off the current LED colour
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
    else if (colourArray[1]>colourArray[0]&& colourArray[1]>colourArray[2]&& colourArray[1]>160) // Light blue= 4
    {
      return 4;
    }
    else if (colourArray[1]>colourArray[0]&& colourArray[1]>colourArray[2]&& colourArray[1]<160 && colourArray[0]>75 && colourArray[2]>75)//purple=5
    {
      return 5;
    }
    else if (colourArray[2]<25&& colourArray[1]<25&& colourArray[0]<25)//Black=0
    {
      return 0;
    }
}

int read_sound(){
  soundArray[0] = getAvgReading_sound(5, 1) / baseSoundArray[0]; // Get average high pass reading
  soundArray[1] = getAvgReading_sound(5, 2) / baseSoundArray[1]; // Get average band pass reading
    
  if(soundArray[0] - soundArray[1] < 1 || soundArray[1] - soundArray[0] < 1){
    return 0;
  }
  else if(soundArray[0] > soundArray[1]){
    return 1;
  }
  else if(soundArray[0] < soundArray[1]){
    return 2;
  }
}
