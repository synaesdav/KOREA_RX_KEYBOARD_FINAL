//Receiver modual to trigger video output using keyboard inputs
//able to delay keyboard strokes based on desired timing
//single menu only (performance mode)
//Korea tour version
//written by David Crittenden 7/2018
//updated after meeting with musicians
//BUTTON LAYOUT
/*
  row 1: All around you WITH COUNT IN / Always Away NO INTRO / Everything is Beautiful Under the Sun NO INTRO / Sing WITH COUNT IN
  row 2: Inferno NO INTRO / Beautiful NO INTRO / All around you NO INTRO / Sing NO INTO
  row 3: CRISTIANO DANCE MUSIC / HEART FRAME FADE / Circle Droplet / Multi - Color Shock Wave
  row 4: FUCK TRUMP / Rainbow Checkers / Red-White Heart / CLEAR ALL
*/

//code derived from https://learn.adafruit.com/remote-effects-trigger/overview-1?view=all
//see also https://learn.adafruit.com/adafruit-feather-32u4-radio-with-rfm69hcw-module/using-the-rfm69-radio

//Ada_remoteFXTrigger_RX_NeoPixel
//Remote Effects Trigger Box Receiver
//by John Park
//for Adafruit Industries
//MIT License
//Modified by David Crittenden 11/2017 - 6/2018

#include <Keyboard.h>//library allows device to act as a keyboard 
#include <SPI.h>
#include <RH_RF69.h>
#include <Wire.h>

#define LED 13

/************ Radio Setup ***************/
// Change to 434.0 or other frequency, must match RX's freq!
#define RF69_FREQ 915.0

//#if defined(ARDUINO_SAMD_FEATHER_M0) // Feather M0 w/Radio
#define RFM69_CS      8
#define RFM69_INT     3
#define RFM69_RST     4
//#endif

// Singleton instance of the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_INT);

bool oldState = HIGH;
int showType = 0;

////////////////////////////variables used in different functions
//radio variables
uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
uint8_t len = sizeof(buf);

//used for mode cycling
uint8_t mode = 0;
uint8_t currentMode;
uint8_t previousMode = 0;

int bpm;
unsigned long oneBeat;//calculates the number of milliseconds of one beat
unsigned long oneBar;//calculates the length of one bar in milliseconds
unsigned long currentMillis;
unsigned long startMillis;
unsigned long reStartMillis;
unsigned long previousRestartMillis;
unsigned long previousMillis;
unsigned long elapsedMillis;
unsigned long previousElapsedMillis;
bool ready = true;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup()
{
  delay(500);
  Serial.begin(115200);
  /*while (!Serial)
    {
    delay(1);  // wait until serial console is open, remove if not tethered to computer
    }*/

  pinMode(LED, OUTPUT);
  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);

  Serial.println("Feather RFM69 RX/TX Test!");
  // manual reset
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);
  if (!rf69.init())
  {
    Serial.println("RFM69 radio init failed");
    while (1);
  }
  Serial.println("RFM69 radio init OK!");
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for low power module)
  // No encryption
  if (!rf69.setFrequency(RF69_FREQ))
  {
    Serial.println("setFrequency failed");
  }

  // If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power with the
  // ishighpowermodule flag set like this:
  rf69.setTxPower(20, true);//set to 20 on transmitter, originally 14

  // The encryption key has to be the same as the one in the server
  uint8_t key[] =
  {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
  };
  rf69.setEncryptionKey(key);

  pinMode(LED, OUTPUT);//sets the onboard led
  Serial.print("RFM69 radio @");  Serial.print((int)RF69_FREQ);  Serial.println(" MHz");
}

void loop()
{
  buttonCheck();

  switch (mode)
  {
    case 1://ALL AROUND YOU - WITH 8 BEAT COUNT IN
      bpm = 120;
      oneBeat = (60000 / bpm);
      oneBar = oneBeat * 8;
      timeCheck();
      currentMode = mode;
      if (elapsedMillis < (oneBar) && ready == true)//COUNT IN
      {
        buttonCheck();
        if (currentMode != mode) break;
        Blink(LED, 50, 5); //blink LED 5 times, 50ms between blinks
        timeCheck();
      }
      if (elapsedMillis > (oneBar) && ready == true)//GO
      {
        buttonCheck();
        if (currentMode != mode) break;
        Keyboard.print(" ");//send message to computer
        Blink(LED, 50, 5); //blink LED 5 times, 50ms between blinks
        ready = false;
        timeCheck();
      }
      break;

    case 2://ALWAYS AWAY - NO COUNT IN
      if (ready == true)
      {
        Keyboard.print(" ");//send message to computer
        ready = false;
        Blink(LED, 50, 5); //blink LED 5 times, 50ms between blinks
      }
      break;

    case 3://EVERYTHING IS BEAUTIFUL - NO COUNT IN
      if (ready == true)
      {
        Keyboard.print(" ");//send message to computer
        ready = false;
        Blink(LED, 50, 5); //blink LED 5 times, 50ms between blinks
      }
      break;

    case 4://SING - WITH 8 BEAT COUNT IN
      bpm = 100;
      oneBeat = (60000 / bpm);
      oneBar = oneBeat * 8;
      timeCheck();
      currentMode = mode;
      if (elapsedMillis < (oneBar) && ready == true)//COUNT IN
      {
        buttonCheck();
        if (currentMode != mode) break;
        Blink(LED, 50, 5); //blink LED 5 times, 50ms between blinks
        timeCheck();
      }
      if (elapsedMillis > (oneBar) && ready == true)//GO
      {
        buttonCheck();
        if (currentMode != mode) break;
        Keyboard.print(" ");//send message to computer
        Blink(LED, 50, 5); //blink LED 5 times, 50ms between blinks
        ready = false;
        timeCheck();
      }
      break;

    case 5://INFERNO - NO COUNT IN
      if (ready == true)
      {
        Keyboard.print(" ");//send message to computer
        ready = false;
        Blink(LED, 50, 5); //blink LED 5 times, 50ms between blinks
      }
      break;

    case 6://BEAUTIFUL - NO INTRO
       if (ready == true)
      {
        Keyboard.print(" ");//send message to computer
        ready = false;
        Blink(LED, 50, 5); //blink LED 5 times, 50ms between blinks
      }
      break;

    case 7://ALL AROUND YOU - NO COUNT IN
      if (ready == true)
      {
        Keyboard.print(" ");//send message to computer
        ready = false;
        Blink(LED, 50, 5); //blink LED 5 times, 50ms between blinks
      }
      break;

    case 8://SING - NO COUNT IN
      if (ready == true)
      {
        Keyboard.print(" ");//send message to computer
        ready = false;
        Blink(LED, 50, 5); //blink LED 5 times, 50ms between blinks
      }
      break;

    case 16://Clear All
      if (ready == true)
      {
        Keyboard.print(" ");//send message to computer
        ready = false;
        Blink(LED, 50, 5); //blink LED 5 times, 50ms between blinks
      }
      break;
    default://unassigned input received
      if (ready == true)
      {
        ready = false;
        Blink(LED, 50, 5); //blink LED 5 times, 50ms between blinks
      }
      break;

  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void Blink(byte PIN, byte DELAY_MS, byte loops)//taken from RadioHead69_RawDemo_RX example code
{
  for (byte i = 0; i < loops; i++)
  {
    digitalWrite(PIN, LOW);
    delay(DELAY_MS);
    digitalWrite(PIN, HIGH);
    delay(DELAY_MS);
  }
}

void timeCheck()
{
  currentMillis = millis();//check the time
  elapsedMillis = currentMillis - reStartMillis;//check if enough time has passed
}

void buttonCheck()
{
  if (rf69.waitAvailableTimeout(2))//originally 100
  {
    // Should be a message for us now
    buf[RH_RF69_MAX_MESSAGE_LEN];
    len = sizeof(buf);

    if (! rf69.recv(buf, &len))
    {
      Serial.println("Receive failed");
      return;
    }

    rf69.printBuffer("Received: ", buf, len);
    buf[len] = 0;

    //previousValues();
    mode = buf[0];
    ready = true;
    currentMillis = millis();//check the time
    reStartMillis = currentMillis;
    previousMillis = currentMillis;
    elapsedMillis = 0;
  }
}
