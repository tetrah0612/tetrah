//MTRN4110 Phase A
//Part 2 and Part 3
//Liam Ho z5114385
/*
2. The demonstrator inputs a string on an external device, send the string to the Arduino
through Bluetooth and display the string on the serial monitor of Bluetooth (2) and the LCD
display (2) – (Total 4)
3. The demonstrator inputs a string in the serial monitor of the Bluetooth, send the string to the
external device and display on the external device (2)
*/

/////////////////////////
//USING PUTTY

//SET TO NO LINE ENDING

//FOR TASK 2  
// YOU DONT NEED TO PRESS ENTER

//FOR TASK 2  
// YOU DO NEED TO PRESS ENTER
/////////////////////////


#include <Wire.h>
#include "DFRobot_LCD.h"

//Create lcd object
DFRobot_LCD lcd(16,2);  //16 characters and 2 lines of show

char string = '0';
const byte  LEDPIN = 13;  //On board LED

void setup() 
{
    pinMode(LEDPIN, OUTPUT); //Set pin to output
    
    // initialize lcd
    lcd.init();
 
    // communication with the host computer
    Serial.begin(9600);  //for serial monitor
        
    // communication with the BT module on serial1
    Serial1.begin(9600);
 
    // LED to show we have started the serial channels
    digitalWrite(LEDPIN, HIGH);  
}
 
void loop() 
{
    //Part 2
    // listen for communication from the BT module and then write it to the serial monitor and display to lcd
    if (Serial1.available())   
    {  
        char s = Serial1.read();
        Serial.write(s);
        lcd.write(s);
    }

   //Part 3
   // listen for user input and send it to the HC-05
   if ( Serial.available() )   
   { 
         string = Serial.read();
         Serial1.write(string); 
   }

}
