#define enable_measure 36
#define read_batt A7 //31

//1,02/3


uint32_t vbat_pin = read_batt;             // A7 for feather nRF52832, A6 for nRF52840

#define VBAT_MV_PER_LSB   (0.73242188F)   // 3.0V ADC range and 12-bit ADC resolution = 3000mV/4096

#define VBAT_DIVIDER      (0.5F)          // 150K + 150K voltage divider on VBAT
#define VBAT_DIVIDER_COMP (2.05F)          // Compensation factor for the VBAT divider

#define REAL_VBAT_MV_PER_LSB (VBAT_DIVIDER_COMP * VBAT_MV_PER_LSB)


float vbat_mv = 0;
uint8_t vbat_per = 0;


float readVBAT(void) {

  digitalWrite(enable_measure, HIGH);

  float raw;

  // Set the analog reference to 3.0V (default = 3.6V)
  analogReference(AR_INTERNAL_3_0);

  // Set the resolution to 12-bit (0..4095)
  analogReadResolution(12); // Can be 8, 10, 12 or 14

  // Let the ADC settle
  delay(1);

  // Get the raw 12-bit, 0..3000mV ADC value
  raw = analogRead(vbat_pin);

  // Set the ADC back to the default settings
  analogReference(AR_DEFAULT);
  analogReadResolution(10);


  digitalWrite(enable_measure, LOW);

  // Convert the raw value to compensated mv, taking the resistor-
  // divider into account (providing the actual LIPO voltage)
  // ADC range is 0..3000mV and resolution is 12-bit (0..4095)
  return raw * REAL_VBAT_MV_PER_LSB;
}

uint8_t mvToPercent(float mvolts) {
  if (mvolts < 3300)
    return 0;

  if (mvolts < 3600) {
    mvolts -= 3300;
    return mvolts / 30;
  }

  mvolts -= 3600;
  return 10 + (mvolts * 0.15F );  // thats mvolts /6.66666666
}


void StartupMeasureBatt()
{

  readVBAT();

  delay(100);
  
  // Get a raw ADC reading
  vbat_mv = readVBAT();

  // Convert from raw mv to percentage (based on LIPO chemistry)
  vbat_per = mvToPercent(vbat_mv);

  // Display the results

  Serial.print("LIPO = ");
  Serial.print(vbat_mv);
  Serial.print(" mV (");
  Serial.print(vbat_per);
  Serial.println("%)");

  //  blebas.write(vbat_per);
  //  blebas.notify(vbat_per);

  if (vbat_per < 10)
  {
    timerBLINKlowBatt.start();
  } else if ((vbat_per >= 10) && (vbat_per < 40))
  {
  
    colorWipe(strip.Color(  255,   255,   0), BRIGHTNESS); 
  //  myLamp.setColor(Color::Purple);
    delay(500);
    
    colorWipe(strip.Color(  0,   0,   0), 50);    // Black/off
          
 //   myLamp.off();
 //   analogWrite(led_power_bottom, 255);
  } else if ((vbat_per >= 40) && (vbat_per < 70))
  {
    colorWipe(strip.Color(  0,   255,   255), BRIGHTNESS);
 //   analogWrite(led_power_bottom, 240);
 //   myLamp.setColor(Color::Yellow);
    delay(500);
//    myLamp.off();
    colorWipe(strip.Color(  0,   0,   0), 50);    // Black/off
         
 //   analogWrite(led_power_bottom, 255);
  } else if (vbat_per >= 70)
  {
    colorWipe(strip.Color(  0,   255,   0), BRIGHTNESS);
//    analogWrite(led_power_bottom, 240);
  //  myLamp.setColor(Color::Green);
    delay(500);
//    myLamp.off();
    colorWipe(strip.Color(  0,   0,   0), 50);    // Black/off
         
//    analogWrite(led_power_bottom, 255);
  }
}


void goMeasureBatt()
{

  // Get a raw ADC reading
  vbat_mv = readVBAT();

  // Convert from raw mv to percentage (based on LIPO chemistry)
  vbat_per = mvToPercent(vbat_mv);

  // Display the results

  Serial.print("LIPO = ");
  Serial.print(vbat_mv);
  Serial.print(" mV (");
  Serial.print(vbat_per);
  Serial.println("%)");

  //  blebas.write(vbat_per);
  //  blebas.notify(vbat_per);

  if (vbat_per < 10)
  {
    timerBLINKlowBatt.start();
  }
}


void batt_startup()
{
  pinMode(read_batt, INPUT);
  pinMode(enable_measure, OUTPUT);

  StartupMeasureBatt();
}



void goBlinkLowBatt()
{
  timerBlinkRunning = true;
//  analogWrite(led_power_bottom, 240);
//  myLamp.blink(Color::Red);
    colorWipe(strip.Color(  255,   0,   0), 50);    // Black/off
         

  if (vbat_per > 10)
  {
    colorWipe(strip.Color(  0,   0,   0), 50);    // Black/off
         
 //   myLamp.off();
//    analogWrite(led_power_bottom, 255);
    timerBLINKlowBatt.stop();
  }
}
