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

const byte numChars = 256;
char receivedChars[numChars];
char tempChars[numChars];        // temporary array for use when parsing

boolean newData = false;


float dt = 20; //co ile pobiera się próbkę
int ep; //uchyb poprzedni
int en; //uchyb następny
int U; //sygnał sterujący
int C; //część całkująca
int Kp = 2; //wzmocnienie
int Ti = 4; //stała całkowania
int Td = 8; //stała różniczkowania


const int minimumX= -100;
const int maximumX = 100;

void flushReceiveSerial(){
  while(Serial.available() > 0) {
    char ch Serial.read();
  }
}

void setup() {
  Serial.begin(57600);

  Engine();
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.setTimeout(50);
  flushReceiveSerial();
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

volatile float distFront = 0.0f;
volatile float distBack = 0.0f;

volatile float frontMeasures[number_of_mesures];
volatile float backMeasures[number_of_mesures];
volatile int mesure_index = 0;

float minimum(int count, volatile float measures[])
{
  if (count < 2)
  {
    return min( measures[0], measures[1]);
  }
  
  float result = measures[0];
  for (int i = 1; i < count; i++)
  {
    result = min(result, measures[i]);
  }
  return result;
}


float remove_min_and_count_average(float minimum, int count, volatile float measures[])
{
  float sum = 0.0;
  for (int i = 0; i < count; i++)
  {
    sum += measures[i];
  }
  sum -= minimum;

  return sum / (float)(count - 1.0);
}

void average()
{
  frontMeasures[mesure_index] = measure(frontSensorTrigger, frontSensorEcho);
  backMeasures[mesure_index] = measure(backSensorTrigger, backSensorEcho);
  ++mesure_index;
  if ( mesure_index == number_of_mesures )
  {
    mesure_index = 0;
  }
  float minFront = minimum(number_of_mesures, frontMeasures);
  float minBack = minimum(number_of_mesures, backMeasures);
  
  distFront = remove_min_and_count_average(minFront, number_of_mesures, frontMeasures);
  distBack = remove_min_and_count_average(minBack, number_of_mesures, backMeasures);
}

void setLeftSpeed( short s )
{
  if (s < 0)
  {
    digitalWrite(B_PHASE, LOW);
  }
  else 
  {
    digitalWrite(B_PHASE, HIGH);
  }
  
  analogWrite(B_ENABLE, s < 0 ? -s : s);
}

void setRightSpeed( short s)
{
  if (s < 0)
  {
    digitalWrite(A_PHASE, LOW);
  }
  else 
  {
    digitalWrite(A_PHASE, HIGH);
  }
  analogWrite(A_ENABLE, s < 0 ? -s : s);
}

void setCustomSpeed(short s)
{
  setLeftSpeed(s);
  setRightSpeed(s);
}

volatile static bool straight = true; 


struct control { 
  int code;
  unsigned char s;
};
struct control c;

int getPidSpeed(int cordX) {
  return pid(cordX);
}

int pid(int x)
{
  en = x; //tutaj założyłem, że wartość uchybu poznajemy za pomocą odczytu z pinu A2
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

bool recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;

    if (Serial.available() > 0 && newData == false) {
        do {
        rc = Serial.read();
        Serial.print(rc);
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
        } while (recvInProgress);
        flushReceiveSerial();
        return true;
    }
    return false;
}

dataPacket parseData() {      // split the data into its parts

    dataPacket tmpPacket;

    char * strtokIndx; // this is used by strtok() as an index

    //strtokIndx = strtok(tempChars,",");      // get the first part - the string
    //strcpy(tmpPacket.message, strtokIndx); // copy it to messageFromPC
 
    strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
    tmpPacket.cordX = atoi(strtokIndx);     // convert this part to an integer

    strtokIndx = strtok(NULL, ",");
    tmpPacket.cordY = atoi(strtokIndx);     // convert this part to a float

    //Serial.println("CordX: ");
    //Serial.print(tmpPacket.cordX);
    return tmpPacket;
}


void showParsedData(dataPacket packet) {
    //Serial.print("Message ");
    //Serial.println(packet.message);
    Serial.print("CORDX ");
    Serial.println(packet.cordX);
    Serial.print("CORDY ");
    Serial.println(packet.cordY);
}

/*z repo ISA */
void loop() {
    if (recvWithStartEndMarkers()) {
        //Serial.println("Odebrano dane");
        strcpy(tempChars, receivedChars);
        dataPacket packet = parseData();
        showParsedData(packet);
        int cordX = packet.cordX;
        int carSpeed = getPidSpeed(cordX);
        
        if (carSpeed <= maximumX && carSpeed >= minimumX) {
          int s = map(carSpeed, minimumX, maximumX, 0, 105);
          Serial.print("Predkosc po pid ");
          Serial.println(s);
          Serial.print("CordX ");
          Serial.println(cordX);
          if (cordX >= -10 && cordX <= 10) {
             SetPowerLevel(PowerSideEnum::Right, 150);
             SetPowerLevel(PowerSideEnum::Left, 150);
//             Serial.println("Jedz prosto");
          } else if (cordX <= -10 && cordX >= -100) {
            /* skrecaj w lewo */
             SetPowerLevel(PowerSideEnum::Right, 150 + s);
             SetPowerLevel(PowerSideEnum::Left, 150);
//             Serial.println("Skrecaj w lewo");
          } else if (cordX >= 10 && cordX <= 100) {
            /* skrecaj w prawo */
             SetPowerLevel(PowerSideEnum::Right, 150);
             SetPowerLevel(PowerSideEnum::Left, 150 + s);
//             Serial.println("Skrecaj w prawo");
          }
        newData = false;
    }
  }
}
