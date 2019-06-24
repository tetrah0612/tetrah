#include <sys/io.h>
#include <iostream>

#define PCM_BASE 0x300

int AnalogIn(int BASE, unsigned char range, unsigned char channel);


int main(){
	if(ioperm(PCM_BASE,16,1) != 0 ){

	std::cout << "fail perm" <<std::endl;
	}

	//output to low
	char Low_B = 0; 
	outb(Low_B, PCM_BASE + 0x3);
 
	//output to low
	char High_B = 0xFF;
	outb(High_B, PCM_BASE + 0xB);

	//now extract analog from analog output board
	//top lcd channel 0
	//bottom lcd channel 1
	char channel1;
	char channel0; 
	int readingC0 = AnalogIn(PCM_BASE, 0, channel0);
	int readingC1 = AnalogIn(PCM_BASE, 0, channel1);
	
	std::cout << readingC0 << std::endl;
	std::cout << readingC1 << std::endl;

	return 0;
}

int AnalogIn(int BASE, unsigned char range, unsigned char channel){

	int Low_B, High_B;

	//Set range
	outb(range, BASE + 1);
	//Select channel
	outb(channel|(channel <<4), BASE+2);
	//sleep 0.1 sec
	usleep(100000000);
	//Trigger conversion
	outb(0x00, BASE+0x0);
	//Wait for conversion to complete
	while(inb(BASE+0x08) & 0x80);
	Low_B = inb(BASE + 0x00);
	High_B = inb(BASE =0x01);

	//return converted data
	return (Low_B >>4) | (High_B << 4);

}