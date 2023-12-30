


#define power_button 2
#define enable_power_button 29



//RBD::Button buttonPower(power_button);

void power_sleep()
{
  Serial.println("sleep");

  //pinMode(power_button,  INPUT_PULLUP_SENSE);

  pinMode(enable_power_button,  INPUT_PULLUP_SENSE);

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

boolean powerbuttonstate = true;
void power_sence_sleep()
{
  int buttonState = analogRead(enable_power_button);
  if ((buttonState == 0) && (powerbuttonstate == true)) {
    Serial.print("pæower button2 = ");
  Serial.println(buttonState);
    powerbuttonstate = false;
  } else if ((buttonState >= 1) && (powerbuttonstate == false)) {
    Serial.print("pæower button2 = ");
  Serial.println(buttonState);
    powerbuttonstate = true;
    power_sleep();
  }

  /*
    if (buttonPower.onPressed()) {
    Serial.println("Button power off");

     //   power_sleep();
    }

    if (buttonPower.onReleased()) {
    Serial.println("Button power on");

    }

  */
}
