#include <SPI.h>


bool powerDown = false;
byte set_ctrl_dpi = 2;  //0 - 250 dpi, 1 - 500, 2 - 1000, 3 - 1250, 4 - 1500, 5 - 1750, 6 - 2000 dpi
byte mouse_ctrl_bit = 0b100000;

bool enable_resolution_settings = false;
byte set_resolution_dpi = 3;  //0 - 250 dpi, 1 - 500, 2 - 750, 3 - 1000, 4 - 1250, 5 - 1500, 6 - 1750, 7 - 2000, 8 - 4000 dpi
byte resolution_bit = 0b00000000;

// SPI and misc pins for the ADNS
#define PIN_SCLK   SCK
#define PIN_MISO   MISO
#define PIN_MOSI   MOSI
#define PIN_NCS    9
#define PIN_MOTION 47


// Define all the registers
#define PROD_ID                          0x00
#define REV_ID                           0x01
#define MOTION_ST                        0x02
#define DELTA_X                          0x03
#define DELTA_Y                          0x04
#define SQUAL                            0x05
#define SHUT_HI                          0x06
#define SHUT_LO                          0x07
#define PIX_MAX                          0x08
#define PIX_ACCUM                        0x09
#define PIX_MIN                          0x0a
#define PIX_GRAB                         0x0b
#define MOUSE_CTRL                       0x0d
#define RUN_DOWNSHIFT                    0x0e
#define REST1_PERIOD                     0x0f
#define REST1_DOWNSHIFT                  0x10
#define REST2_PERIOD                     0x11
#define REST2_DOWNSHIFT                  0x12
#define REST3_PERIOD                     0x13
#define PREFLASH_RUN                     0x14
#define PREFLASH_RUN_DARK                0x18
#define MOTION_EXT                       0x1b
#define SHUT_THR                         0x1c
#define SQUAL_THRESHOLD                  0x1d
#define NAV_CTRL2                        0x22
#define MISC_SETTINGS                    0x25
#define RESOLUTION                       0x33
#define LED_PRECHARGE                    0x34
#define FRAME_IDLE                       0x35
#define RESET                            0x3a
#define INV_REV_ID                       0x3f
#define LED_CTRL                         0x40
#define MOTION_CTRL                      0x41
#define AUTO_LED_CONTROL                 0x43
#define REST_MODE_CONFIG                 0x45


void com_start() {
  digitalWrite(PIN_NCS, HIGH);
  delay(20);
  digitalWrite(PIN_NCS, LOW);
  delay(100);
  digitalWrite(PIN_NCS, HIGH);
  delay(20);
  digitalWrite(PIN_NCS, LOW);
}



byte Read(byte reg_addr) {
  digitalWrite(PIN_NCS, LOW);//begin communication
  // send address of the register, with MSBit = 0 to say it's reading
  SPI.transfer(reg_addr & 0x7f );
  delayMicroseconds(100);
  // read data
  byte data = SPI.transfer(0);
  delayMicroseconds(30);
  digitalWrite(PIN_NCS, HIGH);//end communication
  delayMicroseconds(30);

  return data;
}


void Write(byte reg_addr, byte data) {
  digitalWrite(PIN_NCS, LOW);
  //send address of the register, with MSBit = 1 to say it's writing
  SPI.transfer(reg_addr | 0x80 );
  //send data
  SPI.transfer(data);
  delayMicroseconds(30);
  digitalWrite(PIN_NCS, HIGH);//end communication
  delayMicroseconds(30);
}


void mouse_ctrl() {
  switch (set_ctrl_dpi) {
    case 0:
      bitWrite(mouse_ctrl_bit, 2, 1);
      bitWrite(mouse_ctrl_bit, 3, 0);
      bitWrite(mouse_ctrl_bit, 4, 0);
      break;
    case 1:
      bitWrite(mouse_ctrl_bit, 2, 0);
      bitWrite(mouse_ctrl_bit, 3, 1);
      bitWrite(mouse_ctrl_bit, 4, 0);
      break;
    case 2:
      bitWrite(mouse_ctrl_bit, 2, 0);
      bitWrite(mouse_ctrl_bit, 3, 0);
      bitWrite(mouse_ctrl_bit, 4, 0);
      break;
    case 3:
      bitWrite(mouse_ctrl_bit, 2, 1);
      bitWrite(mouse_ctrl_bit, 3, 1);
      bitWrite(mouse_ctrl_bit, 4, 0);
      break;
    case 4:
      bitWrite(mouse_ctrl_bit, 2, 0);
      bitWrite(mouse_ctrl_bit, 3, 0);
      bitWrite(mouse_ctrl_bit, 4, 1);
      break;
    case 5:
      bitWrite(mouse_ctrl_bit, 2, 1);
      bitWrite(mouse_ctrl_bit, 3, 0);
      bitWrite(mouse_ctrl_bit, 4, 1);
      break;
    case 6:
      bitWrite(mouse_ctrl_bit, 2, 0);
      bitWrite(mouse_ctrl_bit, 3, 1);
      bitWrite(mouse_ctrl_bit, 4, 1);
      break;
    default:
      bitWrite(mouse_ctrl_bit, 2, 0);
      bitWrite(mouse_ctrl_bit, 3, 0);
      bitWrite(mouse_ctrl_bit, 4, 0);
      break;
  }
  if (powerDown == true) {
    bitWrite(mouse_ctrl_bit, 1, 1);
  } else {
    bitWrite(mouse_ctrl_bit, 1, 0);
  }
  /*
    Serial.println(mouse_ctrl_bit, BIN);
    Serial.println(mouse_ctrl_bit, HEX);
  */
}

void adns_startup() {
  //--------Setup SPI Communication---------
  byte out = 0;
  byte read = 0;
  byte bit = 0;
  pinMode(PIN_MISO, INPUT);
  pinMode(PIN_NCS, OUTPUT);
  SPI.begin();
  // set the details of the communication
  SPI.setBitOrder(MSBFIRST); // transimission order of bits
  SPI.setDataMode(SPI_MODE3); // sampling on rising edge
  SPI.setClockDivider(SPI_CLOCK_DIV64); // 16MHz/16 = 1MHz
  delay(100);

  //----------------- Power Up and config ---------------
  com_start();
  
    Write(RESET, 0x5a);        // force reset
    delay(100);                // wait for it to reboot
//    Write(MOUSE_CTRL, 0x20);   //Setup Mouse Control
//    Write(MOTION_CTRL, 0x00);  //Clear Motion Control register
//    delay(100);
  


  mouse_ctrl();

  Write(MOUSE_CTRL, mouse_ctrl_bit);
    Write(MOTION_CTRL, 0x00);  //Clear Motion Control register

  //low power wireless mode
  Write(NAV_CTRL2, 0x00);
  Write(FRAME_IDLE, 0x26);
  Write(PREFLASH_RUN, 0x30);
  Write(PREFLASH_RUN_DARK, 0x30);
  Write(AUTO_LED_CONTROL, 0x01);
  Write(LED_CTRL, 0x01);
  

  Write(0x02, 0x00);
  delay(100);
}



int convTwosComp(int b) { //Convert from 2's complement
  if (b & 0x80) {
    b = -1 * ((b ^ 0xff) + 1);
  }
  return b;
}

void getXY() { //Prints out X and Y values to the serial terminal, use getX and getY sequentially for most operations
  byte x = 0;
  byte y = 0;
  x = Read(0x03);
  y = Read(0x04);
  
  x = convTwosComp(x);
  y = convTwosComp(y);

  y = map(y, 127, -127, -127, 127);
  
  adnsX = x;
  adnsY = y;
}


int getX() { //returns the X acceleration value
  int8_t x = 0;
  x = Read(0x03);
  return (convTwosComp(x));
}

int getY() { //returns the Y acceleration value
  int8_t y = 0;
  y = Read(0x04);
  return (convTwosComp(y));
}



void adns_sleep() {
  Write(MOUSE_CTRL, 0x02);
  SPI.end();
  pinMode(PIN_SCLK, INPUT);
  pinMode(PIN_MOSI, INPUT);
  pinMode(PIN_NCS, INPUT);
}



void adns_changeDPI()
{
  if (set_ctrl_dpi <= 6)
  {
    set_ctrl_dpi++;
  } else {
    set_ctrl_dpi = 0;
  }
  digitalWrite(power_pin, LOW);
  delay(100);
  digitalWrite(power_pin, HIGH);
  delay(100);
  adns_startup();
}
