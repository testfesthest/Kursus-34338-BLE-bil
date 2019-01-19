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

// Set global variables
char c=' ';
boolean NL = true;
byte carMode = 0;
 
void setup() 
{
    pinMode(7, INPUT);
    Serial.begin(9600);// opens serial port, sets data rate to 9600 bps
    Serial.print("Sketch:   ");   Serial.println(__FILE__);
    Serial.print("Uploaded: ");   Serial.println(__DATE__);
    Serial.println(" ");
 
    BTserial.begin(9600);  
    Serial.println("BTserial started at 9600");
    
    //BTserial.print("AT+IMME1" ); //Make sure the module is set to manual connect
    //delay(1000); // These are required
    //BTserial.print("AT+ROLE0" ); //Make sure it is set to SLAVE
    //delay(1000);
    //BTserial.print("AT+NAMEslaveHM" ); //Set the name (in case AT+RENEW has been called)
    //delay(1000)
    //BTserial.print("AT+TYPE0" ); //Make sure it doesnt require password
    //delay(1000);
    
}
 
void loop()
{
    //readFromModuleAndDisplayInMonitor();
    readFromMonitorAndSendToModule();
     // read data only when you receive data
    if (BTserial.available())
    {
        // read the incoming byte
        if (BTserial.read() == integer){
          if (BTserial.read()
          carMode = BTserial.read();
        }
    }
    //switch (receivedFromJoystickModule){
    //case 1
      //forward full on
      //analogWrite(enA, 250); // Send PWM signal to L298N Enable pin
      //analogWrite(enB, 250); // Send PWM signal to L298N Enable pin
      //break;
    //case 2
      // forward slowly
      //analogWrite(enA, 150); // Send PWM signal to L298N Enable pin
      //analogWrite(enB, 150); // Send PWM signal to L298N Enable pin
      //break;
      //...
    //}
}
// Read from the Bluetooth module and send to the Arduino Serial Monitor
void readFromModuleAndDisplayInMonitor()
{
    // send data only when you receive data
    if (BTserial.available())
    {
        // read the incoming byte
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
        // read the incoming byte:
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
