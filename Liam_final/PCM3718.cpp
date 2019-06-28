#include <stdint.h>
#include <sys/io.h>
#include <iostream>
#include "PCM3718.h"

#define MAX_BITS 4095

EmbeddedDevice::PCM3718::PCM3718(EmbeddedOperations* eops, uint32_t base_addr) {
	PCM_BASE = base_addr;
	this -> eops = eops;  //pointer

        //Set range of PCM +-5V
        setRange(0);

	if (eops -> ioperm(PCM_BASE, 16, 1) != 0) {
		std::cout << "fail perm" << std::endl;
	}
}

EmbeddedDevice::PCM3718::PCM3718(EmbeddedOperations* eops, uint32_t base_addr, uint8_t analog_range) {
	PCM_BASE = base_addr;
	this -> eops = eops;  //pointer
	PCM_RANGE = analog_range;

        //Set range of PCM +-5V
        setRange(0);

	if (eops->ioperm(PCM_BASE, 16, 1) != 0) { 
		std::cout << "fail perm" << std::endl;
	}
}

EmbeddedDevice::PCM3718::~PCM3718() {  //Deconstructor
}

//PCM3718 Functions

//read in the high and low byte input, combine them and return the result
//Done: Tested
uint16_t EmbeddedDevice::PCM3718::digitalInput() {
	//16 bit for the purpose of combining later
	uint8_t LowB = eops->inb(PCM_BASE + 0x0003);//low
	uint8_t HighB = eops->inb(PCM_BASE + 0x000B);  //high
	uint16_t TempShift = (HighB << 8); //shift 8 bits of high byte
	uint16_t Combine_Result = TempShift | LowB; //combine with low byte
	return Combine_Result;
}

//return the byte as determined by the input argument (1 = high byte, 0 = low byte)
//Done: Tested
uint8_t EmbeddedDevice::PCM3718::digitalByteInput(bool high_byte) {
	uint8_t Byte_Result;
	if (high_byte == 1) {
		Byte_Result = eops->inb(PCM_BASE + 0x0B);
	}
	else if(high_byte == 0){
		Byte_Result = eops->inb(PCM_BASE + 0x03);
	}
	return Byte_Result;
}

//Return the value of the bit as determined by the input argument (value between 0-15)
//Done: Tested
bool EmbeddedDevice::PCM3718::digitalBitInput(uint8_t bit) {
	//Right shifting ID by position -> furthest spot to the right. 
	//Combining that with bitwise AND (&) with 1 will inform if the bit is set.
	uint16_t Value;
	uint16_t Result_Value;

	Value = digitalInput(); //function from earlier to read and combine high and low bytes
	Value = (Value >> bit);  //shift to right
	Result_Value = Value & 0x0001; // 1 if set, 0 if no set 
	return Result_Value;
}

//Output the provided value to both low and high channels
//Done: Tested
void EmbeddedDevice::PCM3718::digitalOutput(uint16_t value){

	//can do the shove the other bits into 8 bits
	//Set and organise bits
	uint8_t Set_Low = value & 0x00FF; //only consider bottom 8 bits
	uint8_t Set_High = (value >> 8);
	//Move into 8 bit 
	uint8_t L_Value = Set_Low;
	uint8_t H_Value = Set_High;
	//output
	eops->outb(L_Value, PCM_BASE + 0x03);
	eops->outb(H_Value, PCM_BASE + 0x0B);
}

//Output the provided value to the channel determined by the boolean input (1 = high byte, 0 = low byte)
//Done: Tested
void EmbeddedDevice::PCM3718::digitalByteOutput(bool high_byte, uint8_t value) {
	if (high_byte == 1) {  //high byte is 1
		eops->outb(value, PCM_BASE + 0x0B);
	}
	else if(high_byte == 0){   //low byte is 0
		eops->outb(value, PCM_BASE + 0x03);
	}
}

//Change the range for analog input to be the provided rangeCode (look in manual for range definitions)
//Done: Tested
void EmbeddedDevice::PCM3718::setRange(uint8_t new_analog_range) {
	//Don't do any I/O operations
	PCM_RANGE = new_analog_range;
	int i = 0;
	int divide_10 = PCM_RANGE & 0x3;

	if ((PCM_RANGE & 0xC) == 0) {
		Max_Range = 5;
	}
	else if (PCM_RANGE & 0x4) { 
		Max_Range = 10;
	}
	else if (PCM_RANGE & 0x8) {
		Max_Range = 10;
	}

	if (PCM_RANGE & 0x4) {
		Bipolar = 0;
	}
	else {
		Bipolar = 1;
	}

	//Other Ranges are different by divide 10
	while (i < divide_10) {
		Max_Range = Max_Range * 0.1;
		i++;
	}
}

//Receive the input in the analog channel provided, convert it to a voltage (determined by the setRange) and return it
//Done: Tested
double EmbeddedDevice::PCM3718::analogInput(uint8_t channel) const {
	//must be able to be any channel
	// you can select the channel and then use that stored range to set the range for just that channel. 
	
	if(channel < 2){
		uint8_t chan;
		double voltage;
		double gradient;

		//Read analog input procedure
		//select channel
		chan = channel | (channel << 4);
		eops->outb(chan, PCM_BASE + 0x02);
		//select Range
		eops->outb(PCM_RANGE, PCM_BASE + 1);
		//Sleep
		usleep(100000);
		//trigger conversion
		eops->outb(0x00, PCM_BASE);
		//wait for conversion
		while (eops->inb(PCM_BASE + 0x08) & 0x08);
		//Read input
		uint16_t LowB = eops->inb(PCM_BASE + 0x00); //read low
		uint16_t HighB = eops->inb(PCM_BASE + 0x01); //read high
		//Combine High and Low
		uint16_t Input_Contents = (LowB >> 4) | (HighB << 4);

		//Calculating the voltage using graph
		//Do calcs depending on bipolar or not 
		if (Bipolar == 0) {
			gradient = (Max_Range) / MAX_BITS;
			voltage = (Input_Contents * gradient);
		}
		else if (Bipolar == 1) {
			gradient = (2 * Max_Range) / MAX_BITS;
			voltage = (Input_Contents * gradient) - Max_Range;
		}
		return voltage;
	}
	else {
		std::cout << "Incorrect channel input.  Accepts channels 0 to 1." << std::endl;
		return 0;
	}

}

//Have it output in the following style, with voltages displayed to 2 d.p. "channel 1: <channel 1 voltage>\tchannel 2: <channel 2 voltage>\n"
//Done: Tested
std::ostream& EmbeddedDevice::operator<<(std::ostream & output, const EmbeddedDevice::PCM3718 & pcm) {
	//"channel 1: X  channel 2: Y"
	std::cout << "Channel 0:" << pcm.analogInput(0);
	std::cout << "	Channel 1:" << pcm.analogInput(1) << std::endl;
}
