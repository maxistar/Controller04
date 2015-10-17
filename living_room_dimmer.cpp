#ifndef LIVING_ROOM_DIMMER_H
#define LIVING_ROOM_DIMMER_H
#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include "dimmer_types.h"
#include "living_room_dimmer.h"

#define STATE_SWITCH_TIMEOUT 2000 

void LivingRoomDimmer::setup() {
    pinMode(this->buttonPin, INPUT);
    digitalWrite(this->buttonPin, HIGH);
    pinMode(this->light1Pin, OUTPUT);
    pinMode(this->light2Pin, OUTPUT);  
}

LivingRoomDimmer::LivingRoomDimmer(int buttonPin, int light1Pin, int light2Pin) {
    this->buttonPin = buttonPin;
    this->light1Pin = light1Pin;
    this->light2Pin = light2Pin;
  
    this->lightIsOn = 0;
    this->lightMode = 0;
    
    this->buttonPressedTime = 0;
    this->setupMode = 0;

    
    this->modeChangedLastTime = 0;
  
    this->debounceLastTime = 0;
    this->isDebouncing = 0;
  
}


void LivingRoomDimmer::checkSwitches()
{
    if (this->isDebouncing && (this->debounceLastTime + DEBOUNCE) > millis()) {
        return; // not enough time has passed to debounce
    }
    char currentstate = !digitalRead(this->buttonPin);   // read the button
  
    if (currentstate != this->pressed) {
        if (currentstate == 1) {
            // just pressed
            this->onButtonPressed();
            this->pressed = 1;
        }
        else if (currentstate == 0) {
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


void LivingRoomDimmer::onButtonPressed() {
    this->buttonPressedTime = millis();
    if (this->lightIsOn == 1) {
       this->lightIsOn = 0;
    }
    else {
       this->lightIsOn = 1;
    }
    this->applyLightMode();
}

void LivingRoomDimmer::onButtonReleased() {
    this->buttonPressedTime = 0;
    if (this->setupMode == 1) {
       this->setupMode = 0;
    }
}
  
void LivingRoomDimmer::doSetupModeLoop() {
    if ((this->modeChangedLastTime + STATE_SWITCH_TIMEOUT) > millis()) {
        return; // not enough time has passed
    }
    this->modeChangedLastTime = millis();
  
    if (this->lightMode<2) {
        this->lightMode++;
    }
    else {
        this->lightMode = 0;
    }
    this->applyLightMode();
}

void LivingRoomDimmer::applyLightMode() {
    if (this->lightIsOn == 0) {
        digitalWrite(this->light1Pin, LOW);
        digitalWrite(this->light2Pin, LOW); 
    }
    else if (this->lightMode == 0) {
        digitalWrite(this->light1Pin, HIGH);
        digitalWrite(this->light2Pin, HIGH); 
    }
    else if (this->lightMode == 1) {
        digitalWrite(this->light1Pin, LOW);
        digitalWrite(this->light2Pin, HIGH); 
    }
    else if (this->lightMode == 2) {
        digitalWrite(this->light1Pin, HIGH);
        digitalWrite(this->light2Pin, LOW); 
    }
}

void LivingRoomDimmer::onButtonKeepsPressed() {
   if (this->setupMode == 1) {
      this->doSetupModeLoop();
      return; //no need it - all will be handled in do Animation
   }
   if (this->buttonPressedTime + LONG_PRESS < millis()) {
       this->setupMode = 1;
       if (!this->lightIsOn) {
           this->lightIsOn = 1;
           this->applyLightMode();
       }
   }  
}

void LivingRoomDimmer::loop() {
    this->checkSwitches();      // when we check the switches we'll get the current state
 
    if (this->pressed) {
        this->onButtonKeepsPressed();
    }
}

#endif

