#include "SerialCommand.h"
#include "EEPROMAnything.h"

#define SerialPort Serial
#define Baudrate 115200

SerialCommand sCmd(SerialPort); // SerialCommand opbject declaration

bool debug = false;
unsigned long previous, calculationTime; 

struct param_t
{
  unsigned long cycleTime;
  // add other paramaters here that need to be saved in memory 
} params;

void setup()
{
  SerialPort.begin(Baudrate); //set serial baudrate at 115200

  sCmd.setDefaultHandler(onUnknownCommand);
  sCmd.addCommand("help", onHelp);
  sCmd.addCommand("set", onSet);
  sCmd.addCommand("debug", onDebug);
  // add other commands here + add to onHelp

  EEPROM.begin(512);
  EEPROM_readAnything(0, params);

  pinMode(13, OUTPUT);
  // add other pin setup here
  SerialPort.println("ready");
}

void loop()
{
  sCmd.readSerial(); // listen and process serial commands

  unsigned long current = micros();
  if (current - previous > params.cycleTime)
  {
    previous = current;

    // add code here that needs to be executed cyclically

    // normalising and interpolating sensor

    // pid regulation

    // control motors
  }

  unsigned long difference = micros() - current;
  if (difference > calculationTime) calculationTime = difference;
}

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

void onHelp()
{
  SerialPort.println("Available commands:");
  SerialPort.println("help");
  SerialPort.println("set cycle <value>");
  SerialPort.println("debug");
  // add more if you register more commands
}

void onSet()
{
  char* param = sCmd.next();
  char* value = sCmd.next();

  if (strcmp(param, "cycle") == 0)
  {
    params.cycleTime = atol(value);
    SerialPort.print("set cycle: ");
    SerialPort.println(params.cycleTime);
  }
  // give parameters a new value with set command here

  else onUnknownParam((char*) param);

  EEPROM_writeAnything(0, params);
  EEPROM.commit();
}

void onDebug()
{
  SerialPort.print("cycle time: ");
  SerialPort.println(params.cycleTime);

  // show parameters with debug command here

  SerialPort.print("calculation time: ");
  SerialPort.println(calculationTime);
  calculationTime = 0;
}



