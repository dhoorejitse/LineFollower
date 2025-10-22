#include <Arduino.h>

// Motor pins
const int motorA_IN1 = 16;
const int motorA_IN2 = 17;
const int motorB_IN1 = 18;
const int motorB_IN2 = 19;

// PWM range
const int minSpeed = 0;
const int maxSpeed = 255;

// Delay tussen snelheidsveranderingen
const int delayTime = 200;

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("Setup klaar! Motoren test start...");

  pinMode(motorA_IN1, OUTPUT);
  pinMode(motorA_IN2, OUTPUT);
  pinMode(motorB_IN1, OUTPUT);
  pinMode(motorB_IN2, OUTPUT);
}

// Functie om motor te draaien
void driveMotor(int IN1, int IN2, int speed) {
  if (speed > 0) { // Vooruit
    analogWrite(IN1, speed);
    analogWrite(IN2, 0);
  } else if (speed < 0) { // Achteruit
    analogWrite(IN1, 0);
    analogWrite(IN2, -speed);
  } else { // Stop
    analogWrite(IN1, 0);
    analogWrite(IN2, 0);
  }
}

void loop() {
  // Motor A vooruit
  for (int s = 50; s <= maxSpeed; s += 5) {
    driveMotor(motorA_IN1, motorA_IN2, s);
    driveMotor(motorB_IN1, motorB_IN2, 0); // Motor B uit
    Serial.print("Motor A vooruit: ");
    Serial.println(s);
    delay(delayTime);
  }
  // Motor A achteruit
  for (int s = maxSpeed; s >= 50; s -= 5) {
    driveMotor(motorA_IN1, motorA_IN2, -s);
    driveMotor(motorB_IN1, motorB_IN2, 0);
    Serial.print("Motor A achteruit: ");
    Serial.println(s);
    delay(delayTime);
  }

  // Motor B vooruit
  for (int s = 50; s <= maxSpeed; s += 5) {
    driveMotor(motorA_IN1, motorA_IN2, 0);
    driveMotor(motorB_IN1, motorB_IN2, s);
    Serial.print("Motor B vooruit: ");
    Serial.println(s);
    delay(delayTime);
  }
  // Motor B achteruit
  for (int s = maxSpeed; s >= 50; s -= 5) {
    driveMotor(motorA_IN1, motorA_IN2, 0);
    driveMotor(motorB_IN1, motorB_IN2, -s);
    Serial.print("Motor B achteruit: ");
    Serial.println(s);
    delay(delayTime);
  }
}
