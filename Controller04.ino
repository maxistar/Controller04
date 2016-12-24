#include "dimmer_types.h"
#include "simple_switcher.h"


//кнопка
SimpleSwitcher sd1(A5, 7);


 
void setup() 
{
  sd1.setup();
}
 
void loop() 
{
    sd1.loop();
}
