#include "dimmertypes.h"
#include "simpledimmer.h"


SimpleDimmer sd1;
SimpleDimmer sd2;


 
void setup() {

  simpleDimmerSetup(4, 5, sd1);
  simpleDimmerSetup(6, 3, sd2);

}
 
void loop() {
    simpleDimmerLoop(sd1);
    simpleDimmerLoop(sd2);
}
