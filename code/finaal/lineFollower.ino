#include <Arduino.h>
#include "SerialCommand.h"
#include "EEPROMAnything.h"
#include <BluetoothSerial.h>

BluetoothSerial SerialPort;

void IRAM_ATTR handleButtonPress();
void updateRunLED();                  
void setColor(int r, int g, int b);

#define Baudrate 115200

#define MotorLeftForward 16
#define MotorLeftBackward 17
#define MotorRightForward 18
#define MotorRightBackward 19

const int pinR = 21;
const int pinG = 22;
const int pinB = 23;
const int buttonPin = 15;

volatile bool buttonPressed = false;
volatile unsigned long lastInterruptTime = 0;
const unsigned long debounceDelay = 200;

SerialCommand sCmd(SerialPort); // SerialPort for wired
bool debug = false;
bool run = false;
unsigned long previous, calculationTime; //set cycle time 2x calculationTime

const int sensor[] = {35,32,33,25,26,27,14,13};

struct param_t
{
  unsigned long cycleTime;
  int black[8]; //sensor
  int white[8]; //sensor
  int power;
  float diff;
  float kp;
  float ki;
  float kd;
  // add other paramaters here that need to be saved in memory 
} params;

int normalised[8];
float debugPosition;
float output;
float iTerm = 0;
float lastErr;

//*******************************************************************************// SETUP

void setup()
{
  Serial.begin(115200);
  SerialPort.begin("ESP32_BT_DJ");
  SerialPort.println("Verbonden");

  sCmd.setDefaultHandler(onUnknownCommand);
  sCmd.addCommand("help", onHelp);
  sCmd.addCommand("set", onSet);
  sCmd.addCommand("debug", onDebug);
  sCmd.addCommand("calibrate", onCalibrate);
  sCmd.addCommand("run", onRun);

  EEPROM.begin(512);
  EEPROM_readAnything(0, params);

  pinMode(pinR, OUTPUT);
  pinMode(pinG, OUTPUT);
  pinMode(pinB, OUTPUT);
  pinMode(buttonPin, INPUT);

  attachInterrupt(digitalPinToInterrupt(buttonPin), handleButtonPress, RISING);
  updateRunLED();

  SerialPort.println("ready");

  iTerm = 0;
}

//*******************************************************************************// LOOP

void loop()
{
  if (buttonPressed) 
  {
    buttonPressed = false;
    run = !run;
    if (run == false)
    {
      iTerm = 0;
    }
    updateRunLED();
  }

  sCmd.readSerial(); // listen and process serial commands

  unsigned long current = micros();
  if (current - previous > params.cycleTime)
  {
    previous = current;
    
    //measure & normalize
    for (int i = 0; i < 8; i++) normalised[i] = map(analogRead(sensor[i]), params.black[i], params.white[i], 0, 4096);

    //interpolate
    float position = 0;
    int index = 0;
    for (int i = 1; i < 8; i++) if (normalised[i] < normalised[index]) index = i;

    if (normalised[index] > 3000)
    {
      run = false; //can see if the robot is on a line or not, if not it, it will stop
      iTerm = 0;
      updateRunLED();
    }

    if (index == 0) position = -25;
    else if (index == 7) position = 25;
    else 
    {
      int sZero = normalised[index];
      int sMinusOne = normalised[index-1];
      int sPlusOne = normalised[index+1];

      float b = sPlusOne - sMinusOne;
      b = b / 2;

      float a = sPlusOne - b - sZero;

      position = -b / (2 * a);
      position += index;
      position -= 3.5;

      position *= 10;
    }
    debugPosition = position;

    float error = -position;
    output = 0;

    if (run) 
    {
      //proportional
      output = error * params.kp;

      //integration
      iTerm += params.ki * error;
      iTerm = constrain(iTerm, -510, 510);
      output += iTerm;

      //differentiation
      output += params.kd * (error - lastErr);
      lastErr = error;
    }
    else
    {
      iTerm = 0;
      output = 0;
    }

    output = constrain(output, -510, 510); //max en min PWM waarde motoren

    int powerLeft = 0;
    int powerRight = 0;

    if (run) if (output >= 0)
    {
      powerLeft = constrain(params.power + params.diff * output, -255, 255); //PWM motoren
      powerRight = constrain(powerLeft - output, -255, 255); //PWM motoren
      powerLeft = powerRight + output;
    }
    else
    {
      powerRight = constrain(params.power - params.diff * output, -255, 255); //PWM motoren
      powerLeft = constrain(powerRight + output, -255, 255); //PWM motoren
      powerRight = powerLeft - output;
    }

    // control motors
    analogWrite(MotorLeftForward, powerLeft > 0 ? powerLeft : 0);
    analogWrite(MotorLeftBackward, powerLeft < 0 ? -powerLeft : 0);
    analogWrite(MotorRightForward, powerRight > 0 ? powerRight : 0);
    analogWrite(MotorRightBackward, powerRight < 0 ? -powerRight : 0);
  }
  unsigned long difference = micros() - current;
  if (difference > calculationTime) calculationTime = difference;
}

//*******************************************************************************// SET

void onSet() //set commands
{
  char* param = sCmd.next();
  char* value = sCmd.next();

  if (strcmp(param, "cycle") == 0)
  {
    long newCycleTime = atol(value);
    float ratio = ((float) newCycleTime) / ((float) params.cycleTime);
    params.ki *= ratio;
    params.kd /= ratio;
    params.cycleTime = newCycleTime;
  } 
  else if (strcmp(param, "power") == 0) params.power = atol(value);
  else if (strcmp(param, "diff") == 0) params.diff = atof(value);
  else if (strcmp(param, "kp") == 0) params.kp = atof(value);
  else if (strcmp(param, "ki") == 0)
  {
    float cycleTimeInSec = ((float) params.cycleTime) / 1000000;
    params.ki = atof(value) * cycleTimeInSec;
  }
  else if (strcmp(param, "kd") == 0)
  {
    float cycleTimeInSec = ((float) params.cycleTime) / 1000000;
    params.kd = atof(value) / cycleTimeInSec;
  }
  else onUnknownParam((char*) param);

  EEPROM_writeAnything(0, params);
  EEPROM.commit();
}

//*******************************************************************************// CALIBRATE

void onCalibrate() //sensor calibration
{
  char* param = sCmd.next();

  if (strcmp(param, "black") == 0)
  {
    SerialPort.print("start calibrating black... ");
    for (int i = 0; i < 8; i++) params.black[i]=analogRead(sensor[i]);
    SerialPort.println("done");
  }
  else if (strcmp(param, "white") == 0)
  {
    SerialPort.print("start calibrating white... ");    
    for (int i = 0; i < 8; i++) params.white[i]=analogRead(sensor[i]);  
    SerialPort.println("done");      
  }
  else onUnknownParam((char*) param);

  EEPROM_writeAnything(0, params);
  EEPROM.commit();
}

//*******************************************************************************// DEBUG

void onDebug()
{
  SerialPort.print("cycle time: ");
  SerialPort.println(params.cycleTime);

  SerialPort.print("calculation time: ");
  SerialPort.println(calculationTime);
  calculationTime = 0;

  SerialPort.print("black: ");
  for (int i = 0; i < 8; i++)
  {
    SerialPort.print(params.black[i]);
    SerialPort.print(" ");
  }
  SerialPort.println(" ");

  SerialPort.print("white: ");
  for (int i = 0; i < 8; i++)
  {
    SerialPort.print(params.white[i]);
    SerialPort.print(" ");
  }
  SerialPort.println(" ");

  SerialPort.print("normalised: ");
  for (int i = 0; i < 8; i++)
  {
    SerialPort.print(normalised[i]);
    SerialPort.print(" ");
  }
  SerialPort.println(" ");

  SerialPort.print("position: ");
  SerialPort.println(debugPosition);

  SerialPort.print("power: ");
  SerialPort.println(params.power);

  SerialPort.print("diff: ");
  SerialPort.println(params.diff);

  SerialPort.print("kp: ");
  SerialPort.println(params.kp);

  float cycleTimeInSec = ((float) params.cycleTime) / 1000000;
  float ki = params.ki / cycleTimeInSec;
  SerialPort.print("ki: ");
  SerialPort.println(ki);

  float kd = params.kd * cycleTimeInSec;
  SerialPort.print("kd: ");
  SerialPort.println(kd);

  SerialPort.print("iTerm: ");
  SerialPort.println(iTerm);

  SerialPort.print("output: ");
  SerialPort.println(output);
}

//*******************************************************************************// INVALID COMMANDO

void onUnknownCommand(char* command)
{
  SerialPort.print("Unknown Command: \"");
  SerialPort.print(command);
  SerialPort.println("\"");
}

void onUnknownParam(char* param)
{
  SerialPort.print("Unknown Param: \"");
  SerialPort.print(param);
  SerialPort.println("\"");
}

void onUnknownValue(char* value)
{
  SerialPort.print("Unknown Value: \"");
  SerialPort.print(value);
  SerialPort.println("\"");
}

//*******************************************************************************// HELP

void onHelp()
{
  SerialPort.println("Available commands:");
  SerialPort.println("help");
  SerialPort.println("set cycle <value>");
  SerialPort.println("debug");
  SerialPort.println("calibrate <black/white>");
  SerialPort.println("run <true/false>");
  // add more if you register more commands
}

//*******************************************************************************// RUN

void onRun() //start or stop driving
{
  char* param = sCmd.next();

  if (strcmp(param, "true") == 0) 
  {
    run = true;
  } 
  else if (strcmp(param, "false") == 0)
  {
    run = false;
    iTerm = 0;
  }
  else 
  {
    onUnknownParam((char*) param);
    return;
  }
  updateRunLED();
}

void IRAM_ATTR handleButtonPress() {
  unsigned long t = millis();
  if (t - lastInterruptTime > debounceDelay) {
    buttonPressed = true;
    lastInterruptTime = t;
  }
}

void updateRunLED() {
  if (run) setColor(LOW, HIGH, LOW);   // green
  else     setColor(HIGH, LOW, LOW);   // red
}

//*******************************************************************************// RGB

void setColor(int r, int g, int b) {
  digitalWrite(pinR, r);
  digitalWrite(pinG, g);
  digitalWrite(pinB, b);
}








