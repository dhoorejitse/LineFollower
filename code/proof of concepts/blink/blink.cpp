const int pinR = 21;
const int pinG = 22;
const int pinB = 23;

void setup() {
  Serial.begin(115200);
  Serial.println("\nESP32 RGB LED test gestart...");

  pinMode(pinR, OUTPUT);
  pinMode(pinG, OUTPUT);
  pinMode(pinB, OUTPUT);

  // Opstartsequentie: 3 korte witte flitsen
  for (int i = 0; i < 3; i++) {
    setColor(HIGH, HIGH, HIGH);
    delay(150);
    setColor(LOW, LOW, LOW);
    delay(150);
  }

  delay(500);
}

void loop() {
  // Rood
  Serial.println("Rood");
  setColor(HIGH, LOW, LOW);
  delay(1000);
  setColor(LOW, LOW, LOW);
  delay(1000);

  // Groen
  Serial.println("Groen");
  setColor(LOW, HIGH, LOW);
  delay(1000);
  setColor(LOW, LOW, LOW);
  delay(1000);

  // Blauw
  Serial.println("Blauw");
  setColor(LOW, LOW, HIGH);
  delay(1000);
  setColor(LOW, LOW, LOW);
  delay(1000);
}

// Hulpfunctie om RGB te zetten
void setColor(int r, int g, int b) {
  digitalWrite(pinR, r);
  digitalWrite(pinG, g);
  digitalWrite(pinB, b);
}
