#include <MeMCore.h>
#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>

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
  int sensorState;
  while(1){ // No black line encountered
    Serial.print(analogRead(A0));
    Serial.print("\n");
    sensorState = lineFinder.readSensors();
    // When obstacle is encountered, stop the mBot
    if(ultrasonic_3.distanceCm() < 7){
      move(0, 0);
    }
    // Encountered black line
    else if(sensorState == S1_IN_S2_IN){
      move(0, 0);
      // Function to detect color
         // If red is detected, turn left
         if(color == RED){
            move(3, 100);
         }
         // If green is detected, turn right
         else if(color == GREEN){
            move(4, 100);
         }
         // If light blue is detected, two successive right-turns in two grids
         else if(color == LIGHT BLUE){
            while(ultrasonic_3.distanceCm() < 7){
              move(4, 100);
            }
            while(ultrasonic_3.distanceCm() > 7){
              avoid_obstacle();
            }
            while(ultrasonic_3.distanceCm() < 7){
              move(4, 100);
            }
         }
         // If purple is detected, two successive left-turns in two grids
         else if(color == PURPLE){
            while(ultrasonic_3.distanceCm() < 7){
              move(3, 100);
            }
            while(ultrasonic_3.distanceCm() > 7){
              avoid_obstacle();
            }
            while(ultrasonic_3.distanceCm() < 7){
              move(3, 100);
            }
         }
         // If yellow is detected, U-turn in the same grid
         else if(color == YELLOW){
            while(ultrasonic_3.distanceCm() < 7){
              move(3, 100);
            }
         }
         // If black is detected, fire microphone
         else if(color == BLACK){
            // If sound is detected, check frequency
              // If frequency is between 100 and 300Hz, turn left
              // If frequency is 3000Hz and above, turn right
            // If no sound detected, end of maze.
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
    if(analogRead(A0) < 420){ // Left obstacle, turn right
      move(4, 100);
    }
    else if(analogRead(A1) < 150){ // Right obstacle, turn left
      move(3, 100);
    }
    else{
       motor_9.run(-1 * (50 / 100.0 * 255));
       motor_10.run(56 / 100.0 * 255);
    }
}
