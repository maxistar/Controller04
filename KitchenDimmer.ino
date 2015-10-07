#include "dimmer_types.h"
#include "simple_dimmer.h"
#include "simple_kitchen_dimmer.h"


SimpleDimmer sd1;
SimpleKitchenDimmer sd2;
LivingRoomDimmer sd3(1,2,3); //confirm pins when testing

 
void setup() {

  simpleDimmerSetup(4, 5, sd1);
  simpleKitchenDimmerSetup(6, 3, 12, sd2);

  sd3.init();
}
 
void loop() {
    simpleDimmerLoop(sd1);
    simpleKitchenDimmerLoop(sd2);
    
    sd3.loop();
}
