const int led = 13;

void setup() {
  Serial.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  pinMode(led, OUTPUT);
  
}

void loop() {
  if (Serial.available() > 0) {
    int x = Serial.read() + 96;
    int mapped_x = map(x, 1, 192, 0, 255);
    analogWrite(led, mapped_x);
  }
}
