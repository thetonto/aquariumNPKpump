#include <Arduino.h>
#include <Homie.h>
#include <Ticker.h>

//=======================================================================
//                               Setup
//  MQTT Channels
//  homie/aquarium/NPK_1/on/set
//  homie/aquarium/NPK_1/on
//  Compiled to Homie V3
//=======================================================================

//Setup the Homie Firmware
#define FW_NAME "aquariumNPKpumps"
#define FW_VERSION "1.3.0"

const int PIN_NPK1 = D1;
const int PIN_NPK2 = D2;
const int PIN_RELAY = 5;
// #define LED D4  //On board LED - GPIO2
#define LED LED_BUILTIN //On board LED - GPIO16


HomieNode npk1Node("NPK_1", "pump", "switch");
HomieNode npk2Node("NPK_2", "pump", "switch");

Ticker npk1Timer;
Ticker npk2Timer;

//=======================================================================
//  Function to turn off pump NPK_1 when interupt timer triggers
//=======================================================================
void turnOffnpk1()
{
  digitalWrite(LED, HIGH);  //Turn off LED
  digitalWrite(PIN_NPK1, LOW);  //Turn off pump
  npk1Node.setProperty("on").send("false");
  Homie.getLogger() << "npk one pump is off" << endl; 
  npk1Timer.detach();
}


//=======================================================================
//  Function to turn off pump NPK_2 when interupt timer triggers
//=======================================================================
void turnOffnpk2()
{
  digitalWrite(LED, HIGH);  //Turn off LED
  digitalWrite(PIN_NPK2, LOW);  //Turn off pump
  npk2Node.setProperty("on").send("false");
  Homie.getLogger() << "npk two pump is off" << endl; 
  npk2Timer.detach();
}


//=======================================================================
//   Function to check if a string is valid numbers
//=======================================================================
boolean isValidNumber(String str){
   for(byte i=0;i<str.length();i++)
   {
      if(isDigit(str.charAt(i))) return true;
        }
   return false;
} 

//=======================================================================
//   Handler for NPK 1 Pump
//=======================================================================
bool npk1Handler(const HomieRange& range, const String& value) {
    //Check we received a value number
  if (isValidNumber(value)) {
    Homie.getLogger() << "npk one will pump for " << value << " seconds" << endl;
  }
  else
  {
    Homie.getLogger() << "npk one invalid pump intruction received " << value << endl;
    return false;
  }

  int inTime = atol(value.c_str());

  // force the pump off if 0 is received
  if (inTime == 0){
    digitalWrite(PIN_NPK1, LOW);
    Homie.getLogger() << "npk one pump forced off" << endl;
    return true;
  }
  // set on if the value recived = true
  bool on = (value == "true");

  // condition ? first_expression : second_expression; 
  //digitalWrite(PIN_RELAY, on ? HIGH : LOW);
  digitalWrite(LED, LOW);  //Turn on LED while pumping
  // attached a timer for inTime seconds to turn the pump off
  npk1Timer.attach(inTime, turnOffnpk1); 
  npk1Node.setProperty("on").send(value);
  Homie.getLogger() << "npk one pump is " << (on ? "on" : "off") << endl;
  return true;
}

bool npk2Handler(const HomieRange& range, const String& value) {
  
  //Check we received a value number
  if (isValidNumber(value)) {
    Homie.getLogger() << "npk two will pump for " << value << " seconds" << endl;
  }
  else
  {
    Homie.getLogger() << "npk two invalid pump intruction received " << value << endl;
    return false;
  }

  int inTime = atol(value.c_str());

  // force the pump off if 0 is received
  if (inTime == 0){
    digitalWrite(PIN_NPK2, LOW);
    Homie.getLogger() << "npk two pump forced off" << endl;
    return true;
  }

  // set on if the value recived greater than 0
  bool on = (inTime != 0);

  // condition ? first_expression : second_expression; 
  digitalWrite(PIN_NPK2, on ? HIGH : LOW);
  digitalWrite(LED, LOW);  //Turn on LED while pumping
  // attached a timer for 5 seconds to turn the pump off
  npk2Timer.attach(inTime, turnOffnpk2); 
  npk2Node.setProperty("on").send("true");
  Homie.getLogger() << "npk two pump is " << (on ? "on" : "off") << endl;
  return true;
}




void setup() {
  Serial.begin(115200);
  Serial << endl << endl;
  pinMode(PIN_RELAY, OUTPUT);
  digitalWrite(PIN_RELAY, LOW);

  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);

  Homie_setFirmware(FW_NAME, FW_VERSION); // The underscore is not a typo! See Magic bytes
  npk1Node.advertise("npk1pump").setName("NPK1Pump").setDatatype("string").settable(npk1Handler);
  npk2Node.advertise("npk2pump").setName("NPK2Pump").setDatatype("string").settable(npk2Handler);

  Homie.setup();
}

void loop() {
  Homie.loop();
}

