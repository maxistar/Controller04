#ifndef SIMPLE_DIMMER_H
#define SIMPLE_DIMMER_H
#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include "dimmertypes.h"

#define DEBOUNCE 50  // button debouncer, how many ms to debounce, 5+ ms is usually plenty
#define ANIMATION_STEP 10
#define LONG_PRESS 2000  //how long to go into setup mode


void simpleDimmerSetup(int buttonPin, int ledPin, SimpleDimmer &sd){
  // here is where we define the buttons that we'll use. button "1" is the first, button "6" is the 6th, etc
  sd.light1_button = buttonPin;
  sd.light1_led = ledPin;
  sd.light1_on = 0;
  sd.light1_max = 255;
  sd.light1_state = 0; //0 ready
  sd.light1_step = 0; //step to go
  sd.light1_stepSetup = 1;
  sd.light1_stepLight = 4;
  sd.light1_value = 0;
  sd.light1_target = 0;
  sd.light1_setupMode = 0;
  sd.light1_pressedTime = 0;
  sd.lasttime = 0;
  sd.debounceLastTime = 0;
  sd.isDebouncing = 0;

  
  pinMode(sd.light1_button, INPUT);
  digitalWrite(sd.light1_button, HIGH);
  pinMode(sd.light1_led, OUTPUT); 
} 

void check_switches(SimpleDimmer &sd)
{
  sd.justreleased = 0;
  sd.justpressed = 0;
  
  if (sd.isDebouncing && (sd.debounceLastTime + DEBOUNCE) > millis()) {
    return; // not enough time has passed to debounce
  }
  sd.currentstate = !digitalRead(sd.light1_button);   // read the button
  
  if (sd.currentstate != sd.pressed) {
    if (sd.currentstate == 1) {
          // just pressed
          sd.justpressed = 1;
          sd.pressed = 1;
    }
    else if (sd.currentstate == 0) {
          // just released
          sd.justreleased = 1;
          sd.pressed = 0;
    }
    sd.isDebouncing = 1;
  } 
  else {
    sd.isDebouncing = 0; 
  }
  // ok we have waited DEBOUNCE milliseconds, lets reset the timer
  sd.debounceLastTime = millis();
}


void pressButton(SimpleDimmer &sd) {
    sd.light1_pressedTime = millis();
    if (sd.light1_on == 1) {
       sd.light1_on = 0;
       sd.light1_target = 0;
       sd.light1_step = -sd.light1_stepLight;
    }
    else {
       sd.light1_on = 1;
       sd.light1_target = sd.light1_max;
       sd.light1_step = sd.light1_stepLight;
    }
}

void buttonReleased(SimpleDimmer &sd) {
    sd.light1_pressedTime = 0;
    if (sd.light1_setupMode == 1) {
       sd.light1_setupMode = 0;
       sd.light1_max = sd.light1_value;
       sd.light1_step = 0;
    }
}
  
void doAnimation(SimpleDimmer &sd) {
  if ((sd.lasttime + ANIMATION_STEP) > millis()) {
    return; // not enough time has passed
  }
  sd.lasttime = millis();
  
  if (sd.light1_step == 0) return; //no need to animate
  
   if (sd.light1_step > 0) {
       if (sd.light1_value + sd.light1_step > sd.light1_target) {
           sd.light1_value = sd.light1_target;
           if (sd.light1_setupMode == 1) {
               sd.light1_step = -sd.light1_step;
               sd.light1_target = 0;
           }
           else {
               sd.light1_step = 0; //stop animation
           }
       }
       else {
           sd.light1_value += sd.light1_step;
       }
       analogWrite(sd.light1_led, sd.light1_value);
   }
   else {
       if (sd.light1_value + sd.light1_step < sd.light1_target) {
           sd.light1_value = sd.light1_target;
           if (sd.light1_setupMode == 1) {
               sd.light1_step = -1*sd.light1_step;
               sd.light1_target = 255;
           }
           else {
               sd.light1_step = 0; //stop animation
           }
       }
       else {
           sd.light1_value += sd.light1_step;       
       }
       analogWrite(sd.light1_led, sd.light1_value);
   }
}

void button1_keepPressed(SimpleDimmer &sd) {
   if (sd.light1_setupMode == 1) {
      return; //no need it 
   }
   if (sd.light1_pressedTime + LONG_PRESS < millis()) {
       sd.light1_setupMode = 1;
       if (sd.light1_value == 0) {
          sd.light1_step = sd.light1_stepSetup;
          sd.light1_target = 255;
       }
       else {
          sd.light1_step = -1*sd.light1_stepSetup;
          sd.light1_target = 0;
       }
   }  
}

void simpleDimmerLoop(SimpleDimmer &sd) {
  check_switches(sd);      // when we check the switches we'll get the current state
 
    if (sd.justpressed) {
      pressButton(sd);
    }
    if (sd.justreleased) {
      buttonReleased(sd);

    }
    if (sd.pressed) {
      button1_keepPressed(sd);
    }
  doAnimation(sd);  
}

#endif

