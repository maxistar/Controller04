#ifndef SIMPLE_DIMMER_H
#define SIMPLE_DIMMER_H
#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include "dimmer_types.h"
#include "simple_dimmer.h"


SimpleDimmer::SimpleDimmer(int buttonPin, int ledPin){
  // here is where we define the buttons that we'll use. button "1" is the first, button "6" is the 6th, etc
  this->light1_button = buttonPin;
  this->light1_led = ledPin;
  this->light1_on = 0;
  this->light1_max = 255;
  this->light1_state = 0; //0 ready
  this->light1_step = 0; //step to go
  this->light1_stepSetup = 1;
  this->light1_stepLight = 4;
  this->light1_value = 0;
  this->light1_target = 0;
  this->light1_setupMode = 0;
  this->light1_pressedTime = 0;
  this->lasttime = 0;
  this->debounceLastTime = 0;
  this->isDebouncing = 0;
} 

void SimpleDimmer::setup(){
  pinMode(this->light1_button, INPUT);
  digitalWrite(this->light1_button, HIGH);
  pinMode(this->light1_led, OUTPUT); 
} 


void SimpleDimmer::checkSwitches()
{
  if (this->isDebouncing && (this->debounceLastTime + DEBOUNCE) > millis()) {
    return; // not enough time has passed to debounce
  }
  this->currentstate = !digitalRead(this->light1_button);   // read the button
  
  if (this->currentstate != this->pressed) {
    if (this->currentstate == 1) {
          // just pressed
          this->onButtonPressed();
          this->pressed = 1;
    }
    else if (this->currentstate == 0) {
          // just released
          this->onButtonReleased();
          this->pressed = 0;
    }
    this->isDebouncing = 1;
  } 
  else {
    this->isDebouncing = 0; 
  }
  // ok we have waited DEBOUNCE milliseconds, lets reset the timer
  this->debounceLastTime = millis();
}


void SimpleDimmer::onButtonPressed() {
    this->light1_pressedTime = millis();
    if (this->light1_on == 1) {
       this->light1_on = 0;
       this->light1_target = 0;
       this->light1_step = -this->light1_stepLight;
    }
    else {
       this->light1_on = 1;
       this->light1_target = this->light1_max;
       this->light1_step = this->light1_stepLight;
    }
}

void SimpleDimmer::onButtonReleased() {
    this->light1_pressedTime = 0;
    if (this->light1_setupMode == 1) {
       this->light1_setupMode = 0;
       this->light1_max = this->light1_value;
       this->light1_step = 0;
    }
}
  
void SimpleDimmer::doAnimation() {
  if ((this->lasttime + ANIMATION_STEP) > millis()) {
    return; // not enough time has passed
  }
  this->lasttime = millis();
  
  if (this->light1_step == 0) return; //no need to animate
  
   if (this->light1_step > 0) {
       if (this->light1_value + this->light1_step > this->light1_target) {
           this->light1_value = this->light1_target;
           if (this->light1_setupMode == 1) {
               this->light1_step = -this->light1_step;
               this->light1_target = 0;
           }
           else {
               this->light1_step = 0; //stop animation
           }
       }
       else {
           this->light1_value += this->light1_step;
       }
       analogWrite(this->light1_led, this->light1_value);
   }
   else {
       if (this->light1_value + this->light1_step < this->light1_target) {
           this->light1_value = this->light1_target;
           if (this->light1_setupMode == 1) {
               this->light1_step = -1*this->light1_step;
               this->light1_target = 255;
           }
           else {
               this->light1_step = 0; //stop animation
           }
       }
       else {
           this->light1_value += this->light1_step;       
       }
       analogWrite(this->light1_led, this->light1_value);
   }
}

void SimpleDimmer::onButtonKeepsPressed() {
   if (this->light1_setupMode == 1) {
      return; //no need it 
   }
   if (this->light1_pressedTime + LONG_PRESS < millis()) {
       this->light1_setupMode = 1;
       if (this->light1_value == 0) {
          this->light1_step = this->light1_stepSetup;
          this->light1_target = 255;
       }
       else {
          this->light1_step = -1*this->light1_stepSetup;
          this->light1_target = 0;
       }
   }  
}

void SimpleDimmer::loop() {
    this->checkSwitches();      // when we check the switches we'll get the current state
 
    if (this->pressed) {
      this->onButtonKeepsPressed();
    }
    this->doAnimation();  
}

#endif

