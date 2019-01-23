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
int photocellPin = A5;     // the cell and 10K pulldown are connected to a0
int photocellReading;     // the analog reading from the sensor divider
// Ultrasonic sensor
#include <NewPing.h>
#define TRIGGER_PIN 11
#define ECHO_PIN 10
#define MAX_DISTANCE 200
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance
//hm10 connection state pin
int state_pin = 13;
void setup() 
{   
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

    pinMode(state_pin, INPUT);//read state pin 
}
void loop()
{
    // don't do anything unless the connection is on
    if (digitalRead(state_pin) == HIGH)
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
          
          // Forwards
          if (x == 97) //decimal code for "a"
          {
            //Serial.println("Forward");
            // Set motors A direction
            digitalWrite(in1, HIGH);
            digitalWrite(in2, LOW);
            // Set motors B direction
            digitalWrite(in3, HIGH);
            digitalWrite(in4, LOW);
            //spin the motors
            analogWrite(enA, 255); // Send PWM signal to L298N Enable pin
            analogWrite(enB, 255); // Send PWM signal to L298N Enable pin
            autoOn = LOW;
          }
          // turn right
          else if (x == 98) //decimal code for "b"
          {
            //Serial.println("Right");
            // Set motors A direction
            digitalWrite(in1, HIGH);
            digitalWrite(in2, LOW);
            // Set motors B direction
            digitalWrite(in3, HIGH);
            digitalWrite(in4, LOW);
            //spin the motors
            analogWrite(enA, 255); // Send PWM signal to L298N Enable pin
            analogWrite(enB, 0); // Send PWM signal to L298N Enable pin
            autoOn = LOW;
          }
          // turn left
          else if (x == 99) //decimal code for "c"
          {
            //Serial.println("Left");
            // Set motors A direction
            digitalWrite(in1, HIGH);
            digitalWrite(in2, LOW);
            // Set motors B direction
            digitalWrite(in3, HIGH);
            digitalWrite(in4, LOW);
            //spin the motors
            analogWrite(enA, 0); // Send PWM signal to L298N Enable pin
            analogWrite(enB, 255); // Send PWM signal to L298N Enable pin
            autoOn = LOW;
          }
          // Reverse
          else if (x == 100) //decimal code for "c"
          {
            //Serial.println("Reversing");
            // Set motors A direction
            digitalWrite(in1, LOW);
            digitalWrite(in2, HIGH);
            // Set motors B direction
            digitalWrite(in3, LOW);
            digitalWrite(in4, HIGH);
            analogWrite(enA, 255); // Send PWM signal to L298N Enable pin
            analogWrite(enB, 255); // Send PWM signal to L298N Enable pin
            autoOn = LOW;
          }
          // break
          else if (x == 101 && autoOn == LOW)
          {
            //Serial.println("breaking");
            analogWrite(enA, 0); // Send PWM signal to L298N Enable pin
            analogWrite(enB, 0); // Send PWM signal to L298N Enable pin
          }
          // auto-mode
          else if (x == 102)
          {
            if (autoOn == LOW){
              // Set motors A direction
              digitalWrite(in1, HIGH);
              digitalWrite(in2, LOW);
              // Set motors B direction
              digitalWrite(in3, HIGH);
              digitalWrite(in4, LOW);
              analogWrite(enA, 255); // Send PWM signal to L298N Enable pin
              analogWrite(enB, 255); // Send PWM signal to L298N Enable pin
              Serial.println("Automatic mode engaged");
              Serial.println("");
              autoOn = HIGH;
            }
            else if (autoOn == HIGH){
              analogWrite(enA, 0); // Send PWM signal to L298N Enable pin
              analogWrite(enB, 0); // Send PWM signal to L298N Enable pin
              Serial.println("Automatic mode ended");
              Serial.println("");
              autoOn = LOW;
            }
          }
      }
      else
      {
        //analogWrite(enA, 0); // Send PWM signal to L298N Enable pin
        //analogWrite(enB, 0); // Send PWM signal to L298N Enable pin
      }
      if (autoOn == HIGH)
      {
        // get sonar reading
        delay(50);
        unsigned int distance = sonar.ping_cm();
        if (distance < 4 && distance > 0)
        {
          analogWrite(enA, 0); // Send PWM signal to L298N Enable pin
          analogWrite(enB, 0); // Send PWM signal to L298N Enable pin
          autoOn = LOW;
          Serial.println("Automatic mode stopped. Obstacle detected.");
          Serial.println("");
        }
        else
        {
          // get solar cell reading
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
    }else{
      analogWrite(enA, 0);
      analogWrite(enB, 0);
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
