#ifndef CEILING_CONTROLLER_H
#define CEILING_CONTROLLER_H

#define PRESETS_COUNT 3

#define W1_ON_MASK 0x01
#define W2_ON_MASK 0x02
#define R_ON_MASK 0x04
#define G_ON_MASK 0x08
#define B_ON_MASK 0x0F

#define ANIMATION_STEP 25

struct LightsState {
  byte w1;
  byte w2;
  byte r;
  byte g;
  byte b;
};

class SimpleLedDimmer;

struct CeilingControllerConfig {
    byte pinW1;
    byte pinW2;
    byte pinR;
    byte pinG;
    byte pinB;
    byte pinPower;

    word *modbus;
    byte modbusReadRegisters;
    byte modbusReadWriteRegisters;
    byte modbusOnBit;
    byte modbusPresetNum;
    byte modbusW1;
    byte modbusW2;
    byte modbusR;
    byte modbusG;
    byte modbusB;
};

class CeilingController {
private:
    CeilingControllerConfig *config;
    byte currentPresetNum = 0;
    LightsState presets[PRESETS_COUNT];
    void restorePresets();
    char state = 0;
    byte powerState = 0;

    SimpleLedDimmer *w1;
    SimpleLedDimmer *w2;
    SimpleLedDimmer *r;
    SimpleLedDimmer *g;
    SimpleLedDimmer *b;

    void onLedPowerOn(byte mask);
    void onLedPowerOff(byte mask);
    friend void onPowerOnW1(CeilingController *c);
    friend void onPowerOffW1(CeilingController *c);
    friend void onPowerOnW2(CeilingController *c);
    friend void onPowerOffW2(CeilingController *c);
    friend void onPowerOnR(CeilingController *c);
    friend void onPowerOffR(CeilingController *c);
    friend void onPowerOnG(CeilingController *c);
    friend void onPowerOffG(CeilingController *c);
    friend void onPowerOnB(CeilingController *c);
    friend void onPowerOffB(CeilingController *c);


public:
    CeilingController(CeilingControllerConfig *config);
    void setup();
    void loop();
    void on();
    void off();
    char isOn();
    char isOff();
};

class SimpleLedDimmer {
    int ledPin;
    int powerOn = 0;
    int lightOn = 0;
    int lightStep = 0; //step to go
    int lightValue = 0;
    int lightTarget = 0;
    int lightStepLight = 4;
    uint16_t *maxValue;
    long lastTime;
    void (*powerOnCallback)(CeilingController *ceilingController) = NULL;
    void (*powerOffCallback)(CeilingController *ceilingController) = NULL;
    CeilingController *ceilingController;
    
    //modbus stuff
    uint16_t *modbus; 
    int maxValueIndex;

    void doAnimation();
    
    public:
    SimpleLedDimmer(CeilingController *ceilingController, int ledPin, uint16_t *maxValue);
    SimpleLedDimmer(CeilingController *ceilingController, int ledPin, uint16_t *maxValue, void (*powerOnCallback)(CeilingController *ceilingController), void (*powerOffCallback)(CeilingController *ceilingController));

    void setPowerOnCallback(void (*)(CeilingController *ceilingController));
    void setPowerOffCallback(void (*)(CeilingController *ceilingController));
    
    int isOn();
    int isOff();
    
    void on();
    void off();
    
    void loop();
    void setup();
};



void onPowerOnW1(CeilingController *c) {
    c->onLedPowerOn(W1_ON_MASK);
};

void onPowerOffW1(CeilingController *c) {
    c->onLedPowerOff(W1_ON_MASK);
};

void onPowerOnW2(CeilingController *c) {
    c->onLedPowerOn(W2_ON_MASK);
};

void onPowerOffW2(CeilingController *c) {
    c->onLedPowerOff(W2_ON_MASK);
};

void onPowerOnR(CeilingController *c) {
    c->onLedPowerOn(R_ON_MASK);
};

void onPowerOffR(CeilingController *c) {
    c->onLedPowerOff(R_ON_MASK);
};

void onPowerOnG(CeilingController *c) {
    c->onLedPowerOn(W2_ON_MASK);
};

void onPowerOffG(CeilingController *c) {
    c->onLedPowerOff(G_ON_MASK);
};

void onPowerOnB(CeilingController *c) {
    c->onLedPowerOn(B_ON_MASK);
};

void onPowerOffB(CeilingController *c) {
    c->onLedPowerOff(B_ON_MASK);
};

CeilingController::CeilingController(CeilingControllerConfig *config) {
    this->config = config;
    this->currentPresetNum = 0;
    this->restorePresets();

    w1 = new SimpleLedDimmer(this, config->pinW1, &config->modbus[config->modbusW1], onPowerOnW1, onPowerOffW1);
    w2 = new SimpleLedDimmer(this, config->pinW2, &config->modbus[config->modbusW2], onPowerOnW2, onPowerOffW2);
    r = new SimpleLedDimmer(this, config->pinR, &config->modbus[config->modbusR], onPowerOnR, onPowerOffR);
    g = new SimpleLedDimmer(this, config->pinG, &config->modbus[config->modbusG], onPowerOnG, onPowerOffG);
    b = new SimpleLedDimmer(this, config->pinB, &config->modbus[config->modbusB], onPowerOnB, onPowerOffB);

};

void CeilingController::onLedPowerOn(byte mask) {
    this->powerState = this->powerState || mask;  
    if (this->powerState != 0) {
      digitalWrite(this->config->pinPower, 1);
    }
};

void CeilingController::onLedPowerOff(byte mask) {
    this->powerState = this->powerState && !mask;
    if (this->powerState == 0) {
        digitalWrite(this->config->pinPower, 0);
    }
};

/**
 * Restore default presents
 */
void CeilingController::restorePresets() {
  int i;
  for(i=0;i<PRESETS_COUNT;i++) {
    this->presets[i].w1 = 255;
    this->presets[i].w2 = 0;
    this->presets[i].r = 0;
    this->presets[i].g = 0;
    this->presets[i].b = 0;
  };
};

void CeilingController::setup() {
  this->config->modbus[config->modbusPresetNum] = this->currentPresetNum;
};

void CeilingController::on() {
    this->state = 1;
    w1->on();
    w2->on();
    r->on();
    g->on();
    b->on();
};

void CeilingController::off() {
    this->state = 0;
    w1->off();
    w2->off();
    r->off();
    g->off();
    b->off();
};

char CeilingController::isOn() {
    return this->state;
};

char CeilingController::isOff() {
    return this->state == 0;
};

void CeilingController::loop() {
  if (this->config->modbus[this->config->modbusPresetNum] != this->currentPresetNum) {
     this->currentPresetNum = this->config->modbus[config->modbusPresetNum];
     //copy presets to modbus
     this->config->modbus[config->modbusW1] = this->presets[this->currentPresetNum].w1;
     this->config->modbus[config->modbusW2] = this->presets[this->currentPresetNum].w2;
     this->config->modbus[config->modbusR] = this->presets[this->currentPresetNum].r;
     this->config->modbus[config->modbusG] = this->presets[this->currentPresetNum].g;
     this->config->modbus[config->modbusB] = this->presets[this->currentPresetNum].b;
  }

  if (this->config->modbus[this->config->modbusW1] != this->presets[this->currentPresetNum].w1) {
      this->presets[this->currentPresetNum].w1 = this->config->modbus[this->config->modbusW1];
  }

  if (this->config->modbus[this->config->modbusW2] != this->presets[this->currentPresetNum].w2) {
      this->presets[this->currentPresetNum].w2 = this->config->modbus[this->config->modbusW2];
  }

  if (this->config->modbus[this->config->modbusR] != this->presets[this->currentPresetNum].r) {
      this->presets[this->currentPresetNum].r = this->config->modbus[this->config->modbusR];
  }

  if (this->config->modbus[this->config->modbusG] != this->presets[this->currentPresetNum].g) {
      this->presets[this->currentPresetNum].g = this->config->modbus[this->config->modbusG];
  }

  if (this->config->modbus[this->config->modbusB] != this->presets[this->currentPresetNum].b) {
      this->presets[this->currentPresetNum].b = this->config->modbus[this->config->modbusB];
  }
  
};

SimpleLedDimmer::SimpleLedDimmer(CeilingController *ceilingController, int ledPin, uint16_t *maxValue) {
  this->ledPin = ledPin;
  this->maxValue = maxValue;
  this->ceilingController = ceilingController;
} 

SimpleLedDimmer::SimpleLedDimmer(CeilingController *ceilingController, int ledPin, uint16_t *maxValue, void (*powerOnCallback)(CeilingController *ceilingController), void (*powerOffCallback)(CeilingController *ceilingController)) {
  this->ledPin = ledPin;
  this->maxValue = maxValue;
  this->ceilingController = ceilingController;
  this->powerOnCallback = powerOnCallback;
  this->powerOffCallback = powerOffCallback;
}

void SimpleLedDimmer::setup(){
    pinMode(this->ledPin, OUTPUT); 
} 

void SimpleLedDimmer::setPowerOnCallback(void (*callback)(CeilingController *ceilingController)) {
    powerOnCallback = callback;
}
void SimpleLedDimmer::setPowerOffCallback(void (*callback)(CeilingController *ceilingController)) {
    powerOffCallback = callback;
}



void SimpleLedDimmer::doAnimation() {
  if ((this->lastTime + ANIMATION_STEP) > millis()) {
    return; // not enough time has passed
  }
  this->lastTime = millis();
  
  if (this->lightStep == 0) return; //no need to animate
  
  if (this->lightStep > 0) { //going up
       if (this->lightValue + this->lightStep > this->lightTarget) {
           this->lightValue = this->lightTarget;
           this->lightStep = 0; //stop animation
       }
       else {
           this->lightValue += this->lightStep;
       }
       if (this->powerOn == 0) {
          this->powerOn = 1;
          this->powerOnCallback(ceilingController);
       }
       analogWrite(this->ledPin, 255-this->lightValue);
   }
   else { //going down
       if (this->lightValue + this->lightStep < this->lightTarget) {
           this->lightValue = this->lightTarget;           
           this->lightStep = 0; //stop animation
           if (this->lightValue == 0) {
               //power off
               this->powerOn = 0;
               this->powerOffCallback(ceilingController);                
           }
           
       }
       else {
           this->lightValue += this->lightStep;       
       }
       analogWrite(this->ledPin, 255-this->lightValue);
   }
}

void SimpleLedDimmer::loop() {
    this->doAnimation();
}

int SimpleLedDimmer::isOn() {
  return lightOn;  
}

int SimpleLedDimmer::isOff() {
  return lightOn == 0;  
}


void SimpleLedDimmer::on() {
    if (this->lightOn) return; //already on, nothing to do
    
    this->lightOn = 1;
    this->lightTarget = *maxValue;
    this->lightStep = this->lightStepLight;    
}

void SimpleLedDimmer::off() {
    if (this->lightOn == 0) return; //already off, nothing to do
    
    this->lightOn = 0;
    this->lightTarget = 0;
    this->lightStep = -this->lightStepLight;    
}


#endif
