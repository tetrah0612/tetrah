#include <stdint.h>
#include <sys/io.h>
#include <iostream>
#include "MSIP404.h"
//Encoder

#define Index_addr 0x800

	//constructor
EmbeddedDevice::MSIP404::MSIP404(EmbeddedOperations* eops, uint32_t base_addr) {
	this->eops = eops;
	BASE_MSIP = base_addr;

	if (eops->ioperm(BASE_MSIP, 16, 1) != 0) {
		std::cout << "fail perm" << std::endl;
	}
}

//Deconstructor
EmbeddedDevice::MSIP404::~MSIP404(){
}

//Reset the channel determined by the input value
void EmbeddedDevice::MSIP404::resetChannel(uint8_t channel) {
	//page 6 of Manual
	eops->outb(0x00, BASE_MSIP + channel);
}

//Read the channel determined by the input value
//You are expected to be able to read channels 0-7. Assume the same pattern as channel 0-3 for channels 4-7.
//DONE: TESTED
int32_t EmbeddedDevice::MSIP404::readChannel(uint8_t channel) {
	// 8 channels
	//each channel has 4 bytes
	//read from MSB to LSB

	uint32_t Byte_4 = eops->inb(BASE_MSIP + ((channel * 4) + 3));
	uint32_t Byte_3 = eops->inb(BASE_MSIP + ((channel * 4) + 2));
	uint32_t Byte_2 = eops->inb(BASE_MSIP + ((channel * 4) + 1));
	uint32_t Byte_1 = eops->inb(BASE_MSIP + (channel * 4));

	Byte_2 = (Byte_2 << 8);
	Byte_3 = (Byte_3 << 16);
	Byte_4 = (Byte_4 << 24);

	int32_t value_channel = Byte_1 + Byte_2 + Byte_3 + Byte_4;

	return value_channel;
}

//Read the index bit for the channel determined by the input value
//You may assume the encoder index pulse base address is always 0x800.
//You are expected to read the index pulses for channels 0-2 ONLY
//DONE: xxx
bool EmbeddedDevice::MSIP404::readIndex(uint8_t channel) {
	//Port to read index is 0x800
	uint32_t Read_Value;
	uint32_t Result;

	switch (channel){
	case 0: //if channel 0
		//read index value
		Read_Value = eops->inb(Index_addr);
		//bit mask to select only the bit 7
		Result = Read_Value & 0x80;
		break;
	case 1:
		Read_Value = eops->inb(Index_addr);
		//bit mask to select only the bit 5 
		Result = Read_Value & 0x20;
		break;
	case 2:
		Read_Value = eops->inb(Index_addr + 1);
		//bit mask to select only the bit 7
		Result = Read_Value & 0x80;
		break;
	}
	return Result;
}

//Reset all (8) channels
//DONE: xxx
bool EmbeddedDevice::MSIP404::operator!() {
	int count = 0;
	int channel_num = 0;
	int Result = 0;

	//loop through all 8 channels and reset them
	while (count < 8) {
		resetChannel(channel_num);
		count++;
		channel_num++;
	}

	//indicate that all is done resetting and return 1
	count = 0;
	if (eops->inb(BASE_MSIP + 0x07) == 0x00) {
		Result = 1;
	}
	else {
		Result = 0;
	}

	return Result;
}
