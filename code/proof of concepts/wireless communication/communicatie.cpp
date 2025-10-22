#include <BluetoothSerial.h>

BluetoothSerial BTSerial;

void setup() {
  Serial.begin(115200);
  BTSerial.begin("ESP32_BT_DJ");

  Serial.println("Bluetooth is klaar. Verbind met 'ESP32_BT_DJ' via PuTTY.");
}

void loop() {
  if (BTSerial.available()) {
    String incoming = BTSerial.readString(); //
    Serial.print("Ontvangen via Bluetooth: ");
    Serial.println(incoming);

    String response = "ESP32 ontving: " + incoming;
    BTSerial.println(response);
  }
  if (Serial.available()) {
    String command = Serial.readString();
    BTSerial.println("Van USB: " + command);
  }
}
