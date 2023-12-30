
#include "RotaryEncoder.h"

#define PIN_A     34
#define PIN_B     39


void wheel_setup() {

  // Initialize Encoder
  RotaryEncoder.begin(PIN_A, PIN_B);

  // Start encoder
  RotaryEncoder.start();
}

void wheel_loop()
{
  int value = RotaryEncoder.read();

  if (value != 0)
  {
    mouse_wheel = value;
    mouse_update = true;
  } else {
    mouse_wheel = 0;
  }

}
