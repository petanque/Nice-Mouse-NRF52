


#define power_pin 13

int adnsX;
int adnsY;
int adnsX_Last;
int adnsY_Last;


// Initialize the mouse control bytes
byte mouse_buttons = 0;
int mouse_wheel = 0;

boolean mouse_update = false;

#include "ADNS3050.h"
#include <SPI.h>

#include "timer.h"
#include "timerManager.h"
Timer timerADNS;
Timer timerSLEEP;
Timer timerBLINK;
Timer timerTOPmulti;
Timer timerBLINKlowBatt;
Timer timerMeasureBatt;
Timer timerPowerButtonDetect;

boolean timerBlinkRunning = false;
byte TOPtimerColour = 0; //0 = none, 1 = blue, 2 = red, 3 = ???

#include "lights.h"

volatile boolean Sleeping = false;

#include "Bluefruit_hid.h"


#include "buttons.h"
#include "MouseWheel.h"

#include "measure_batt.h"
#include "power_switch.h"

void read_adns()
{
  getXY();
}
void go2sleep()
{
  Serial.println("sleep");

  pinMode(left_button,  INPUT_PULLUP_SENSE);
  pinMode(right_button,  INPUT_PULLUP_SENSE);
  pinMode(wheel_button,  INPUT_PULLUP_SENSE);
  pinMode(top_button,  INPUT_PULLUP_SENSE);
  pinMode(back_button,  INPUT_PULLUP_SENSE);
  pinMode(forward_button,  INPUT_PULLUP_SENSE);
  //pinMode(2,   INPUT_PULLUP_SENSE);
  //pinMode(29,  INPUT_PULLUP_SENSE);


  timerBLINK.stop();
  timerTOPmulti.stop();
  timerBLINKlowBatt.stop();

  adns_sleep();

  digitalWrite(power_pin, LOW);

  delay(100);

  Sleeping = true;

  // power down nrf52.
  sd_power_system_off();

}


void setup() {

  Serial.begin(115200);

  pinMode(power_pin, OUTPUT);
  digitalWrite(power_pin, LOW);
  delay(100);
  digitalWrite(power_pin, HIGH);
  delay(100);

  adns_startup();

  lights_startup();


  pinMode(PIN_MOTION, INPUT);

  //Set the interval in milliseconds we want to read the adns chip
  timerADNS.setInterval(1);
  timerADNS.setCallback(read_adns);

  //Set the interval in milliseconds when we want to go to sleep
  timerSLEEP.setInterval(90000);
  timerSLEEP.setCallback(go2sleep);

  //Set the interval in milliseconds when we want to go to BLINK
  timerBLINK.setInterval(200);
  timerBLINK.setCallback(goBlink);

  //Set the interval in milliseconds when we want to go to check for multiple button presses
  timerTOPmulti.setInterval(500);
  timerTOPmulti.setCallback(TOPmultipress);

  //Set the interval in milliseconds when we want to go to BLINK for low batt
  timerBLINKlowBatt.setInterval(200);
  timerBLINKlowBatt.setCallback(goBlinkLowBatt);



  //Set the interval in milliseconds when we want to check batteryu per
  timerMeasureBatt.setInterval(60000);
  timerMeasureBatt.setCallback(goMeasureBatt);


  //Set the interval in milliseconds when we want to go to check for multiple button presses
  timerPowerButtonDetect.setInterval(500);
  timerPowerButtonDetect.setCallback(power_sence_sleep);


  //Start all the timers
  TimerManager::instance().start();
  timerBLINK.stop();
  timerTOPmulti.stop();
  timerBLINKlowBatt.stop();


  wheel_setup();
  batt_startup();

  hid_setup();
}

void loop() {
  
  power_sence_sleep();

  if (Sleeping == true)
  {
    //   digitalWrite(PIN_SCLK, LOW);
    //   digitalWrite(PIN_NCS, LOW);
    //   digitalWrite(PIN_MOSI, LOW);
    //   digitalWrite(PIN_MISO, LOW);
    delay(100);
    digitalWrite(power_pin, HIGH);
    delay(100);
    adns_startup();
    Serial.println("Woke from sleep");
    Sleeping = false;
    StartupMeasureBatt();
  }


  //Update all the timers at once
  TimerManager::instance().update();

  if ((adnsX_Last != adnsX) || (adnsY_Last != adnsY))
  {
    Serial.print("x ");
    Serial.println(adnsX);
    Serial.print("y ");
    Serial.println(adnsY);
//        bluemicro_hid.mouseMove(adnsX, adnsY);
    adnsX_Last = adnsX;
    adnsY_Last = adnsY;

    mouse_update = true;
  }

  lights_loop();

  buttons_loop();
  wheel_loop();

  if (mouse_update == true)
  {
    blehid.mouseReport(mouse_buttons, adnsX, adnsY, mouse_wheel);
    timerSLEEP.reset();
    mouse_update = false;
    //Serial.println("Mouse Update");
  }


  //NRF_EVT_POWER_USB_DETECTED = 19
  //NRF_EVT_POWER_USB_POWER_READY = 9
  //NRF_EVT_POWER_USB_REMOVED = 11
  //Serial.println(usbpwrrdy_enable);
}
