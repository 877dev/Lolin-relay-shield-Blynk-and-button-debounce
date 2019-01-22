// v5 added Blynk support - final working version
 

#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = ""; //Enter the Auth code which was send by Blink

char ssid[] = "";  //Enter your WIFI Name
char pass[] = "";  //Enter your WIFI Password

const byte BUTTON = D5;
const byte RELAY = D6;
bool LastVirtualPinState = 0;                  // "0", "FALSE", "LOW' means exactly the same
int buttonState = LOW;  // the current state of BUTTON
int thisbuttonState = LOW; // the current and previous readings from the input pin
int lastbuttonState = LOW;// the current and previous readings from the input pin

// time is measured in milliseconds and will quickly exceed limitations of an integer, so we use a long for these two
unsigned long lastDebounceTime = 0;  // the time the button state last switched
unsigned long debounceDelay = 50;    // the state must remain the same for this many millis to register the button press

void setup() {
  
  Serial.begin(115200);            // See the connection status in Serial Monitor
  Blynk.begin(auth, ssid, pass);   //connects to WiFi network, then connects to Blynk server
    
  pinMode(BUTTON, INPUT);
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, LOW);   //sets relay to OFF
}


void loop() {

   Blynk.run();  // This function should be called frequently to process incoming commands and perform housekeeping of Blynk connection.
  
  // the BUTTON is read multiple times and the value must remain the same for debounceDelay millis to toggle the relay

  // read BUTTON state, HIGH when pressed, LOW when not
  thisbuttonState = digitalRead(BUTTON);

  // if the current state does not match the previous state
  // the button was just pressed/released, or is transition noise
  if (thisbuttonState != lastbuttonState) {
    // reset the timer
    lastDebounceTime = millis();
  }

  // once delay millis have elapsed, if the state remains the same, register the press
  if ((millis() - lastDebounceTime) > debounceDelay) {

    // if the button state has changed
    if (thisbuttonState != buttonState) {
      buttonState = thisbuttonState;

      // only toggle the relay if the buttonState has switched from LOW to HIGH
      if (buttonState == HIGH) {
             
        flipRelayOutput();   //calls flipRelayOutput function if above condition is met
      }
    }
  }

  // persist for next loop iteration
  lastbuttonState = thisbuttonState;

}


void flipRelayOutput()  // a function to flip the relay output, is called from the physical button.
{
  digitalWrite(RELAY, !digitalRead(RELAY));    //writes the inverse value to the relay pin  (booleon NOT operator )
  Blynk.virtualWrite(V6, digitalRead(RELAY)*255);   //write 255 or 0 to pin V6 for iOS Blynk app widget led (0-255 brightness)
  Serial.println( millis() ); // added just for interest
}



// BLYNK_WRITE is a function called every time device gets an update of a Virtual Pin value from the server (or app):
// Contains virtual button "latching" code
BLYNK_WRITE(V5)
{
    int pinValue = param.asInt(); // assigning incoming value from pin V5 to a variable

    if ((pinValue) && (!LastVirtualPinState)) // "pinValue" is your virtual pin current state ||||||  this means same as "if ((pinValue == 1) && (LastVirtualPinState == 0))"
    //if V5 virtual button is still being pressed, the LastVirtualState is set to 1, and !LastVirtualState will therefore be 0. Hence 1 && 0 condition == 0 and relay flipRelayOutput will not be called.
  
  {
     flipRelayOutput();   //calls function to flip relay output and flip virtual led state
  }
  
  LastVirtualPinState = pinValue;  // sets LastVirtualPinState to the same as pinValue, so if pinValue (V5 button) is high, LastVirtualPinState gets set to high
}


 
