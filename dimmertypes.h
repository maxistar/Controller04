struct SimpleDimmer {
// here is where we define the buttons that we'll use. button "1" is the first, button "6" is the 6th, etc
int light1_button;
int light1_led;
int light1_on;
int light1_max;
int light1_state; //0 ready
int light1_step; //step to go
int light1_stepSetup;
int light1_stepLight;
int light1_value;
int light1_target;
int light1_setupMode;
long light1_pressedTime;

char currentstate;
char isDebouncing;
long debounceLastTime;
long lasttime;

// we will track if a button is just pressed, just released, or 'currently pressed' 
char pressed; 
char justpressed;
char justreleased;

};

