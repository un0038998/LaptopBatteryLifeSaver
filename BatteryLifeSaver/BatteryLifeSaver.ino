#include <SoftwareSerial.h>

#define BT_RECEIVE_TIMEOUT 60000
#define BAUD 9600
#define relay 13
#define MIN_CUT_OFF_BATTERY_PCT 30
#define MAX_CUT_OFF_BATTERY_PCT 90

String receivedString;
SoftwareSerial BTserial(2, 3); // RX | TX


bool started  = false;
long lastreceivedTimeInMili = 0;
bool relayState = LOW;

void setup() 
{
  BTserial.begin(BAUD);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, relayState);
}

void loop() 
{
  receivedString = "";
  if (BTserial.available() > 0)
  {
    started = true;
    receivedString = BTserial.readStringUntil('\n');
    lastreceivedTimeInMili = millis();
  }

  //Intial condition: Keep battery charger on if bluetooth communication is not started yet since begining
  if (!started)
  {
    digitalWrite(relay, LOW); 
    return;
  }

  //If bluetooth data times out after 1 minute then swicth off charger currently. 
  //This is to handle case when laptop goes in sleep mode and we dont receive any data.
  if ((millis() - lastreceivedTimeInMili) > BT_RECEIVE_TIMEOUT)
  {
    digitalWrite(relay, HIGH); 
    return;
  }
  
  if (receivedString != "")
  {
    int batteryPercentage = receivedString.toInt();
    if (batteryPercentage >= MAX_CUT_OFF_BATTERY_PCT)
    {
      relayState = HIGH;
    }
    else if (batteryPercentage < MIN_CUT_OFF_BATTERY_PCT) 
    {
      relayState = LOW;      
    }
    digitalWrite(relay, relayState); 
  }
}
