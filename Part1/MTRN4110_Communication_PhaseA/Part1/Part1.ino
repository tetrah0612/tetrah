//MTRN4110 Phase A
//Part 1
// AT Mode 'OK'
//Liam Ho z5114385
/*
1. The demonstrator inputs “AT” in the serial monitor of Bluetooth and receives a response “OK” (2)
*/




/////////////////////
// PUT IN "BOTH NL & CR MODE" !!!!!!!!!
/////////////////////



char InSerial = '0';
const byte  LEDPIN = 13;  //On board LED
 
void setup() 
{
    pinMode(LEDPIN, OUTPUT); //Set pin to output
 
    // communication with the host computer
    Serial.begin(9600);  //for serial monitor
    Serial.println("Do not power the Bluetooth module");
    Serial.println(" ");
    Serial.println("On the Bluetooth module, plug the EN pin, then plug power");
    Serial.println("The LED on the Bluetooth module should now flash every 2 seconds");
    Serial.println(" ");
    Serial.println("After entering AT mode, type 1 and hit send");
    Serial.println(" ");
 
 
    // wait for the user to type "1" in the serial monitor
    while (Serial.read() !='1')
    {
        if ( Serial1.available() )
        {  
          InSerial = Serial1.read();  
        }
    }  
 
 
    // communication with the BT module on serial1
    Serial1.begin(38400);
 
    // LED to show we have started the serial channels
    digitalWrite(LEDPIN, HIGH);  
 
    Serial.println(" ");
    Serial.println("HC-06 in AT mode.");
    Serial.println("Remember to to set Both NL & CR in the serial monitor and baud.");
    Serial.println("Remember ONLY Upper case.");
    Serial.println(" "); 
}
 
void loop() 
{
    // listen for communication from the BT module and then write it to the serial monitor
    if (Serial1.available())   
    {  
      Serial.write(Serial1.read());  
    }
 
    // listen for user input and send it to the HC-06
   if (Serial.available())
   {  
    Serial1.write(Serial.read());  
   }
}
