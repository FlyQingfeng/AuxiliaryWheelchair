#include "Arduino.h"
#include "Motor.h"

Motor::Motor() {
}

void Motor::init(int DR_pin=A2,int pwm_pin=6) {
  Serial.begin(9600);
  DR=DR_pin;
  PWM=pwm_pin;
  //设置电机引脚为输出模式
  pinMode(DR, OUTPUT);
  pinMode(PWM, OUTPUT);
  Serial.println("init Motor");
}
//x 0~1
float Motor::Smoothing(float x) {
  if(x<0||x>1){
    return 0.f;
  }
  float y=0.f;
  if(x>0.5){
    y = 4*pow(1-x, 2) * (3 - 4*(1-x));
  }else{
    y = 4*pow(x, 2) * (3 - 4*(x));
  }
  return y;
}
void Motor::SwitchState(){
  switch (State) {
    case MotorState::Stop:
      {
        State = MotorState::Stop;
        break;
      }
    case MotorState::Up:
      {
        State = MotorState::Down;
        break;
      }
    case MotorState::Down:
      {
        State = MotorState::Up;
        break;
      }
  }
}
void Motor::SetDuration(int dur){
  duration=dur;
}

void Motor::Start() {
  if(State==MotorState::Up){
    return;
  }
  
  digitalWrite(DR, LOW);
  State = MotorState::Up;
  startTime=millis();
//  Serial.println("开始向上运动");
}
void Motor::Close() {
  if(State==MotorState::Down){
    return;
  }
  digitalWrite(DR, HIGH);
  State = MotorState::Down;
  startTime=millis();
//  Serial.println("开始向下运动");
}

void Motor::Stop() {
  if(State==MotorState::Stop){
    return;
  }
  State = MotorState::Stop;
//  Serial.println("停止运动");
}
void Motor::ReSet() {
//  Serial.println("复位");
}
void Motor::SetState(MotorState st) {
  State=st;
}
//计算PWM
int Motor::CalculatePWM() {
  unsigned long elapsedTime = millis() - startTime;
  int pwm=0;
  if (elapsedTime < duration) {
    float progress = float(elapsedTime) / float(duration);
    pwm = int(Smoothing(progress) * 255);
  } else {
    pwm = 0;
    SetState(MotorState::Stop);
  }
  return pwm;
}
//更新电机
void Motor::UpdataTick() {
  delay(tickdelay);
  switch (State) {
    case MotorState::Stop:
      {
        if(dutyCycle>0){
          dutyCycle-=stpopstep;
          analogWrite(PWM, dutyCycle);
        }else if(dutyCycle<0){
          dutyCycle+=1;
          analogWrite(PWM, dutyCycle);
        }
        else{
          analogWrite(PWM, 0);
        }
        break;
      }
    case MotorState::Up:
      {
        dutyCycle=CalculatePWM();
        analogWrite(PWM, dutyCycle);
        break;
      }
    case MotorState::Down:
      {
        dutyCycle=CalculatePWM();
        analogWrite(PWM, dutyCycle);
        break;
      }
  }
//  if(dutyCycle>0){
//    Serial.println(dutyCycle);
//  }
}
