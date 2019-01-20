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
byte carMode = 0; //
String data; //Store each line of Serial communication
//motor shield
#define enA 7
#define in1 6
#define in2 5
#define in3 4
#define in4 3
#define enB 2
 
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
        String x = "";
        String y = "";
        y = BTserial.read();
        x = BTserial.read();
        Serial.print(x);
        Serial.print("  ");
        Serial.println(y);
        Serial.println("___________");
        //problemet her er at ovenstående metode er meget primitiv. Det er slet ikke en selvfølge at bytes
        //afsendt i en bestemt rækkefølge kommer frem i samme rækkefølge - der er støj, små forskelle i
        // modulernes hastighed, forskydninger osv osv som gør at de nogle gange bytter plads. Inkorperer i 
        // stedet metoden brugt her, hvor han "pakker" serial data ind med header of footer og flag: 
        // https://arduino.stackexchange.com/questions/23964/bluetooth-sending-values-from-two-joysticks
        //delay(100);
        int xx = 0;
        int yy = 0;
        xx = x.toFloat();
        yy = y.toFloat();
        analogWrite(enA, yy); // Send PWM signal to L298N Enable pin
        analogWrite(enB, yy); // Send PWM signal to L298N Enable pi
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
