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
// make global instance of bluetooth serial
AltSoftSerial BTserial; 
// https://www.pjrc.com/teensy/td_libs_AltSoftSerial.html

// Set global variables
char c=' '; //contains data received from BLE module
boolean NL = true;
String data; //Store each line of Serial communication
byte autoOn = LOW;
//motor shield
#define enA 7
#define in1 6
#define in2 5
#define in3 4
#define in4 3
#define enB 2
//Photosensor
int photocellPin = 0;     // the cell and 10K pulldown are connected to a0
int photocellReading;     // the analog reading from the sensor divider
 
void setup() 
{
    pinMode(7, INPUT);
    Serial.begin(9600);// opens serial port, sets data rate to 9600 bps
    Serial.print("Sketch:   ");   Serial.println(__FILE__);
    Serial.print("Uploaded: ");   Serial.println(__DATE__);
    Serial.println(" ");
    
    BTserial.begin(9600);  
    Serial.println("BTserial started at 9600"); 
    Serial.println("Initializing the slave...."); 
    initializeSlave();
    Serial.println("Done!"); 
    //motor shield
    pinMode(enA, OUTPUT);
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);
    pinMode(in3, OUTPUT);
    pinMode(in4, OUTPUT);
    pinMode(enB, OUTPUT);
    //pinMode(button, INPUT);
    // Set initial rotation direction
    // Set Motor A backward
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    // Set Motor B backward
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);

    //analogWrite(enA, 200); // Send PWM signal to L298N Enable pin
    //analogWrite(enB, 200); // Send PWM signal to L298N Enable pin
}
void loop()
{
    //For debugging/development purposes
    //readFromModuleAndDisplayInMonitor();
    //readFromMonitorAndSendToModule();
    
    // read data only when you receive data
    if (BTserial.available())
    {
        // read the incoming bytes (byte if you used .read())
        //char y = BTserial.read();
        char x = BTserial.read();
        // full speed forwards
        if (x == 97) //decimal code for "a"
        {
          Serial.write("a");
          analogWrite(enA, 255); // Send PWM signal to L298N Enable pin
          analogWrite(enB, 255); // Send PWM signal to L298N Enable pin
          autoOn = LOW;
        }
        // turn right
        else if (x == 98) //decimal code for "b"
        {
          Serial.write("b");
          analogWrite(enA, 255); // Send PWM signal to L298N Enable pin
          analogWrite(enB, 0); // Send PWM signal to L298N Enable pin
          autoOn = LOW;
        }
        // turn left
        else if (x == 99) //decimal code for "c"
        {
          Serial.write("c");
          analogWrite(enA, 0); // Send PWM signal to L298N Enable pin
          analogWrite(enB, 255); // Send PWM signal to L298N Enable pin
          autoOn = LOW;
        }
        // break
        else if (x == 100)
        {
          Serial.write("d");
          analogWrite(enA, 0); // Send PWM signal to L298N Enable pin
          analogWrite(enB, 0); // Send PWM signal to L298N Enable pin
          autoOn = LOW;
        }
        // drive forward until ...
        else if (x == 101)
        {
          Serial.write("e");
          analogWrite(enA, 255); // Send PWM signal to L298N Enable pin
          analogWrite(enB, 255); // Send PWM signal to L298N Enable pin
          autoOn = HIGH;
          Serial.println("Automatic mode engaged");
          Serial.println("");
        }
    }
    else
    {
      //analogWrite(enA, 0); // Send PWM signal to L298N Enable pin
      //analogWrite(enB, 0); // Send PWM signal to L298N Enable pin
    }
    if (autoOn == HIGH)
    {
      photocellReading = analogRead(photocellPin);
      Serial.print("Analog reading = ");
      Serial.println(photocellReading);// the raw analog reading
      if (photocellReading > 800)
      {
        analogWrite(enA, 0); // Send PWM signal to L298N Enable pin
        analogWrite(enB, 0); // Send PWM signal to L298N Enable pin
        autoOn = LOW;
        Serial.println("Automatic mode stopped. Sufficient solar power detected.");
        Serial.println("");
      }
    }
}














void initializeSlave()
{
  BTserial.print("AT+RENEW" ); //FACTORY RESET
  delay(1000); // These are required
  BTserial.print("AT+PIO11" ); //Do not blink. HIGH when connected, LOW when not
  delay(1000);
  BTserial.print("AT+RESET" ); //Reset to apply change
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
