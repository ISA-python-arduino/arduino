const int led = 13;

void setup() {
  Serial.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.setTimeout(50);
  pinMode(led, OUTPUT);
  
}

void loop() {
  if (Serial.available() > 0) {
    int x = Serial.parseInt();
    Serial.println(x);
    analogWrite(led, map(x, -100, 100, 0, 255));
  }
}
