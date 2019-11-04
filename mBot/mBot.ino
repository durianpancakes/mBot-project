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
    sensorState = lineFinder.readSensors();
    // When obstacle is encountered, stop the mBot
    if(ultrasonic_3.distanceCm() < 7){
      move(0, 0);
    }
    // Encountered black line
    else if(sensorState == S1_IN_S2_IN){
      move(0, 0);
      // Function to detect color
         // If red is detected
         // If light blue is detected, two successive right-turns in two grids
         // If purple is detected, two successive left-turns in two grids
         // If green is detected, turn right
         // If yellow is detected, U-turn in the same grid
         // If black is detected, fire microphone
            // If sound is detected, check frequency
              // If frequency is between 100 and 300Hz, turn left
              // If frequency is 3000Hz and above, turn right
            // If no sound detected, end of maze.
    }
    else if(analogRead(A0) < 300){ // Left obstacle, turn right
      move(4, 100);
    }
    else if(analogRead(A1) < 280){ // Right obstacle, turn left
      move(3, 100);
    }
    else{
      move(1, 100);
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
