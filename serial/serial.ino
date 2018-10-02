void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

}

void loop() {
  if (Serial.available()) {
    Serial.write(Serial.read());
  }
}
