/*
  Enclosed Universal Notification and Alarm System EUNAS (yoo nis)

  ######## ##     ## ##    ##    ###     ######  
  ##       ##     ## ###   ##   ## ##   ##    ## 
  ##       ##     ## ####  ##  ##   ##  ##       
  ######   ##     ## ## ## ## ##     ##  ######  
  ##       ##     ## ##  #### #########       ## 
  ##       ##     ## ##   ### ##     ## ##    ## 
  ########  #######  ##    ## ##     ##  ######  

  i took a bunch of spare parts to make a timer for my coffee and laundy.
  I didn't want an audio alarm, like the rest of my timers...and I wanted 
  custom times for one button presses.
  buuuuut...If I'm making something I should make it usefuller.
  How about a weather alarm so I know when to cover my car?
  How about a notification for my favorite streamers?

  Done:
  Buttons
  timer
  4 digit display
  strip of 3 neo pixels for the buttons
  wifi
  github

  todo:
  weather-sorta  (Still missing XML parsing and logic.)
  Twitch
*/

//4 digit display
#include <TM1637Display.h>

//Wifi and Weather
#include <SPI.h>
#include <WiFi101.h>
#include "arduino_secrets.h"
String lat = "40.927227";
String lon = "-73.966860";
int status = WL_IDLE_STATUS;
char server[] = "api.openweathermap.org";
WiFiClient client;

char ssid[] = SECRET_SSID; //  your network SSID (name)
char pass[] = SECRET_PASS;//  your network PASSWORD ()

//open weather map api key
String apiKey = SECRET_APIKEY;

// Module connection pins (Digital Pins)
#define CLK 5
#define DIO 6

const bool testMode = false;

//Neo Pixel LED stuff
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// neo pixel data pin for buttons
#define LED_PIN   15 // button strip
// How many NeoPixels are attached buttons?  Button NeoPixel Strip
#define LED_COUNT 3  // button strip
// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// neo pixel data pin for notification strip
#define LED_PIN_TWO   16 // button strip
// How many NeoPixels are on the top?  Button NeoPixel Strip
#define LED_COUNT_TWO 5  // button strip
// Declare our NeoPixel strip object:
Adafruit_NeoPixel notiStrip(LED_COUNT_TWO, LED_PIN_TWO, NEO_GRB + NEO_KHZ800);

// button states
int buttonStateOne = 0;  // variable for reading the pushbutton status
int buttonStateTwo = 0;  // variable for reading the pushbutton status
int buttonStateThree = 0;  // variable for reading the pushbutton status
// constants won't change. They're used here to set pin numbers:
const int buttonPinOne = 9;  // the number of the first button
const int buttonPinTwo = 10;  // the number of the second button
const int buttonPinThree = 11;  // the number of the third button
const int ledPin = 13;    // the number of the SMD LED pin

// timer vars
long timerOne = 240000; //length of timer one
long timerTwo = 2700000; //length of timer2
//timer states
int timerStateOne = 0;  //do we count?
int timerStateTwo = 0;  
int timerAlarmOne = 0;   //did we forget?
int timerAlarmTwo = 0;
int generalCounter = 0;
unsigned long timExpireyOne = 0;
unsigned long timExpireyTwo = 0;

//four digit display thing stuff
// TM1637 tm(CLK,DIO);
TM1637Display display(CLK, DIO);
// uint8_t blank[] = { 0x00, 0x00, 0x00, 0x00 };

void setup() {

  //Configure pins for Adafruit ATWINC1500 Feather
  WiFi.setPins(8,7,4,2);

  //Initialize serial and wait for port to open:
  Serial.begin(9600);

  pinMode(ledPin, OUTPUT);  // initialize digital pin 13 as an output (from the blink tutorial)
  digitalWrite(ledPin, LOW);    // turn the LED off by making the voltage LOW

  // initialize the pushbutton pins as an inputs:
  pinMode(buttonPinOne, INPUT);
  pinMode(buttonPinTwo, INPUT);
  pinMode(buttonPinThree, INPUT);

  if (testMode) {
    timerOne = 3000; //length of timer one
    timerTwo = 5000; //length of timer2
  }

  strip.begin();            // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();             // Turn OFF all pixels ASAP
  strip.setBrightness(255); // Set BRIGHTNESS to about 1/5 (max = 255)

  notiStrip.begin();            // INITIALIZE NeoPixel strip object (REQUIRED)
  notiStrip.show();             // Turn OFF all pixels ASAP
  notiStrip.setBrightness(255); // Set BRIGHTNESS to about 1/5 (max = 255)

  //four digit display thing stuff
  // uint8_t data[] = { 0xff, 0xff, 0xff, 0xff };
  uint8_t blank[] = { 0x00, 0x00, 0x00, 0x00 };
  display.setBrightness(0x00);
 
  // All segments on
  display.setSegments(blank);

  //Start the wifi
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  status = WiFi.begin(ssid, pass);
  if (status == WL_CONNECTED) {
    Serial.println("crazy it worked!");
    notiStrip.setPixelColor(4,notiStrip.Color(0,5,0));
  } else {
    status = WiFi.begin(ssid,pass);
    Serial.println("NO Internet! WTF...I'm going on without it.");
    notiStrip.setPixelColor(4,notiStrip.Color(5,0,0));
  }
  notiStrip.show();
}

void loop() {


  //when will then be now?
  unsigned long rightMeow = millis();

  //checkbuttons
  buttonStateOne = digitalRead(buttonPinOne);
  buttonStateTwo = digitalRead(buttonPinTwo);
  buttonStateThree = digitalRead(buttonPinThree);
  // Start the coffee timer
  if (buttonStateOne == HIGH && timerAlarmOne == 0) {
    timerStateOne = 1;
    timExpireyOne = rightMeow + timerOne;
    strip.setPixelColor(0,strip.Color(0, 25, 0));
  } else if (buttonStateOne == HIGH && timerAlarmOne == 1) {
    strip.setPixelColor(0,strip.Color(100,100,0));
    strip.show();
    timerAlarmOne = 0;
    delay(500);
  }
  // Start the laundry timer
  if (buttonStateTwo == HIGH && timerAlarmTwo == 0) {
    timerStateTwo = 1;
    timExpireyTwo = rightMeow + timerTwo;
    strip.setPixelColor(1,strip.Color(0, 0, 25));
  } else if (buttonStateTwo == HIGH && timerAlarmTwo == 1) {
    // you can reset individual alarms if they are done and multiple timers are going off.
    strip.setPixelColor(1,strip.Color(100,100,0));
    strip.show();
    timerAlarmTwo = 0;
    delay(500);
  }
  //LED Test easter egg Run rainbows across all the led's
  if (buttonStateThree == HIGH && buttonStateOne == HIGH) {
    rainbow(8);
    // getWeather();
  }
  // Cancel or reset or something
  if (buttonStateThree == HIGH) {
    // Reset the states and alarms.
    timerStateOne = 0;
    timerStateTwo = 0;
    timerAlarmOne = 0;
    timerAlarmTwo = 0;
    //Timer is running or was in alarm mode: turn off button LED
    strip.setPixelColor(0,strip.Color(0,0,0));
    strip.setPixelColor(1,strip.Color(0,0,0));
    strip.setPixelColor(2,strip.Color(100,100,0));
    // if (timerAlarmOne == 0 && timerAlarmTwo == 0){
    //   strip.setBrightness(0xff);
    // }
    // Update the 4 digit display
    uint8_t blank[] = { 0x00, 0x00, 0x00, 0x00 };
    display.setBrightness(0x00);
    display.setSegments(blank);
  } else {
    strip.setPixelColor(2,strip.Color(0,0,0));
  }
  strip.show();
  notiStrip.show();
  updateCounters();

  // unsigned long currentTime = millis();
  // /* This is my event_1 */
  // if ( currentTime - time_1 >= timerOne) {
  //   Serial.print("LDR: ");
  //   unsigned long testNum = currentTime - time_1;
  //   Serial.println( testNum );
  //   Serial.print("HTime: ");
  //   // anal
  //   // Serial.println()
    
  //   /* Update the timing for the next event*/
  //   time_1 = currentTime;
  // }

  //   /* This is my event_2 */
  // if ( currentTime - time_2 >= timerTwo) {

  //   Serial.print("Temp: ");
  //   // Serial.println( analogRead(tempSensor) );

  //   /* Update the timing for the next event*/
  //   time_2 = currentTime;
  // }
}

void updateCounters() {
  int newCount = 0; //the number we should be displaying:
  // uint8_t data[] = { 0xff, 0xff, 0xff, 0xff };  //for display
  uint8_t blankTwo[] = { 0x00, 0x00, 0x00, 0x00 };  //for display
  //when will then be now?
  unsigned long rightMeow = millis();
  // We just passed the timer threshold.
  if (timerStateOne == 1 && timExpireyOne < rightMeow) { 
    timerAlarmOne = 1;
    timerStateOne = 0;
    //set the button to red
    strip.setPixelColor(0,strip.Color(255, 0, 0));
    strip.show();
    //update the display
    blankTwo[0] = display.encodeDigit(0);
    blankTwo[1] = display.encodeDigit(0);
    display.setBrightness(0xff);
    // display.setSegments(blank);
  // what if we're just still counting?
  } else if (timerStateOne == 1 && timExpireyOne > rightMeow) {
    if (testMode) {
      newCount = (timExpireyOne - rightMeow)/1000;
    } else {
      newCount = (timExpireyOne - rightMeow)/60000;
    }
    blankTwo[0] = display.encodeDigit(newCount / 10 % 10);
    blankTwo[1] = display.encodeDigit(newCount % 10);
    // display.setBrightness(0x00);
    // display.setSegments(blank);
  // } else if (timerAlarmOne == 1 && timerStateOne == 0) {
  //   // update the display
  //   blankTwo[0] = display.encodeDigit(2);
  //   blankTwo[1] = display.encodeDigit(2);
  //   // display.setBrightness(0xff);
  }
  // if the alarm was previously just on we're going into alarm mode
  if (timerStateTwo == 1 && timExpireyTwo < rightMeow) { 
    timerAlarmTwo = 1;
    timerStateTwo = 0;
    //set the button to red
    strip.setPixelColor(1,strip.Color(255, 0, 0));
    // strip.show();
    //update the display
    blankTwo[2] = display.encodeDigit(0);
    blankTwo[3] = display.encodeDigit(0);
    display.setBrightness(0xff);
    // display.setSegments(blank);
  // what if we're just still counting?
  } else if (timerStateTwo == 1 && timExpireyTwo > rightMeow) {
    if (testMode) {
      newCount = (timExpireyTwo - rightMeow)/1000;
    } else {
      newCount = (timExpireyTwo - rightMeow)/60000;
    }
    blankTwo[2] = display.encodeDigit(newCount / 10 % 10);
    blankTwo[3] = display.encodeDigit(newCount % 10);
    // display.setBrightness(0x00);
  // } else if (timerAlarmTwo == 1 && timerStateTwo ==0) {
  //   // update the display
  //   blankTwo[2] = display.encodeDigit(1);
  //   blankTwo[3] = display.encodeDigit(1);
  //   display.setBrightness(0xff);
  }
  //if in an alarm state, keep it there
  if (timerAlarmOne == 1) {
    blankTwo[0] = display.encodeDigit(0);
    blankTwo[1] = display.encodeDigit(0);
  }
  if (timerAlarmTwo == 1) {
    blankTwo[2] = display.encodeDigit(0);
    blankTwo[3] = display.encodeDigit(0);
  }
  // What if there is nothing to do but clean?
  // Check for lights to be turned off.
  if (timerStateOne == 0 && timerAlarmOne == 0) {
    blankTwo[0] = 0x00;
    blankTwo[1] = 0x00;
    strip.setPixelColor(0,strip.Color(0,0,0));
  }
  if (timerStateTwo == 0 && timerAlarmTwo == 0) {
    blankTwo[2] = 0x00;
    blankTwo[3] = 0x00;
    strip.setPixelColor(1,strip.Color(0,0,0));
  }
  //edge case where I get two cups of coffee during a laundry cycle
  //this resets brightness and alarm status for the top leds
  if (timerAlarmOne == 0 && timerAlarmTwo == 0) {
    display.setBrightness(0x00);
    notiStrip.setPixelColor(0,(0,0,0));
    notiStrip.setPixelColor(1,(0,0,0));
    notiStrip.setPixelColor(2,(0,0,0));
    notiStrip.setPixelColor(3,(0,0,0));
    notiStrip.setPixelColor(4,(0,0,0));
  }
  if (timerAlarmOne == 1 || timerAlarmTwo == 1) {
    //light the top lights:
    notiStrip.setPixelColor(0,strip.Color(255,0,0));
    notiStrip.setPixelColor(1,strip.Color(255,0,0));
    notiStrip.setPixelColor(2,strip.Color(255,0,0));
    notiStrip.setPixelColor(3,strip.Color(255,0,0));
    notiStrip.setPixelColor(4,strip.Color(255,0,0));
  }
  //check wifi
  if (status == WL_CONNECTED) {
    // Serial.println("crazy it worked!");
    notiStrip.setPixelColor(4,notiStrip.Color(0,5,0));
  } else {

    // Serial.println("NO Internet! WTF...I'm going on without it.");
    notiStrip.setPixelColor(4,notiStrip.Color(5,0,0));
  }
  display.setSegments(blankTwo);
  strip.show(); 
  notiStrip.show();
}

void countDisp (int num) {}

void timerCount (uint16_t ledSpot, uint32_t leColor, int leDelay) {
  // if
  strip.setPixelColor(ledSpot, leColor);
  strip.show();
}

void blinkTwice() {
  // displayNumber(2);
  digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(200);              // wait for a second
  digitalWrite(ledPin, LOW);    // turn the LED off by making the voltage LOW
  delay(200);              // wait for a second
  digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(200);              // wait for a second
  digitalWrite(ledPin, LOW);    // turn the LED off by making the voltage LOW
  delay(200);              // wait for a second
  // tm.clearDisplay();
}

void blinkOnce() {
  // displayNumber(1);
  digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);              // wait for a second
  digitalWrite(ledPin, LOW);    // turn the LED off by making the voltage LOW
  delay(500);              // wait for a second
  // tm.clearDisplay();
}

// void showTime() {
//   tm.clearDisplay();
//   displayNumber(31);
//   delay(300);
//   tm.display(2,13);
//   delay(500);
//   tm.clearDisplay();
// }

void lightOne() {
  strip.setPixelColor(0,strip.Color(255,   0,   0));
  strip.show();
}

void killOne() {
  strip.setPixelColor(0,strip.Color(0,   0,   0));
  strip.show();
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this loop:
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    // strip.rainbow() can take a single argument (first pixel hue) or
    // optionally a few extras: number of rainbow repetitions (default 1),
    // saturation and value (brightness) (both 0-255, similar to the
    // ColorHSV() function, default 255), and a true/false flag for whether
    // to apply gamma correction to provide 'truer' colors (default true).
    strip.rainbow(firstPixelHue);
    notiStrip.rainbow(firstPixelHue);
    // Above line is equivalent to:
    // strip.rainbow(firstPixelHue, 1, 255, 255, true);
    strip.show(); // Update strip with new contents
    notiStrip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
  strip.clear();
  notiStrip.clear();
}

void getWeather() {
  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    client.print("GET /data/2.5/forecast?");
    // client.print("q="+location);
    client.print("lat="+lat);
    client.print("&lon="+lon);
    client.print("&appid="+apiKey);
    client.print("&mode=xml");
    client.println("&cnt=2");
    // client.println("&units=standard");
    client.println("Host: api.openweathermap.org");
    client.println("Connection: close");
    client.println();
  } else {
    Serial.println("unable to connect");
  }

  delay(1000);
  String line = "";
  while (client.connected()) {
    line = client.readStringUntil('\n');
    Serial.println(line);
    line = client.readStringUntil('\n');
    Serial.println(line);
    line = client.readStringUntil('\n');
    Serial.println(line);
  }
  Serial.print("Goodbye!");
  Serial.println("...go away\n");
}
