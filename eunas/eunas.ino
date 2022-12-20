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

const bool testMode = false;

//4 digit display
#include <TM1637Display.h>
//four digit display thing stuff
// Module connection pins (Digital Pins)
#define CLK 5
#define DIO 6
TM1637Display display(CLK, DIO);

//Wifi
// #include <SPI.h>
#include <WiFi101.h>
#include "arduino_secrets.h"
int status = WL_IDLE_STATUS;
WiFiClient client;
char ssid[] = SECRET_SSID; //  your network SSID (name)
char pass[] = SECRET_PASS;//  your network PASSWORD ()

//weather stuff
char server[] = "api.openweathermap.org";
//open weather map api key
String apiKey = SECRET_APIKEY;
String lat = "40.927227";
String lon = "-73.966860";

//Neo Pixel LED stuff
#include <Adafruit_NeoPixel.h>

// neo pixel data pin for buttons
#define LED_PIN   15 // button strip
// How many NeoPixels are attached buttons?  Button NeoPixel Strip
#define LED_COUNT 3  // button strip
// Declare our NeoPixel strip object:
Adafruit_NeoPixel buttStrip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// neo pixel data pin for notification strip
#define LED_PIN_TWO   16 // button strip
// How many NeoPixels are on the top?  Button NeoPixel Strip
#define LED_COUNT_TWO 5  // button strip
// Declare our NeoPixel strip object:
Adafruit_NeoPixel notiStrip(LED_COUNT_TWO, LED_PIN_TWO, NEO_GRB + NEO_KHZ800);

// button states and stuff
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
bool timerStateOne = false;  //do we count?
bool timerStateTwo = false;  
bool timerAlarmOne = false;   //did we forget?
bool timerAlarmTwo = false;
// int generalCounter = 0;
unsigned long timExpireyOne;
unsigned long timExpireyTwo;
unsigned long rightMeow;

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
    timerOne = 11000; //length of timer one
    timerTwo = 14000; //length of timer2
  }

  buttStrip.begin();            // INITIALIZE NeoPixel strip object (REQUIRED)
  buttStrip.show();             // Turn OFF all pixels ASAP
  buttStrip.setBrightness(255); // Set BRIGHTNESS to about 1/5 (max = 255)

  notiStrip.begin();            // INITIALIZE NeoPixel strip object (REQUIRED)
  notiStrip.show();             // Turn OFF all pixels ASAP
  notiStrip.setBrightness(255); // Set BRIGHTNESS to about 1/5 (max = 255)

  //four digit display thing stuff
  // uint8_t data[] = { 0xff, 0xff, 0xff, 0xff };
  uint8_t blank[] = { 0x00, 0x00, 0x00, 0x00 };
  // display.setBrightness(0x00);
  display.setBrightness(0xFF);
 
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
    Serial.println("No Internet! WTF...I'm going on without it.");
    notiStrip.setPixelColor(4,notiStrip.Color(5,0,0));
  }
  notiStrip.show();

  //when will then be now?
  rightMeow = millis();
}

void loop() {

  //when will then be now?
  rightMeow = millis();

  checkButtons();

  checkAlarms();

  updateDisplay();

  updateLights();

}

void checkButtons () {

  //read the buttons
  buttonStateOne = digitalRead(buttonPinOne);
  buttonStateTwo = digitalRead(buttonPinTwo);
  buttonStateThree = digitalRead(buttonPinThree);

  // Start the coffee timer
  if (buttonStateOne == HIGH && timerAlarmOne == false) {
    // Start timer one
    timerStateOne = true;
    timExpireyOne = rightMeow + timerOne;
    // buttStrip.setPixelColor(0,buttStrip.Color(0, 25, 0));
  } else if (buttonStateOne == HIGH && timerAlarmOne == true) {
    //cancel timer one
    // buttStrip.setPixelColor(0,buttStrip.Color(100,100,0));
    // buttStrip.show();
    timerAlarmOne = false;
    delay(500);
  }
  // Start the laundry timer
  if (buttonStateTwo == HIGH && timerAlarmTwo == false) {
    timerStateTwo = true;
    timExpireyTwo = rightMeow + timerTwo;
    // buttStrip.setPixelColor(1,buttStrip.Color(0, 0, 25));
  } else if (buttonStateTwo == HIGH && timerAlarmTwo == true) {
    // you can reset individual alarms if they are done and multiple timers are going off.
    // buttStrip.setPixelColor(1,buttStrip.Color(100,100,0));
    // buttStrip.show();
    timerAlarmTwo = false;
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
    timerStateOne = false;
    timerStateTwo = false;
    timerAlarmOne = false;
    timerAlarmTwo = false;
    //Timer is running or was in alarm mode: turn off button LED
    buttStrip.setPixelColor(0,buttStrip.Color(0,0,0));
    buttStrip.setPixelColor(1,buttStrip.Color(0,0,0));
    buttStrip.setPixelColor(2,buttStrip.Color(100,100,0));
    // Update the 4 digit display
    uint8_t blank[] = { 0x00, 0x00, 0x00, 0x00 };
    // display.setBrightness(0x00);
    display.setSegments(blank);
    buttStrip.show();
  } else {
    buttStrip.setPixelColor(2,buttStrip.Color(0,0,0));
    buttStrip.show();
  }
}

void checkAlarms () {
  // int newCount = 0; //the number we should be displaying:
  // We just passed the timer threshold.
  if (timerStateOne == true && timExpireyOne < rightMeow) { 
    timerAlarmOne = true;
    timerStateOne = false;
  }
  // if the alarm was previously just on we're going into alarm mode
  if (timerStateTwo == true && timExpireyTwo < rightMeow) { 
    timerAlarmTwo = true;
    timerStateTwo = false;
  }
}

void updateDisplay() {
  int newCountOne = (timExpireyOne - rightMeow)/1000; // (seconds) the number we should be displaying:
  int newCountTwo = (timExpireyTwo - rightMeow)/1000; // (seconds) the number we should be displaying:
  //make a piecemeal display
  uint8_t blankTwo[] = { 0x00, 0x00, 0x00, 0x00 };  //for display

  if (timerAlarmOne || timerAlarmTwo) {
    display.setBrightness(0xff);
  } else {
    display.setBrightness(0x00);
  }

  if (timerAlarmOne && timerAlarmTwo) {  //Technically this isn't necessary...but the display doesn't work without it
    display.showNumberDec(0,true);
  } else if ((timerAlarmOne && ! timerStateTwo) || (timerAlarmTwo && ! timerStateOne)) {
    display.showNumberDecEx(0, 0b01000000, true);
  }

  //we have a disp hog:
  if ((timerStateOne || timerAlarmOne) != (timerStateTwo || timerAlarmTwo)){
    // we got a hog!
    //make way for a display hog
    if (! timerStateTwo && ! timerAlarmTwo) { //check if timer two is doing something
      if (timerStateOne) {  //that means timer state one can hog the display.
        //hog the display:
        int hCountOne = (newCountOne / 60)*100 + (newCountOne % 60);
        display.showNumberDecEx(hCountOne, 0b01000000, true);
      }
    } else if (! timerStateOne && ! timerAlarmOne) { //check if timer one is doing something
      if (timerStateTwo) {  //that means timer state two can hog the display.
        //hog the display:
        int hCountTwo = (newCountTwo / 60)*100 + (newCountTwo % 60);
        display.showNumberDecEx(hCountTwo, 0b01000000, true);
        delay(10);
      }
    } 
  } else {
    // populate it individually:
    if (timerStateOne) {
      // if we have over a minute left report the minutes...else seconds
      if (newCountOne < 60) { 
        // report last two digits
        blankTwo[0] = display.encodeDigit(newCountOne / 10 % 10);
        blankTwo[1] = display.encodeDigit(newCountOne % 10);
        Serial.println("'bingo'");
      } else {
        //report tens of minutes and minutes
        blankTwo[0] = display.encodeDigit(newCountOne / 600 % 10);
        blankTwo[1] = display.encodeDigit(newCountOne / 60 % 10);
        Serial.println("'bongo'");
      }
    } else {
      blankTwo[0] = 0x00;
      blankTwo[1] = 0x00;
    }
    if (timerAlarmOne) { //Timer is in alarm mode 
      //update the display
      display.setBrightness(0xff);
      blankTwo[0] = display.encodeDigit(0);
      blankTwo[1] = display.encodeDigit(0);
    }
    if (timerStateTwo) {
      if (newCountTwo < 60) {
        blankTwo[2] = display.encodeDigit(newCountTwo / 10 % 10);
        blankTwo[3] = display.encodeDigit(newCountTwo % 10);
      } else {
        blankTwo[2] = display.encodeDigit(newCountTwo / 600 % 10);
        blankTwo[3] = display.encodeDigit(newCountTwo / 60 % 10);
      }
    } else {
      blankTwo[2] = 0x00;
      blankTwo[3] = 0x00;
    }
    if (timerAlarmTwo) { // Timer 2 is alarm state
      //update the display
      display.setBrightness(0xff);
      blankTwo[2] = display.encodeDigit(0);
      blankTwo[3] = display.encodeDigit(0);
    }
    display.setSegments(blankTwo);
  }
}

void updateLights() {
  // turn off button led
  if ( ! timerStateOne && ! timerAlarmOne ) { buttStrip.setPixelColor(0,buttStrip.Color(0,0,0)); }
  // In progress status light
  if (timerStateOne) {buttStrip.setPixelColor(0,buttStrip.Color(0,25,0));} // turn on first button
  // Turn off button LED
  if (timerAlarmOne) {buttStrip.setPixelColor(0,buttStrip.Color(255,0,0));} // turn on first button
  // Turn off button LED
  if ( ! timerStateTwo && ! timerAlarmTwo ) { buttStrip.setPixelColor(1,buttStrip.Color(0,0,0)); }
  // Second timer progress light
  if (timerStateTwo) {buttStrip.setPixelColor(1,buttStrip.Color(0,0,25));} // turn on Second button
  //edge case where I get two cups of coffee during a laundry cycle
  if (timerAlarmTwo) {buttStrip.setPixelColor(1,buttStrip.Color(255,0,0));} // turn on second button
  //this resets brightness and alarm status for the top leds
  if ( ! timerAlarmOne && ! timerAlarmTwo) {
    // reset brightness to low
    // display.setBrightness(0);
    // Turn off the top lights
    notiStrip.setPixelColor(0,(0,0,0));
    notiStrip.setPixelColor(1,(0,0,0));
    notiStrip.setPixelColor(2,(0,0,0));
    notiStrip.setPixelColor(3,(0,0,0));
    notiStrip.setPixelColor(4,(0,0,0));
  }
  if (timerAlarmOne == true || timerAlarmTwo == true) {
    //light the top lights:
    notiStrip.setPixelColor(0,buttStrip.Color(255,0,0));
    notiStrip.setPixelColor(1,buttStrip.Color(255,0,0));
    notiStrip.setPixelColor(2,buttStrip.Color(255,0,0));
    notiStrip.setPixelColor(3,buttStrip.Color(255,0,0));
    // notiStrip.setPixelColor(4,buttStrip.Color(255,0,0));
    // display.setBrightness(0xFF);
  }
  //check wifi
  if (status == WL_CONNECTED) {
    // Serial.println("crazy it worked!");
    notiStrip.setPixelColor(4,notiStrip.Color(0,5,0));
  } else {
    // Serial.println("NO Internet! WTF...I'm going on without it.");
    notiStrip.setPixelColor(4,notiStrip.Color(5,0,0));
  }
  buttStrip.show(); 
  notiStrip.show();
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this loop:
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    // buttStrip.rainbow() can take a single argument (first pixel hue) or
    // optionally a few extras: number of rainbow repetitions (default 1),
    // saturation and value (brightness) (both 0-255, similar to the
    // ColorHSV() function, default 255), and a true/false flag for whether
    // to apply gamma correction to provide 'truer' colors (default true).
    buttStrip.rainbow(firstPixelHue);
    notiStrip.rainbow(firstPixelHue);
    // Above line is equivalent to:
    // buttStrip.rainbow(firstPixelHue, 1, 255, 255, true);
    buttStrip.show(); // Update strip with new contents
    notiStrip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
  buttStrip.clear();
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
