#include <SoftwareSerial.h>
#include "Motor.h"
#include "Btn.h"

//定义抬脚电机的控制引脚
const int DR1_pin = A3;
const int DR2_pin = A2;
const int Left_PWM_pin = 9;
const int Right_PWM_pin = 6;

const int DR1_back_pin = A1;
const int Back_L_PWM_pin = 3;
const int DR2_back_pin = A4;
const int Back_R_PWM_pin = 11;

SoftwareSerial mSerial(5,4);
//摇杆
const int ARX=A7;
const int ARY=A6;
const int ARIn=A5;
int V_X=0;
int V_Y=0;
int dt_range=40;//摇杆兼容范围（用于避免摇杆512非中心带偏移情况）

//控制状态枚举
enum class ControlState{
  Feet=0,
  Back,
  Wifi
}CState;

enum class WifiControl{
  L_Start=0,
  L_Close,
  R_Start,
  R_Close,
  B_Start,
  B_Close,
  StopAll,
}WIState;


//切换控制模式(按键回调)
void swicthMode(){
  int size=3;//模式数量
  CState=(ControlState)(((int)CState+1)%size);
  Serial.println("Switch Mode");
}

//定义电机类
Motor left_feet;//轮椅脚部控制类
Motor right_feet;//轮椅脚部控制类
Motor Back_motor_L;//背部电机
Motor Back_motor_R;//背部电机

MButton SwitchBtn(ARIn,swicthMode);

//足部电机一次性最大运动时间
int delaytime = 3000;


void WifiInputControl(){
  switch(WIState){
    case WifiControl::L_Start:{
      left_feet.Start();
      break;
    }
    case WifiControl::L_Close:{
      left_feet.Close();
      break;
    }
    case WifiControl::R_Start:{
      right_feet.Start();
      break;
    }
    case WifiControl::R_Close:{
      right_feet.Close();
      break;
    }
    case WifiControl::B_Start:{
      Back_motor_L.Start();
      Back_motor_R.Start();
      break;
    }
    case WifiControl::B_Close:{
      Back_motor_L.Close();
      Back_motor_R.Close();
      break;
    }
    case WifiControl::StopAll:{
      left_feet.Stop();
      right_feet.Stop();
      Back_motor_L.Stop();
      Back_motor_R.Stop();
      break;
    }
  }
}


//初始化输入
void InitInput() {
  pinMode(ARX, INPUT);
  pinMode(ARY, INPUT);
//  pinMode(ARIn, INPUT_PULLUP);
  mSerial.begin(9600);
}
//足部控制
void FeetControl(){
  //左脚控制
  if(V_Y>512+dt_range){
    left_feet.Start();
  }else if(V_Y<512-dt_range){
    left_feet.Close();
  }
  else{
    left_feet.Stop();
  }
  //右脚控制
  if(V_X>512+dt_range){
    right_feet.Start();
  }else if(V_X<512-dt_range){
    right_feet.Close();
  }
  else{
    right_feet.Stop();
  }
}
//背部控制
void BackControl(){
  //背部控制
  if(V_Y>512+dt_range){
    Back_motor_L.Start();
    Back_motor_R.Start();
  }else if(V_Y<512-dt_range){
    Back_motor_L.Close();
    Back_motor_R.Close();
  }
  else{
    Back_motor_L.Stop();
    Back_motor_R.Stop();
  }
}

//读取控制输入
void ReadInput() {
  
   
  V_X=analogRead(ARX);
  V_Y=analogRead(ARY);
  switch(CState){
    case ControlState::Feet:{
      FeetControl();
      break;
    }
    case ControlState::Back:{
      BackControl();
      break;
    }
    case ControlState::Wifi:{
      //Wifi控制
      if (mSerial.available() > 0) 
      {
          char input = mSerial.read();
          if(input=='s'||input=='S'){
            WIState=WifiControl::StopAll;
          }else{
            int v=(int)(input-'0');
            WIState=(WifiControl)v;
            Serial.println(v);
          }
      }
      WifiInputControl();
      break;
    }
  }
//  Serial.println(digitalRead(ARIn));
  //控制按键tick
  SwitchBtn.Tick();//切换模式按钮Tick
}

void setup() {
  InitInput();
  Serial.begin(9600);
  
  //初始化轮椅脚部控制类
  left_feet.init(DR1_pin,Left_PWM_pin);
  left_feet.SetDuration(delaytime);
  left_feet.SetStopStep(2);
  left_feet.ReSet();

  right_feet.init(DR2_pin,Right_PWM_pin);
  right_feet.SetStopStep(2);
  right_feet.SetDuration(delaytime);
  right_feet.ReSet();

  Back_motor_L.init(DR1_back_pin,Back_L_PWM_pin);
  Back_motor_L.SetDuration(delaytime+1000);
  Back_motor_L.SetStopStep(3);
  Back_motor_L.ReSet();
  
  Back_motor_R.init(DR2_back_pin,Back_R_PWM_pin);
  Back_motor_R.SetDuration(delaytime+1000);
  Back_motor_R.SetStopStep(3);
  Back_motor_R.ReSet();

  //默认控制状态
  CState=ControlState::Wifi;
  WIState=WifiControl::StopAll;
}

void loop() {
  //轮椅脚部控制tick
  left_feet.UpdataTick();
  right_feet.UpdataTick();
  Back_motor_L.UpdataTick();
  Back_motor_R.UpdataTick();
  
  ReadInput();
}
