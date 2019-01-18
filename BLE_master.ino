//  SerialIn_SerialOut_HM-10_01
//
//  Uses hardware serial to talk to the host computer and AltSoftSerial for communication with the bluetooth module
//
//  What ever is entered in the serial monitor is sent to the connected device
//  Anything received from the connected device is copied to the serial monitor
//  Does not send line endings to the HM-10
//
//  Pins
//  BT VCC to Arduino 5V out. 
//  BT GND to GND
//  Arduino D8 (SS RX) - BT TX no need voltage divider 
//  Arduino D9 (SS TX) - BT RX through a voltage divider (5v to 3.3v)
//
 
#include <AltSoftSerial.h>
AltSoftSerial BTserial; 
// https://www.pjrc.com/teensy/td_libs_AltSoftSerial.html

char c=' ';
boolean NL = true;
int state = 0; //bruges ikke længere tror jeg
const int SW_pin = 2; // digital pin connected to switch output
const int X_pin = 0; // analog pin connected to X output
const int Y_pin = 1; // analog pin connected to Y output

void setup() 
{
    pinMode(7, INPUT);
    Serial.begin(9600);
    Serial.print("Sketch:   ");   Serial.println(__FILE__);
    Serial.print("Uploaded: ");   Serial.println(__DATE__);
    Serial.println(" ");
    
    pinMode(SW_pin, INPUT);//joystick push button
    digitalWrite(SW_pin, HIGH); //hvorfor?
 
    BTserial.begin(9600);  
    Serial.println("BTserial started at 9600");
    
    //connectToSlave();
    //Serial.println("This module is connected to its slave"); //the module will still be single letters at each iteration after this to complete the operation
}
 
void loop()
{
    //For debugging purposes
    readFromModuleAndDisplayInMonitor();
    readFromMonitorAndSendToModule();
    //get the joystick input
    //if (connection is on (use the pin on the board) then run below code. 
    //For now dont as it interferes with the connectToSlave() process by writing to the serial monitor
    int x = map(analogRead(X_pin),0,1023,250,-250);
    int y = map(analogRead(Y_pin),0,1023,250,-250);
    //int pushButton = digitalRead(SW_pin);
    //Serial.print(String(x)+"  "+String(y)+"\n");
    //Serial.print(x);

    //OBS.: if there are problems with connecting etc, then use the reset code for each in the sticky
    // send joystick input to slave module
      // no way to send two numbers (x and y) at once (but confirm with teacher) since the BLE module can only
      // send one asci code each iteration. What you must do instead is send ONE characer/number (doesn't matter
      // but can be represented by asci code (right, confirm understanding with teacher)) and make switch cases here.
      // the more switch cases, the finer the steering can be. Maybe there is a way to send two at once, but forget about it for now.
   //case(   
   // now the slave's version of this code needs to be combined with the motor code in "Car code" folder
      // there should be an amount of switc cases there equal to what is present here. Already somewhat made and
      // commented out
   // Create code to use with the infrared remote
   // færddigør github inkorporering
   // læs slides igennem for krav til rapporten
   // skim siderne i bogen
   // lav rapporten
   // lav billeder med fritzil
}


void readFromModuleAndDisplayInMonitor()
{
    // Read from the Bluetooth module and send to the Arduino Serial Monitor
    if (BTserial.available())
    {
        c = BTserial.read();
        Serial.write(c); //println cannot be used, as c only contains 1 character at a time
    }
}

// Get what the user wrote int the serial monitor and send it to the BLE module
// only really used for testing and setting configurations on it
void readFromMonitorAndSendToModule()
{
  // Read from the Serial Monitor and send to the Bluetooth module
    if (Serial.available())
    {
        c = Serial.read();
        // do not send line end characters to the HM-10
        if (c!=10 & c!=13 ) 
        {  
             BTserial.write(c);
        }
        // Echo the user input to the main window. 
        // If there is a new line print the ">" character.
        if (NL) { Serial.print("\r\n>");  NL = false; }
        Serial.write(c);
        if (c==10) { NL = true; }
    }
}

void connectToSlave()
{
  BTserial.print("AT+IMME1" ); //Make sure the module is set to manual connect
  delay(1000); // These are required
  BTserial.print("AT+ROLE1" ); //Make sure it is set to master
  delay(1000);
  BTserial.print("AT+CON508CB16603EB" ); //Connect to the slave module using its address
  delay(1000);
}