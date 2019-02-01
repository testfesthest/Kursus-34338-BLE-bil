//----------------------------------------//
//------------- Joystick -----------------//
//----------------------------------------//
 
#include <AltSoftSerial.h> // https://www.pjrc.com/teensy/td_libs_AltSoftSerial.html
AltSoftSerial BTserial; // make global instance

// Define global variables and constants
char c=' '; //contains the byte received from BLE module
//hm10
const int state_pin = 12;
const int break_pin = 13;
// Joystick
const int SW_pin = 2;
const int X_pin = 0;
const int Y_pin = 1;
int carMode = 0; // each mode (number) corresponds to a certain speed and direction for the car.
int SW_val = 1; // 0 means joystick pressed down. 1 means not pressed down.

void setup() 
{
    //opens serial ports and sets data rate to 9600 bps
    Serial.begin(9600);
    BTserial.begin(9600);
    pinMode(SW_pin, INPUT);
    pinMode(state_pin, INPUT);
    pinMode(break_pin, OUTPUT);
    initializeMaster();
}
void loop()
{
    // if bluetooth connection is established
    if (digitalRead(state_pin) == HIGH)
    {
      // read if joystick is pressed down
      SW_val = digitalRead(SW_pin);
      if (SW_val == 0)
      {
         //auto-forward ON/OFF 
         BTserial.write("f");
         delay(500);
      }
      //For development purposes
      //readFromModuleAndDisplayInMonitor();
      //readFromMonitorAndSendToModule();
      
      //get the joystick  x and y input
      int x = map(analogRead(X_pin),0,1023,0,100);
      int y = map(analogRead(Y_pin),0,1023,0,100);
      // Forwards
      if ((y <= 100 && y >= 98) && (x <= 60 && x >= 40))
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
      // reverse
      if ((y <= 5 && y >= 0) && (x <= 60 && x >= 40))
      {
        BTserial.write("d");
      }
      // break
      if ((y <= 52 && y >= 48) && (x <= 52 && x >= 48))
      {
        BTserial.write("e");
      }
    }
    else{
      // if joystick is pressed down
      SW_val = digitalRead(SW_pin);
      if (SW_val == 0)
      {
          connectToSlave();
          delay(500); //to prevent it from spinning
      }
    }
    delay(100); //to prevent a traffic jam in seial buffer on slave module
}

// initializes the connected hm10 module
void initializeMaster()
{
  Serial.println("Initializing the master...."); 
  BTserial.print("AT+RENEW" ); //FACTORY RESET
  delay(1000); // These are required
  BTserial.print("AT+IMME1" ); //Set module to manual connect
  delay(1000); 
  BTserial.print("AT+ROLE1" ); //Set module to master
  delay(1000);
  BTserial.print("AT+PIO11" ); //Do not blink. HIGH when connected, LOW when not
  delay(1000); 
  BTserial.print("AT+RESET" ); //Reset to apply changes
  delay(1000);
  Serial.println("Done!");
}
// establishes connection to the joystick
void connectToSlave()
{
  BTserial.print("AT+CON508CB16603EB" ); //slave module address is 508CB16603EB
  Serial.println("Connection Established");
  delay(1000);
}
// Read from the hm10 and send to the Serial Monitor (used for testing & configuration)
void readFromModuleAndDisplayInMonitor()
{
    // send data only when you receive data
    if (BTserial.available())
    {
        // read the incoming byte
        c = BTserial.read();
        Serial.write(c);
    }
}
// Sends user's serial monitor input to the hm10 (used for testing & configuration)
void readFromMonitorAndSendToModule()
{
    // read data only when you receive data
    if (Serial.available())
    {
        c = Serial.read();
        BTserial.write(c); // do not send line end characters to the HM-10 (c=10 or c=13)
    }
}
