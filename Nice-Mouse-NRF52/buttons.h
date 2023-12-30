/*
  #include "src\Buttons\Buttons.h"

  byte button_pins[] = {38, 32, 20, 36, 22, 24};
*/
#define left_button 38
#define right_button 11
#define wheel_button 22
#define top_button 33
#define back_button 8
#define forward_button 24

#include <RBD_Timer.h>  // https://github.com/alextaujenis/RBD_Timer
#include <RBD_Button.h> // https://github.com/alextaujenis/RBD_Button

RBD::Button buttonLeft(left_button);
RBD::Button buttonRight(right_button);
RBD::Button buttonWheel(wheel_button);
RBD::Button buttonTop(top_button);
RBD::Button buttonBack(back_button);
RBD::Button buttonForward(forward_button);


// Define constants for mouse buttons
#define MOUSE_BUTTON_LEFT     0b000001
#define MOUSE_BUTTON_RIGHT    0b000010
#define MOUSE_BUTTON_MIDDLE   0b000100
#define MOUSE_BUTTON_BACKWARD 0b001000
#define MOUSE_BUTTON_FORWARD  0b010000

int TOPtimes = 0;
boolean TOPpressed = false;
void TOPmultipress()
{
  TOPpressed = !TOPpressed;
  if (TOPpressed == false)
  {
    if (TOPtimes == 1)
    {
 //     myLamp.blink(Color::Blue);
    colorWipe(strip.Color(  0,   0,   255), BRIGHTNESS);    // Black/off
         
      timerBLINK.start();
    } else if (TOPtimes == 2)
    {
//      myLamp.blink(Color::Green);
    colorWipe(strip.Color(  0,   255,   0), BRIGHTNESS);    // Black/off
         
      timerBLINK.start();
    } else if (TOPtimes >= 3)
    {
    colorWipe(strip.Color(  0,   255,   255), BRIGHTNESS);    // Black/off
         
//      myLamp.blink(Color::Cyan);
      timerBLINK.start();
      adns_changeDPI();
    }
    TOPtimes = 0;
    timerTOPmulti.stop();
  }
}


void ButtonPress(byte button) {
  // Set the corresponding bit in the pressed_buttons byte
  mouse_buttons |= button;
  mouse_update = true;
}

void ButtonRelease(byte button) {
  // Set the corresponding bit in the released_buttons byte
  mouse_buttons &= ~button;
  mouse_update = true;
}

void buttons_loop() {


  if (buttonLeft.onPressed()) {
    Serial.println("Button Left Pressed");
    ButtonPress(MOUSE_BUTTON_LEFT);
  }

  if (buttonLeft.onReleased()) {
    Serial.println("Button Left Released");
    ButtonRelease(MOUSE_BUTTON_LEFT);
  }
  if (buttonRight.onPressed()) {
    Serial.println("Button Right Pressed");
    ButtonPress(MOUSE_BUTTON_RIGHT);
  }

  if (buttonRight.onReleased()) {
    Serial.println("Button Right Released");
    ButtonRelease(MOUSE_BUTTON_RIGHT);
  }

  if (buttonTop.onPressed()) {
    Serial.println("Button Top Pressed");
    if (timerBlinkRunning == false)
    {
      if (TOPtimes == 0)
      {
        timerTOPmulti.start();
      }
      TOPtimes++;
    }
  }
  if (buttonTop.onReleased()) {
    Serial.println("Button Top Released");

  }

  if (buttonWheel.onPressed()) {
    Serial.println("Button Wheel Pressed");
    ButtonPress(MOUSE_BUTTON_MIDDLE);
  }

  if (buttonWheel.onReleased()) {
    Serial.println("Button Wheel Released");
    ButtonRelease(MOUSE_BUTTON_MIDDLE);
  }


  if (buttonBack.onPressed()) {
    Serial.println("Button Back Pressed");
    ButtonPress(MOUSE_BUTTON_BACKWARD);
  }

  if (buttonBack.onReleased()) {
    Serial.println("Button Back Released");
    ButtonRelease(MOUSE_BUTTON_BACKWARD);
  }

  if (buttonForward.onPressed()) {
    Serial.println("Button Forward Pressed");
    ButtonPress(MOUSE_BUTTON_FORWARD);
  }

  if (buttonForward.onReleased()) {
    Serial.println("Button Forward Released");
    ButtonRelease(MOUSE_BUTTON_FORWARD);

  }

  // bluemicro_hid.mouseButtonPress(mouse_buttons);
}
