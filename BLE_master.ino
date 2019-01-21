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

// Define global variables and constants
char c=' ';
boolean NL = true;
int state = 0; //bruges ikke længere tror jeg
const int SW_pin = 2; // digital pin connected to switch output
const int state_pin = 12; // digital pin connected to state output
const int break_pin = 13; // digital pin connected to break output
const int X_pin = 0; // analog pin connected to X output
const int Y_pin = 1; // analog pin connected to Y output
int carMode = 0; // each mode (number) corresponds to a certain speed and direction for the car.
int SW_val = 1; // 0 means pressed down. 1 means not pressed down.

void setup() 
{
    pinMode(7, INPUT);
    Serial.begin(9600); //opens serial port, sets data rate to 9600 bps
    Serial.print("Sketch:   ");   Serial.println(__FILE__);
    Serial.print("Uploaded: ");   Serial.println(__DATE__);
    Serial.println(" ");
    pinMode(SW_pin, INPUT);//joystick push button
    pinMode(state_pin, INPUT);//read state pin 
    pinMode(break_pin, OUTPUT);//break connection pin
    digitalWrite(SW_pin, HIGH); //hvorfor?
    //bluetooth initialize
    BTserial.begin(9600);  
    Serial.println("BTserial started at 9600");
    Serial.println("Initializing the master...."); 
    initializeMaster();
    Serial.println("Done!"); 
    //Serial.println("This module is connected to its slave"); //the module will still be single letters at each iteration after this to complete the operation
}
void loop()
{
    // if bluetooth connection is established
    if (digitalRead(state_pin) == HIGH)
    {
      // connect if the joystick is pressed down
      SW_val = digitalRead(SW_pin);
      if (SW_val == 0){ 
        // break connection
        digitalWrite(break_pin, LOW);
      }
      //For debugging/development purposes
      //readFromModuleAndDisplayInMonitor();
      //readFromMonitorAndSendToModule();
      
      //get the joystick input
      int x = map(analogRead(X_pin),0,1023,0,100);
      int y = map(analogRead(Y_pin),0,1023,0,100);
      Serial.print(x);
      Serial.print("  ");
      Serial.println(y);
      //Packing the Serial message
      // full speed forwards
      if ((y <= 100 && y >= 98) && (x <= 51 && x >= 49))
      {
        BTserial.write("a");
      }
      // turn right
      else if ((y <= 55 && y >= 45) && (x <= 100 && x >= 60))
      {
        BTserial.write("b");
      }
      // turn left
      else if ((y <= 55 && y >= 45) && (x <= 10 && x >= 0))
      {
        BTserial.write("c");
      }
      // stop
      if ((y <= 5 && y >= 0) && (x <= 51 && x >= 49))
      {
        BTserial.write("d");
      }
      // auto forward
      if ((y == 0 && x ==0))
      {
        BTserial.write("e");
        delay(500); // just enough for you to remove your thumb again, so you don't send more than one "e"
      }
      //BTserial.print(x); //print() converts it to binary (a byte) for us. Else use write().
      
      //Serial.print("  ");
      //Serial.println(y);
    }
    else{
      // connect if the joystick is pressed down
      SW_val = digitalRead(SW_pin);
      if (SW_val == 0){ 
        connectToSlave();
      }
    }
    
    
}











void initializeMaster()
{
  BTserial.print("AT+RENEW" ); //FACTORY RESET
  delay(1000); // These are required
  BTserial.print("AT+IMME1" ); //Make sure the module is set to manual connect
  delay(1000); 
  BTserial.print("AT+ROLE1" ); //Make sure it is set to master
  delay(1000);
  BTserial.print("AT+PIO11" ); //Do not blink. HIGH when connected, LOW when not
  delay(1000); 
  BTserial.print("AT+RESET" ); //Reset to apply changes
  delay(1000); 
  // you could leave out the AT+RESET and just move AT+ROLE1 down as AT+ROLE also restarts the module by itself
}
void connectToSlave()
{
  BTserial.print("AT+CON508CB16603EB" ); //Connect to the slave module using its address
  Serial.println("Connection Established");
  delay(1000);
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
    // read data only when you receive data
    if (Serial.available())
    {
        // read the incoming byte
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
