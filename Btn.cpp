#include "Arduino.h"
#include "Btn.h"

MButton::MButton(int pin, Fun f,int m) {
  fun = f;
  Pin = pin;
  mod=m;
  if(mod==0){
    pinMode(Pin, INPUT_PULLUP);
  }else{
    pinMode(Pin, INPUT);
  }
}
void MButton::Cheek(){
  if (digitalRead(Pin) == LOW) {
      if (canPr == 1) {
        fun();
        cd = 0;
        canPr = 0;
      }
    } else {
      if (cd < maxcd) {
        cd += delaytick;
        canPr = 0;
      } else {
        canPr = 1;
      }
    }
    
}
void MButton::Tick() {
//   delay(delaytick);
  // Cheek();
  lasttick_time=millis();//软件按钮防抖
  
   delay(1);
  if (time < 1) {
    time+=millis()-lasttick_time;
//    Serial.println(millis());
  } else {
    time=0;
    Cheek();
  }
  
}
