#include <Arduino.h>


typedef void (*Fun)();//参数和返回值都为void*指针

class MButton{
public:
  MButton(int pin,Fun f,int m=0);//传入读取引脚，传入按下回调函数
  ~MButton()=default;
  void Tick();
  void Cheek();
private:
  int Pin=0;      //按钮引脚
  int cd=0;       //当前cd时间
  int maxcd=500;  //最大cd
  long time=0;     //计时器
  unsigned long lasttick_time=0;//上一tick时间
  int delaytick=10;//tick延迟
  int canPr=1;   //按钮是否可以按下
  Fun fun;
  int mod=0;
};
