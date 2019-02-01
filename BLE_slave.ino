//----------------------------------------//
//---------------- Car -------------------//
//----------------------------------------//
 
#include <AltSoftSerial.h> // https://www.pjrc.com/teensy/td_libs_AltSoftSerial.html
AltSoftSerial BTserial; // make global instance

// Set global variables
char c=' '; //contains the byte received from BLE module
bool autoOn = false;
//motor shield
#define enA 7
#define in1 6
#define in2 5
#define in3 4
#define in4 3
#define enB 2
int motorSpeed = 200;
//Photosensor
#define SOLAR_PIN = A5;
int solarCellReading;
// Ultrasonic sensor
#include <NewPing.h>
#define TRIGGER_PIN 11
#define ECHO_PIN 10
#define MAX_DISTANCE 200
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
//hm10 connection state pin
#define STATE_PIN = 13;

void setup() 
{   
    // opens serial ports and sets data rate to 9600 bps
    Serial.begin(9600);
    BTserial.begin(9600);  
    Serial.println("BTserial started at 9600"); 
    initializeSlave();
    //motor shield
    pinMode(enA, OUTPUT);
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);
    pinMode(in3, OUTPUT);
    pinMode(in4, OUTPUT);
    pinMode(enB, OUTPUT);
    // Set initial rotation direction
      // Set Motors A
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
      // Set Motors B
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    pinMode(STATE_PIN, INPUT);
}
void loop()
{
    // if bluetooth connection is established
    if (digitalRead(STATE_PIN) == HIGH)
    {
      //For development purposes
      //readFromModuleAndDisplayInMonitor();
      //readFromMonitorAndSendToModule();
      
      // read data only when you receive data
      if (BTserial.available())
      {
          // read the incoming byte
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
            analogWrite(enA, motorSpeed);
            analogWrite(enB, motorSpeed);
            autoOn = false;
          }
          // turn right
          else if (x == 98) //decimal code for "b"
          {
            // Set motors A direction
            digitalWrite(in1, HIGH);
            digitalWrite(in2, LOW);
            // Set motors B direction
            digitalWrite(in3, LOW);
            digitalWrite(in4, HIGH);
            // Spin the motors
            analogWrite(enA, motorSpeed);
            analogWrite(enB, motorSpeed);
            autoOn = false;
          }
          // turn left
          else if (x == 99) //decimal code for "c"
          {
            // Set motors A direction
            digitalWrite(in1, LOW);
            digitalWrite(in2, HIGH);
            // Set motors B direction
            digitalWrite(in3, HIGH);
            digitalWrite(in4, LOW);
            // Spin the motors
            analogWrite(enA, motorSpeed);
            analogWrite(enB, motorSpeed);
            autoOn = false;
          }
          // Reverse
          else if (x == 100) //decimal code for "d"
          {
            // Set motors A direction
            digitalWrite(in1, LOW);
            digitalWrite(in2, HIGH);
            // Set motors B direction
            digitalWrite(in3, LOW);
            digitalWrite(in4, HIGH);
            // Spin the motors
            analogWrite(enA, motorSpeed);
            analogWrite(enB, motorSpeed);
            autoOn = false;
          }
          // break
          else if (x == 101 && autoOn == false)
          {
            analogWrite(enA, 0);
            analogWrite(enB, 0);
          }
          // auto-mode
          else if (x == 102)
          {
            if (autoOn == false){
              // Set motors A direction
              digitalWrite(in1, LOW);
              digitalWrite(in2, HIGH);
              // Set motors B direction
              digitalWrite(in3, LOW);
              digitalWrite(in4, HIGH);
              // Spin the motors
              analogWrite(enA, motorSpeed);
              analogWrite(enB, motorSpeed);
              Serial.println("Automatic mode engaged");
              Serial.println("");
              autoOn = true;
              // Disconnect to save power
              
            }
            //stop auto mode if button is pressed again
            else if (autoOn == true){
              analogWrite(enA, 0);
              analogWrite(enB, 0); 
              Serial.println("Automatic mode ended");
              Serial.println("");
              autoOn = false;
            }
          }
      }
      if (autoOn == true)
      {
        // get sonar reading
        delay(50);
        unsigned int distance = sonar.ping_cm();
        //Serial.println(distance);
        if (distance < 15 && distance > 0)
        {
          analogWrite(enA, 0);
          analogWrite(enB, 0);
          autoOn = false;
          Serial.println("Automatic mode stopped. Obstacle detected.");
          Serial.println("");
        }
        else
        {
          // get solar cell analog reading
          solarCellReading = analogRead(SOLAR_PIN);
          if (solarCellReading > 800)
          {
            analogWrite(enA, 0);
            analogWrite(enB, 0);
            autoOn = false;
            Serial.println("Automatic mode stopped. Sufficient solar power detected.");
            Serial.println("");
          }
        }
      } 
    }
    else
    {
      analogWrite(enA, 0);
      analogWrite(enB, 0);
    }
}

// initializes the connected hm10 module
void initializeSlave()
{
  Serial.println("Initializing the slave....");
  BTserial.print("AT+RENEW" ); //FACTORY RESET
  delay(1000); // These are required
  BTserial.print("AT+PIO11" ); //Do not blink. HIGH when connected, LOW when not.
  delay(1000);
  BTserial.print("AT+RESET" ); //Reset to apply change
  delay(1000); 
  Serial.println("Done!");
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
