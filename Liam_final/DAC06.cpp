#include <stdint.h>
#include <iostream>
#include <sys/io.h>
#include "EmbeddedOperations.h"
#include "DAC06.h"


#define MAX_BIT 4095

//constructor
EmbeddedDevice::DAC06::DAC06(EmbeddedOperations* eops, uint32_t base_addr) {
	this->eops = eops; //pointer
	DAC_BASE = base_addr;

	if (eops->ioperm(DAC_BASE, 16, 1) != 0) {
		std::cout << "fail perm" << std::endl;
	}
}

//Deconstructor
EmbeddedDevice::DAC06::~DAC06() { 
}

//Output the raw value provided to the specified channel in the DAC 
//TESTED: GOOD
void EmbeddedDevice::DAC06::analogOutputRaw(uint8_t channel, uint16_t value) {
	//split into 2 values
	// 8 bits (low) -> 4 bits (high nibble) =  12 bits

	//8 bits for LS
	uint8_t temp1 = (value >> 4);
	uint8_t LS_Byte = temp1;
	//4 bits for MS
	uint8_t temp2 = (value >> 8);
	uint8_t MS_Nibble = temp2;

	//write
	eops->outb(LS_Byte, DAC_BASE + (channel * 2));
	eops->outb(MS_Nibble, DAC_BASE + (channel * 2) + 1);
}

//Output the desired voltage provided to the specified channel in the DAC
//TESTED: DONE
void EmbeddedDevice::DAC06::analogOutputVoltage(uint8_t channel, double desired_voltage) {
	//DAC set at standard +-5V Range
	//gradient = 10 / MAX_BIT; //from -5 to +5
	//refer to manual

	uint16_t value = ((desired_voltage + (0.5*10))/10)*4095;	

	analogOutputRaw(channel, value);
}