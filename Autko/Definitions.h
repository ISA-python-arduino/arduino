//ENGINE PIN
#define A_ENABLE 2  //PWM
#define A_PHASE 46 //0 - przod, 1 - tyl DIGITAL
#define B_ENABLE 3  //PWM
#define B_PHASE 48  //0 - przod, 1 - tyl DIGITAL
#define MODE 44     //sterowanie 0 - ciężkie, 1 - uproszczone DIGITAL
#define LEFT_ENCODER_0 50
#define LEFT_ENCODER_1 51
#define RIGHT_ENCODER_0 42
#define RIGHT_ENCODER_1 43

enum class PowerSideEnum
{
  Left,
  Right
};

#define US_FRONT              0
#define US_FRONT_TRIGGER_PIN  8
#define US_FRONT_ECHO_PIN     9

#define US_BACK               1
#define US_BACK_TRIGGER_PIN   10
#define US_BACK_ECHO_PIN      11

#define US_LEFT               2
#define US_LEFT_TRIGGER_PIN   6
#define US_LEFT_ECHO_PIN      7

#define US_RIGHT              3
#define US_RIGHT_TRIGGER_PIN  4
#define US_RIGHT_ECHO_PIN     5
 
enum class UltraSoundSensor {
  Front = 0,
  Back = 1,
  Left = 2,
  Right = 3,
  
  __first = Front,
  __last = Right,
  
  All,
};
 

int ultrasound_trigger_pin[] = {
  [UltraSoundSensor::Front] = US_FRONT_TRIGGER_PIN,
  [UltraSoundSensor::Back]  = US_BACK_TRIGGER_PIN,
  [UltraSoundSensor::Left]  = US_LEFT_TRIGGER_PIN,
  [UltraSoundSensor::Right] = US_RIGHT_TRIGGER_PIN,
};

 
int ultrasound_echo_pin[] = {
  [UltraSoundSensor::Front] = US_FRONT_ECHO_PIN,
  [UltraSoundSensor::Back]  = US_BACK_ECHO_PIN,
  [UltraSoundSensor::Left]  = US_LEFT_ECHO_PIN,
  [UltraSoundSensor::Right] = US_RIGHT_ECHO_PIN,
};

//speeds

#define STOP 0
#define SPEED_0 0
#define SPEED_1 100
#define SPEED_2 150
#define SPEED_3 200

//directions

#define LEFT_FRONT LOW
#define RIGHT_FRONT HIGH
#define LEFT_BACK HIGH
#define RIGHT_BACK LOW

//LEDS

#define LED_PIN 12
#define LED_COUNT 4
#define LED_COLORS 14

//SERVO
#define SERVO_X 8
#define SERVO_Y 9

#define WIFI_SSID String("Czolg_1")
#define WIFI_PSWD String("12345678")
#define WIFI_CHL String("5")
#define WIFI_ENC String("3")
