#define DEBOUNCE 10  // button debouncer, how many ms to debounce, 5+ ms is usually plenty
#define ANIMATION_STEP 10
#define LONG_PRESS 2000  //how long to go into setup mode
// here is where we define the buttons that we'll use. button "1" is the first, button "6" is the 6th, etc
int light1_button = 4;
int light1_led = 5;
int light1_on = 0;
int light1_max = 255;
int light1_state = 0; //0 ready
int light1_step = 0; //step to go
int light1_stepSetup = 1;
int light1_stepLight = 4;
int light1_value = 0;
int light1_target = 0;
int light1_setupMode = 0;
long light1_pressedTime = 0;

char previousstate;
char currentstate;
long debounceLastTime;


 
// we will track if a button is just pressed, just released, or 'currently pressed' 
char pressed, justpressed, justreleased;
 
void setup() {
  pinMode(light1_button, INPUT);
  digitalWrite(light1_button, HIGH);
  pinMode(light1_led, OUTPUT);

}
 
void check_switches()
{
  char index;
 
  if ((debounceLastTime + DEBOUNCE) > millis()) {
    return; // not enough time has passed to debounce
  }
  // ok we have waited DEBOUNCE milliseconds, lets reset the timer
  debounceLastTime = millis();
 
    justpressed = 0;       // when we start, we clear out the "just" indicators
    justreleased = 0;
 
    currentstate = digitalRead(light1_button);   // read the button

    if (currentstate == previousstate) {
      if ((pressed == LOW) && (currentstate == LOW)) {
          // just pressed
          justpressed = 1;
      }
      else if ((pressed == HIGH) && (currentstate == HIGH)) {
          // just released
          justreleased = 1;
      }
      pressed = !currentstate;  // remember, digital HIGH means NOT pressed
    }
    
    previousstate = currentstate;   // keep a running tally of the buttons
}


void pressButton() {
    light1_pressedTime = millis();
    if (light1_on == 1) {
       light1_on = 0;
       light1_target = 0;
       light1_step = -light1_stepLight;
    }
    else {
       light1_on = 1;
       light1_target = light1_max;
       light1_step = light1_stepLight;
    }
}

void buttonReleased() {
    light1_pressedTime = 0;
    if (light1_setupMode == 1) {
       light1_setupMode = 0;
       light1_max = light1_value;
       light1_step = 0;
    }
}



long lasttime = 0;
  
void doAnimation() {
  if ((lasttime + ANIMATION_STEP) > millis()) {
    return; // not enough time has passed
  }
  lasttime = millis();
  
  if (light1_step == 0) return; //no need to animate
  
   if (light1_step > 0) {
       if (light1_value + light1_step > light1_target) {
           light1_value = light1_target;
           if (light1_setupMode == 1) {
               light1_step = -light1_step;
               light1_target = 0;
           }
           else {
               light1_step = 0; //stop animation
           }
       }
       else {
           light1_value += light1_step;
       }
       analogWrite(light1_led, light1_value);
   }
   else {
       if (light1_value + light1_step < light1_target) {
           light1_value = light1_target;
           if (light1_setupMode == 1) {
               light1_step = -light1_step;
               light1_target = 255;
           }
           else {
               light1_step = 0; //stop animation
           }
       }
       else {
           light1_value += light1_step;       
       }
       analogWrite(light1_led, light1_value);
   }
}

void button1_keepPressed() {
   if (light1_setupMode == 1) {
      return; //no need it 
   }
   if (light1_pressedTime + LONG_PRESS < millis()) {
       light1_setupMode = 1;
       if (light1_value == 0) {
          light1_step = light1_stepSetup;
          light1_target = 255;
       }
       else {
          light1_step = -light1_stepSetup;
          light1_target = 0;
       }
   }  
}

 
void loop() {
  check_switches();      // when we check the switches we'll get the current state
 
    if (justpressed) {
      pressButton();
    }
    if (justreleased) {
      buttonReleased();
    }
    if (pressed) {
      button1_keepPressed();
    }
  doAnimation();
}
