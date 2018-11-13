void MovementDirection(bool leftDirection, bool rightDirection);
void MovementSpeed(int speed);

int LeftCount = 0;
int RightCount = 0;

int actualSpeed = STOP;

void LeftCounter(){
  LeftCount--;
}

void RightCounter(){
  RightCount--;
}


//=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0
void Engine(){
  pinMode(A_ENABLE, OUTPUT);  //ustawienie pinów do PWM na output
  pinMode(B_ENABLE, OUTPUT);

  pinMode(A_PHASE, OUTPUT);   //ustawienie pinów do kierunku jazdy na output
  pinMode(B_PHASE, OUTPUT);

  pinMode(MODE, OUTPUT);      //ustawienie pinu rozdzaju sterowania na output
  digitalWrite(MODE, HIGH);   //ustawienie uproszczonego sterowania ;P

  pinMode(LEFT_ENCODER_0, INPUT);         //uruchomienie enkoderóœ
  pinMode(RIGHT_ENCODER_0, INPUT);

  attachInterrupt(digitalPinToInterrupt(LEFT_ENCODER_0), RightCounter, CHANGE);
  attachInterrupt(digitalPinToInterrupt(RIGHT_ENCODER_0), LeftCounter, CHANGE);

  MovementDirection(LEFT_FRONT, RIGHT_FRONT);
  MovementSpeed(0);
}

//=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0
void MovementDirection(bool leftDirection, bool rightDirection){
  digitalWrite(A_PHASE, leftDirection);
  digitalWrite(B_PHASE, rightDirection);
}

//=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0=0
void MovementSpeed(int speed){
    if(speed < -1)
        speed = -1;
    if(speed > 3)
        speed = 3;
    
//    if(speed < 0)
//        MovementDirection(LEFT_BACK, RIGHT_BACK);
//    else
//        MovementDirection(LEFT_FRONT, RIGHT_FRONT);
    
    switch(speed){
        case -1:
            speed = SPEED_1;
            break;
        case 0:
            speed = STOP;
            break;
        case 1:
            speed = SPEED_1;
            break;
        case 2:
            speed = SPEED_2;
            break;
        case 3:
            speed = SPEED_3;
            break;
    }
    analogWrite(A_ENABLE, speed);
    analogWrite(B_ENABLE, speed);
}

