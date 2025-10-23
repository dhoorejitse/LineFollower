const int pinR = 21;
const int pinG = 22;
const int pinB = 23;
const int buttonPin = 15;

volatile bool buttonPressed = false;
bool isInterrupted = false;

unsigned long previousMillis = 0;
int colorState = 0; // 0=rood, 1=groen, 2=blauw
const unsigned long interval = 1000; // 1 seconde per kleur

// Debounce variabelen
volatile unsigned long lastInterruptTime = 0;
const unsigned long debounceDelay = 200; // 200 ms

void IRAM_ATTR handleButtonPress() {
  unsigned long currentTime = millis();
  // Check of laatste interrupt minstens debounceDelay ms geleden is
  if (currentTime - lastInterruptTime > debounceDelay) {
    buttonPressed = true;
    lastInterruptTime = currentTime;
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("\nESP32 RGB LED test gestart...");

  pinMode(pinR, OUTPUT);
  pinMode(pinG, OUTPUT);
  pinMode(pinB, OUTPUT);
  pinMode(buttonPin, INPUT); // externe pulldown

  attachInterrupt(digitalPinToInterrupt(buttonPin), handleButtonPress, RISING);

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
  // --- knop afhandelen ---
  if (buttonPressed) {
    buttonPressed = false;
    isInterrupted = !isInterrupted;

    if (isInterrupted) {
      Serial.println("Onderbroken → wit");
      setColor(HIGH, HIGH, HIGH);
    } else {
      Serial.println("Hervatten cyclus");
      previousMillis = millis(); // herstart timing
    }
  }

  // --- cyclus ---
  if (!isInterrupted) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      nextColor();
    }
  }
}

void nextColor() {
  switch (colorState) {
    case 0:
      Serial.println("Rood");
      setColor(HIGH, LOW, LOW);
      break;
    case 1:
      Serial.println("Groen");
      setColor(LOW, HIGH, LOW);
      break;
    case 2:
      Serial.println("Blauw");
      setColor(LOW, LOW, HIGH);
      break;
  }
  colorState = (colorState + 1) % 3;
}

void setColor(int r, int g, int b) {
  digitalWrite(pinR, r);
  digitalWrite(pinG, g);
  digitalWrite(pinB, b);
}
