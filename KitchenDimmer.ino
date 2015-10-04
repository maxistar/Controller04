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


int light2_button = 15;
int light2_led = 9;
int light2_on = 0;

 
// This handy macro lets us determine how big the array up above is, by checking the size
#define NUMBUTTONS 2

char previousstate[NUMBUTTONS];
char currentstate[NUMBUTTONS];
long debounceLastTime;


 
// we will track if a button is just pressed, just released, or 'currently pressed' 
char pressed[NUMBUTTONS], justpressed[NUMBUTTONS], justreleased[NUMBUTTONS];
 
void setup() {
  char i;
 
  // set up serial port
  Serial.begin(9600);
  Serial.print("Button checker with ");
  Serial.print(NUMBUTTONS, DEC);
  Serial.println(" buttons");
  // pin13 LED
  pinMode(13, OUTPUT);
 
  pinMode(light1_button, INPUT);
  digitalWrite(light1_button, HIGH);
  pinMode(light1_led, OUTPUT);

  pinMode(light2_button, INPUT);
  digitalWrite(light2_button, HIGH);
  pinMode(light2_led, OUTPUT);
}
 
void check_switches()
{
  char index;
 
  if ((debounceLastTime + DEBOUNCE) > millis()) {
    return; // not enough time has passed to debounce
  }
  // ok we have waited DEBOUNCE milliseconds, lets reset the timer
  debounceLastTime = millis();
 
  for (index = 0; index < NUMBUTTONS; index++) {
    justpressed[index] = 0;       // when we start, we clear out the "just" indicators
    justreleased[index] = 0;
 
    currentstate[0] = digitalRead(light1_button);   // read the button
    currentstate[1] = digitalRead(light2_button);   // read the button

    if (currentstate[index] == previousstate[index]) {
      if ((pressed[index] == LOW) && (currentstate[index] == LOW)) {
          // just pressed
          justpressed[index] = 1;
      }
      else if ((pressed[index] == HIGH) && (currentstate[index] == HIGH)) {
          // just released
          justreleased[index] = 1;
      }
      pressed[index] = !currentstate[index];  // remember, digital HIGH means NOT pressed
    }
    //Serial.println(pressed[index], DEC);
    previousstate[index] = currentstate[index];   // keep a running tally of the buttons
  }
}


void pressButton(int i) {
    switch(i) {
    case 0:
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
    break;
    case 1:
    if (light2_on == 1) {
       light2_on = 0;
       analogWrite(light2_led, 0);
    }
    else {
       light2_on = 1;
       analogWrite(light2_led, 255);
    }
    break; 
    }
}

void buttonReleased(int i) {
    switch(i) {
    case 0:
    light1_pressedTime = 0;
    if (light1_setupMode == 1) {
       light1_setupMode = 0;
       light1_max = light1_value;
       light1_step = 0;
    }
    break;
    case 1:

    break; 
    }
}



long lasttime = 0;
  
void doAnimation() {
  //if (millis() < lasttime) {
  //   lasttime = millis(); // we wrapped around, lets just try again
  //}
 
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
               Serial.println("change animation animation direction 1");
           }
           else {
               Serial.println("stop on animation, value ");
               Serial.print(light1_value);
               Serial.println("");
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
               Serial.println("change animation animation direction 2");
           }
           else {
               Serial.println("stop off animation, value");
               Serial.print(light1_value);
               Serial.println("");
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
       Serial.println("enter setup mode");
       if (light1_value == 0) {
          Serial.println("start switch on");
          light1_step = light1_stepSetup;
          light1_target = 255;
       }
       else {
          Serial.println("start switch off");
          light1_step = -light1_stepSetup;
          light1_target = 0;
       }
   }  
}

 
void loop() {
  check_switches();      // when we check the switches we'll get the current state
 
    if (justpressed[0]) {
      Serial.print(0, DEC);
      Serial.println(" Just pressed");
      pressButton(0);
      // remember, check_switches() will CLEAR the 'just pressed' flag
    }
    if (justreleased[0]) {
      Serial.print(0, DEC);
      Serial.println(" Just released");
      buttonReleased(0);
      // remember, check_switches() will CLEAR the 'just pressed' flag
    }
    if (pressed[0]) {
      //Serial.print(0, DEC);
      //Serial.println(" pressed");
      button1_keepPressed();
      // is the button pressed down at this moment
    }

  //button2
      if (justpressed[1]) {
      Serial.print(1, DEC);
      Serial.println(" Just pressed");
      pressButton(1);
      // remember, check_switches() will CLEAR the 'just pressed' flag
    }
    if (justreleased[1]) {
      Serial.print(1, DEC);
      Serial.println(" Just released");
      buttonReleased(1);
      // remember, check_switches() will CLEAR the 'just pressed' flag
    }
    if (pressed[1]) {
      //Serial.print(1, DEC);
      //Serial.println(" pressed");
      //button1_keepPressed();
      // is the button pressed down at this moment
    }

  
  doAnimation();
}
