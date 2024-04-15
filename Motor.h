#include <Arduino.h>
#include <math.h>


//定义抬脚电机的控制引脚
// const int DR1 = A2;
// const int DR2 = A3;
// const int Left_PWM = 6;
// const int Right_PWM = 9;

//数学e
const float e = M_E;

enum class MotorState{
  Stop=0,
  Up,
  Down
};

class Motor {
public:
  Motor();
  ~Motor()=default;
  void init(int DR_pin=A2,int pwm_pin=6);
  void ReSet();
  void Start();
  void Close();
  void UpdataTick();
  void Stop();
  void SetDuration(int dur);
  void SetState(MotorState st);
  void SwitchState();
  static float Smoothing(float x);
  int CalculatePWM();
  bool IsStop(){
    return State==MotorState::Stop;
  }
  void SetStopStep(int step=1){
    stpopstep=step;
  }
private:
  //定义抬脚电机的控制引脚
  int DR;//电机方向控制 0反向 1正向
  int PWM;//转速控制 0-255
  int nowPos =0; //记录电机位置
  int dutyCycle = 0;                    // 初始占空比
  int duration = 3000;  // 过渡时间（毫秒）
  unsigned long startTime;              // 启动时间
  MotorState State;  //状态
  int tickdelay=5; //tick延迟
  int stpopstep=1;
};
