const int led = 13;

void setup() {
  Serial.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.setTimeout(50);
  pinMode(led, OUTPUT);
  
}

struct control { 
  int code;
  unsigned byte s;
}

void loop() {
  struct control c;
  if (Serial.available() > sizeof(control)) {
    int x = Serial.readBytes(&c, sizeof(control));
    
  }
}
