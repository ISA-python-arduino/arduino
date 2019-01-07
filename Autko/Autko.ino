#include "helper.h"
#include <Wire.h> 
#include "Definitions.h"
#include "Engine.h"

#define addr 0x1E
const unsigned int number_of_mesures = 5;

const unsigned int frontSensorTrigger = 8;
const unsigned int frontSensorEcho = 9;
const unsigned int backSensorTrigger = 10;
const unsigned int backSensorEcho = 11;

const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];        // temporary array for use when parsing

boolean newData = false;

volatile float distFront = 0.0f;
volatile float distBack = 0.0f;

volatile float frontMeasures[number_of_mesures];
volatile float backMeasures[number_of_mesures];
volatile int mesure_index = 0;

unsigned long t;
float dt = 20; //co ile pobiera się próbkę
int ep = 0; //uchyb poprzedni
int en; //uchyb następny
int U; //sygnał sterujący
int C; //część całkująca
int Kp = 1; //wzmocnienie
int Ti = 8; //stała całkowania
float Td = 1.05; //stała różniczkowania

const int minimumX= -100;
const int maximumX = 100;

void setUltraSound(int us) {
  pinMode(ultrasound_trigger_pin[us], OUTPUT);
  pinMode(ultrasound_echo_pin[us], INPUT);
  digitalWrite(ultrasound_trigger_pin[us],0);
}

void setup() {
  Serial.begin(57600);
  setUltraSound(US_FRONT);
  Engine();
  while (!Serial) {
  }
  t = millis();
}

float measure( int trigger, int echo )
{
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);

  float result = pulseIn(echo, HIGH) / 58.0f;

  if (result < 5.0f) 
  {
    return measure(trigger, echo);
  }

  return result > 200.0 ? 200.0 : result;
}

int getPid(int value) {
  return pid(value);
}

int pid(int x)
{
  unsigned long currentTime = millis();
  dt = currentTime - t;
  t = currentTime;
  en = x;
  C += ((ep + en)/2)*dt;
  U = Kp*(en + (1/Ti)*C/1000 + Td*(en - ep)*1000/dt);
  ep = en;
  return U;
}

void SetPowerLevel(PowerSideEnum side, int level)
{
  level = constrain(level, -255, 255);
  
  if (side == PowerSideEnum::Right) {
    if (level > 0) {
      // do przodu
      digitalWrite(A_PHASE, 1);
      analogWrite(A_ENABLE, level);
    } else if (level < 0) {
      // do tyłu
      digitalWrite(A_PHASE, 0);
      analogWrite(A_ENABLE, -level);
    } else {
      // stop
      digitalWrite(A_PHASE, 0);
      analogWrite(A_ENABLE, 0);
    }
  }
  
  if (side == PowerSideEnum::Left) {
    if (level > 0) {
      // do przodu
      digitalWrite(B_PHASE, 1);
      analogWrite(B_ENABLE, level);
    } else if (level < 0) {
      // do tyłu
      digitalWrite(B_PHASE, 0);
      analogWrite(B_ENABLE, -level);
    } else {
      // stop
      digitalWrite(B_PHASE, 0);
      analogWrite(B_ENABLE, 0);
    }
  } 
}

void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;

    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

dataPacket parseData() {
    dataPacket tmpPacket;
    char * strtokIndx; 
    strtokIndx = strtok(tempChars,",");
    strcpy(tmpPacket.message, strtokIndx);
    strtokIndx = strtok(NULL, ",");
    tmpPacket.cordX = atoi(strtokIndx);
    strtokIndx = strtok(NULL, ",");
    tmpPacket.cordY = atoi(strtokIndx);
    return tmpPacket;
}


void showParsedData(dataPacket packet) {
    Serial.print("Message ");
    Serial.println(packet.message);
    Serial.print("CORDX ");
    Serial.println(packet.cordX);
    Serial.print("CORDY ");
    Serial.println(packet.cordY);
}

void loop() {
  float distance = measure(US_FRONT_TRIGGER_PIN, US_FRONT_ECHO_PIN);
  
  recvWithStartEndMarkers();
  if (newData && distance > 20.0f) {
    strcpy(tempChars, receivedChars);
    dataPacket packet = parseData();
    int cordX = packet.cordX;
    int s = map(abs(cordX), 0, maximumX, 0, 105);
    int pidSpeed = getPid(s);
    if (pidSpeed < 0) { 
      pidSpeed = 0;
    } else if (pidSpeed > 105) {
      pidSpeed = 105;
    }
    if (cordX >= -10 && cordX <= 10) {
       SetPowerLevel(PowerSideEnum::Right, 150);
       SetPowerLevel(PowerSideEnum::Left, 150);
    } else if (cordX <= -10 && cordX >= -100) {
       SetPowerLevel(PowerSideEnum::Right, 150 + s);
       SetPowerLevel(PowerSideEnum::Left, 150);
    } else if (cordX >= 10 && cordX <= 100) {
       SetPowerLevel(PowerSideEnum::Right, 150);
       SetPowerLevel(PowerSideEnum::Left, 150 + s);
    }   
    newData = false;
  } else if (distance < 20) {
    SetPowerLevel(PowerSideEnum::Right, 0);
    SetPowerLevel(PowerSideEnum::Left, 0);
  }
}
