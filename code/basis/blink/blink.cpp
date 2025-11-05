#include "SerialCommand.h"
#include "EEPROMAnything.h"
#include <BluetoothSerial.h>

BluetoothSerial BTSerial;

#define SerialPort Serial
#define Baudrate 115200

SerialCommand sCmd(SerialPort);
SerialCommand sCmdBT(BTSerial);

bool debug = false;
volatile bool run = false;
unsigned long previous, calculationTime;
unsigned long blinkElapsedTime = 0;

struct param_t
{
  unsigned long cycleTime;
  unsigned long blinkPeriod;
} params;

const int pinLed = 21;
const int pinButton = 15;

void IRAM_ATTR onButtonPress() {
  run = !run;
}

void setup()
{
  SerialPort.begin(Baudrate);
  BTSerial.begin("ESP32_BT_DJ");
  Serial.println("Bluetooth is klaar. Verbind met 'ESP32_BT_DJ' via PuTTY.");

  sCmd.setDefaultHandler(onUnknownCommand);
  sCmd.addCommand("help", onHelp);
  sCmd.addCommand("set", onSet);
  sCmd.addCommand("debug", onDebug);

  sCmdBT.setDefaultHandler(onUnknownCommand);
  sCmdBT.addCommand("help", onHelp);
  sCmdBT.addCommand("set", onSet);
  sCmdBT.addCommand("debug", onDebug);

  EEPROM.begin(512);
  EEPROM_readAnything(0, params);

  pinMode(pinLed, OUTPUT);
  pinMode(pinButton, INPUT);
  attachInterrupt(digitalPinToInterrupt(pinButton), onButtonPress, RISING);

  for (int i = 0; i < 3; i++) {
    digitalWrite(pinLed, HIGH);
    delay(150);
    digitalWrite(pinLed, LOW);
    delay(150);
  }
  delay(300);
  for (int i = 0; i < 2; i++) {
    digitalWrite(pinLed, HIGH);
    delay(500);
    digitalWrite(pinLed, LOW);
    delay(200);
  }

  SerialPort.println("ready");
}

void loop()
{
  sCmd.readSerial();
  sCmdBT.readSerial();

  unsigned long current = micros();
  if (current - previous > params.cycleTime)
  {
    previous = current;

    if (Serial.available()) {
      String command = Serial.readString();
      BTSerial.println("Van USB: " + command);
    }

    if (run)
    {
      blinkElapsedTime += params.cycleTime;
      if (blinkElapsedTime >= (params.blinkPeriod / 2))
      {
        digitalWrite(pinLed, !digitalRead(pinLed));
        blinkElapsedTime = 0;
      }
    }
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
  SerialPort.println("set cycle <unsigned long>");
  SerialPort.println("set blink <unsigned long>");
  SerialPort.println("set run <start/stop>");
  SerialPort.println("debug");
}

void onSet()
{
  char* param = sCmd.next();
  if (!param) param = sCmdBT.next();
  char* value = sCmd.next();
  if (!value) value = sCmdBT.next();

  if (strcmp(param, "cycle") == 0)
  {
    params.cycleTime = atol(value);
    SerialPort.print("set cycle: ");
    SerialPort.println(params.cycleTime);
  }
  else if (strcmp(param, "blink") == 0)
  {
    params.blinkPeriod = atol(value);
    SerialPort.print("set blink: ");
    SerialPort.println(params.blinkPeriod);
  }
  else if (strcmp(param, "run") == 0)
  {
    if (strcmp(value, "start") == 0)
    {
      run = true;
      SerialPort.print("set run start");
    }
    else if (strcmp(value, "stop") == 0)
    {
      run = false;
      SerialPort.print("set run stop");
    } 
    else onUnknownValue(value);
  }
  else onUnknownParam(param);

  EEPROM_writeAnything(0, params);
  EEPROM.commit();
}

void onDebug()
{
  SerialPort.print("cycle time: ");
  SerialPort.println(params.cycleTime);

  SerialPort.print("calculation time: ");
  SerialPort.println(calculationTime);
  calculationTime = 0;

  SerialPort.print("blink period: ");
  SerialPort.println(params.blinkPeriod);

  SerialPort.print("run: ");
  SerialPort.println(run);
}
