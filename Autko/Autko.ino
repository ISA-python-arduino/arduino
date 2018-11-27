// Example 5 - parsing text and numbers with start and end markers in the stream

#include "helper.h"
#include <Wire.h> 
#include "Definitions.h"

#define addr 0x1E
const unsigned int number_of_mesures = 5;

const unsigned int frontSensorTrigger = 8;
const unsigned int frontSensorEcho = 9;
const unsigned int backSensorTrigger = 10;
const unsigned int backSensorEcho = 11;

const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];        // temporary array for use when parsing
dataPacket packet;

boolean newData = false;


void setup() {
  Serial.begin(57600);
  
  pinMode(A_ENABLE, OUTPUT);  //ustawienie pinów do PWM na output
  pinMode(B_ENABLE, OUTPUT);

  pinMode(A_PHASE, OUTPUT);   //ustawienie pinów do kierunku jazdy na output
  pinMode(B_PHASE, OUTPUT);

  pinMode(MODE, OUTPUT);      //ustawienie pinu rozdzaju sterowania na output
  digitalWrite(MODE, LOW);

  pinMode(frontSensorTrigger, OUTPUT);
  pinMode(frontSensorEcho, INPUT);

  pinMode(backSensorTrigger, OUTPUT);
  pinMode(backSensorEcho, INPUT);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.setTimeout(50);
  
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

void setLeftSpeed( short speed )
{
  if (speed < 0)
  {
    digitalWrite(B_PHASE, LOW);
  }
  else 
  {
    digitalWrite(B_PHASE, HIGH);
  }
  
  analogWrite(B_ENABLE, speed < 0 ? -speed : speed);
}

void setRightSpeed( short speed )
{
  if (speed < 0)
  {
    digitalWrite(A_PHASE, LOW);
  }
  else 
  {
    digitalWrite(A_PHASE, HIGH);
  }
  analogWrite(A_ENABLE, speed < 0 ? -speed : speed);
}

void setSpeed(short speed)
{
  setLeftSpeed(speed);
  setRightSpeed(speed);
}

volatile static bool straight = true; 


struct control { 
  int code;
  unsigned char s;
};
struct control c;
/*
void loop() {
  if (Serial.available() > 0) {
    String code = Serial.readStringUntil(';');
    c.code = code.toInt();
    String s = Serial.readStringUntil(';');
    c.s = s.toInt();
    
    setSpeed(c.s);
    Serial.print(c.s);
    Serial.print(' ');
    Serial.println(c.code);
  }
}
*/


/*z repo ISA */
void loop() {
    recvWithStartEndMarkers();
    if (newData == true) {
        strcpy(tempChars, receivedChars);
            // this temporary copy is necessary to protect the original data
            //   because strtok() used in parseData() replaces the commas with \0
        packet = parseData();

        if(packet.cordX < -5) {
          Serial.println("SKREC PRAWO ");
          setRightSpeed(60);
        }
        else if(packet.cordX > 5) {
          Serial.println("SKREC LEWO ");
          setLeftSpeed(60);
        } else {
          Serial.println("Jedz prosto ");
          setSpeed(100);
        }
      
        //setRightSpeed
        //setLeftSpeed
        
        showParsedData(packet);
        newData = false;
    }
}

//============

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

//============

dataPacket parseData() {      // split the data into its parts

    dataPacket tmpPacket;

    char * strtokIndx; // this is used by strtok() as an index

    strtokIndx = strtok(tempChars,",");      // get the first part - the string
    strcpy(tmpPacket.message, strtokIndx); // copy it to messageFromPC
 
    strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
    tmpPacket.cordX = atoi(strtokIndx);     // convert this part to an integer

    strtokIndx = strtok(NULL, ",");
    tmpPacket.cordY = atoi(strtokIndx);     // convert this part to a float
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
