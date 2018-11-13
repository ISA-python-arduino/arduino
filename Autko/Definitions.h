//ENGINE PIN

#define A_ENABLE 2  //PWM
#define A_PHASE 22  //0 - przod, 1 - tyl DIGITAL
#define B_ENABLE 3  //PWM
#define B_PHASE 23  //0 - przod, 1 - tyl DIGITAL
#define MODE 30     //sterowanie 0 - ciężkie, 1 - uproszczone DIGITAL
#define LEFT_ENCODER_0 40
#define LEFT_ENCODER_1 41
#define RIGHT_ENCODER_0 42
#define RIGHT_ENCODER_1 43


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
