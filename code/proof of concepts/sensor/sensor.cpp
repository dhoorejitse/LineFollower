const int analogPins[8] = {13,14,27,26,25,33,32,35};

void setup() {
  Serial.begin(115200);
  
  for (int i = 0; i < 8; i++) {
    pinMode(analogPins[i], INPUT);
  }
}

void loop() {
  Serial.print("Values: ");
  for (int i = 0; i < 8; i++) {
    int analogValue = analogRead(analogPins[i]);
    Serial.print(analogValue);
    Serial.print(" ");
  }
  Serial.print("\n");

  delay(250);
}
