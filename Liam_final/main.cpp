#include "EmbeddedOperations.h"
#include "DAC06.h"
#include "MSIP404.h"
#include "PCM3718.h"

#include <iostream>


int main() {

	EmbeddedOperations eops;

// 	EmbeddedDevice::PCM3718 pcm(&eops, 0x300);
 	EmbeddedDevice::MSIP404 msi(&eops, 0x200);
//	EmbeddedDevice::DAC06   dac(&eops, 0x320);

	std::cout << "testing mains functions.\n";

//////////////
//PCM
/////////////
// 	//read in full digital input
// 	uint16_t full = pcm.digitalInput();
// 
// 	// read in the high byte
// 	uint8_t highHalf = pcm.digitalByteInput(1);
// 
// 	// read in the low byte
// 	uint8_t lowHalf = pcm.digitalByteInput(0);
// 
// 	usleep(5000000);
// 
// 	if (full != (highHalf << 8 | lowHalf)) {
// 		std::cout << "input was wrong" << std::endl;
// 	}
// 	else {
// 		std::cout << "input seems correct" << std::endl;
// 	}
// 
// 
// 	if (pcm.digitalBitInput(0)) {
// 		std::cout << "Bit zero was on" << std::endl;
// 	}
// 	else {
// 		std::cout << "bit zero was off" << std::endl;
// 	}
// 
// 
// 	if (pcm.digitalBitInput(7)) {
// 		std::cout << "Bit seven was on" << std::endl;
// 	}
// 	else {
// 		std::cout << "bit seven was off" << std::endl;
// 	}
// 
// 
// 	if (pcm.digitalBitInput(8)) {
// 		std::cout << "Bit eight is always on" << std::endl;
// 	}
// 	else {
// 		std::cout << "this shouldnt happen" << std::endl;
// 	}
// 
// 	std::cout << "sleeping" << std::endl;
// 	usleep(5000000);
// 
// 	//set range for +-5v
// 	pcm.setRange(0);
// 
// 	std::cout << "analog input for channel 0 is " << pcm.analogInput(0) << std::endl;
// 
// 	std::cout << pcm << std::endl;
// 
// 	std::cout << "sleeping" << std::endl;
// 	usleep(5000000);
// 
// 	std::cout << "outputting 0xAA to high byte" << std::endl;
// 	pcm.digitalByteOutput(1, 0xAA);
// 	std::cout << "outputting 0xF7 to low byte" << std::endl;
// 	pcm.digitalByteOutput(0, 0xF7);
// 
// 	std::cout << "sleeping" << std::endl;
// 	usleep(5000000);
// 	std::cout << "outputting 0x83be to bytes" << std::endl;
// 	pcm.digitalOutput(0x83be);

//////////////
//DAC //DONE	
/////////////
	//DONE	
	//std::cout << "outputting 4095 (+5V) to the dac" << std::endl;
	//dac.analogOutputRaw(0, 4095); //output 5V
	//std::cout << "outputting 0 (-5v)  to the dac" << std::endl;
	//dac.analogOutputRaw(0, 0); //output -5V
	//usleep(5000000);

	//DONE	
	//std::cout << "outputting -1.28V to the dac" << std::endl;
	//dac.analogOutputVoltage(0, -3);

//////////////
//MSI
/////////////
	//Done
 	usleep(5000000);
 	std::cout << "reset channel 0" << std::endl;
 	msi.resetChannel(0);
// 
// 	std::cout << "printing encoder every 0.1 seconds for the next 10 seconds\n";
// 	usleep(5000000);
// 	for (int i = 0; i < 100; i++) {
// 		std::cout << msi.readChannel(0) << std::endl;
// 		usleep(100000);
// 	}

	std::cout << "checking index pulse the next period\n";
	usleep(5000000);
	bool pulse = false;
	for (int i = 0; i < 1000000; i++) {
		if (msi.readIndex(0)) {
			pulse = true;
			break;
		}
	}
//fix
	std::cout << "Index pulse detected " << pulse << std::endl;
	
	for (int i = 0; i < 100; i++) {
		
		if(msi.readIndex(0) == 1){
				std::cout << "Index pulse detected "<< std::endl;
				msi.resetChannel(0);

		} else {

			std::cout << msi.readChannel(0) << std::endl;
		}

		//usleep(100000);
	}

// 	std::cout << "reset all channels" << std::endl;
// 	!msi;

// 	if (msi.readChannel(0) != 0) {
// 		std::cout << "reset failed\n";
// 	}
// 	else {
// 		std::cout << "reset successful\n";
// 	}
// 
// 	std::cout << "completed\n";

	return 0;
}


