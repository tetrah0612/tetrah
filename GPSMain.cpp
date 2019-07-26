#include <SMObject.h>
#include <SMStructs.h>
#include <conio.h>
#include <iostream>

#define WAIT_TIME 40
#define CRC32_POLYNOMIAL			0xEDB88320L

using namespace System;
using namespace System::Threading;
#using <System.dll>

using namespace System;
using namespace System::Net::Sockets;
using namespace System::Net;
using namespace System::Text;

#pragma pack(1)
struct GPS_Local{
	unsigned char Local_Header[4]; //make sure header is right
	unsigned char Local_Discard1[40];  // skip 44 bytes  //fill up Discards with content you being used
	double Local_Northing;
	double Local_Easting;
	double Local_Height;
	unsigned char Local_Discard2[40]; //skip 40 bytes
	unsigned long Local_CRC;  //4 bytes
};

///////////////////
//Function Declaration
//////////////////

unsigned long CRC32Value(int i);
unsigned long CalculateBlockCRC32(unsigned long ulCount, unsigned char* ucBuffer);

//////////////////
//////////////////

int main() {

	//creating object
	SMObject PMObj(_TEXT("PMObj"), sizeof(ProcessManagementStruct));

	//pointer to structure PM
	ProcessManagementStruct* PMSMPtr = nullptr;

	PMObj.SMAccess();
	if (PMObj.SMAccessError) {
		Console::WriteLine("Shared memory access failed");
		return -2;
	}

	//pointer typecasted to access 
	PMSMPtr = (ProcessManagementStruct*)PMObj.pData;

	//GPS Stuff

	int PortNumber = 24000;

	// Pointer to TcpClient type object on managed heap
	TcpClient^ Client;

	// arrays of unsigned chars to receive data
	array<unsigned char>^ ReadData;

	// Creat TcpClient object and connect to it
	Client = gcnew TcpClient("192.168.1.200", PortNumber);

	// Configure connection
	Client->NoDelay = true;
	Client->ReceiveTimeout = 500;//ms
	Client->SendTimeout = 500;//ms
	Client->ReceiveBufferSize = 1024;
	Client->SendBufferSize = 1024;

	// unsigned char arrays of 16 bytes each are created on managed heap
	ReadData = gcnew array<unsigned char>(112);

	NetworkStream^ Stream = Client->GetStream();

	//Variables
	int PMHeartBeatCount = 0;
	PMSMPtr->ShutDown.Flags.GPS = 0;

	while (!PMSMPtr->ShutDown.Flags.GPS) {
		//gps is alive
		PMSMPtr->HeartBeats.Flags.GPS = 1;

		////check PM module flag
		//if (PMSMPtr->PMHeartBeats.Flags.GPS) {
		//	PMSMPtr->PMHeartBeats.Flags.GPS = 0;
		//	PMHeartBeatCount = 0;
		//}
		//else {
		//	if (++PMHeartBeatCount > WAIT_TIME) { //if PM is very critical and is not working. terminate all
		//		PMSMPtr->ShutDown.Status = 0xFF;
		//	}
		//}

		if (Stream->DataAvailable) {
			// Wait for the server to prepare the data, 1 ms would be sufficient, but used 10 ms
			System::Threading::Thread::Sleep(10);
			// Read the incoming data (sends as binary)
			Stream->Read(ReadData, 0, ReadData->Length);
		}

		GPS_Local GPS;
		//Create pointer to point to the first byte of struct
		unsigned char* GPSBytePtr;
		//points to the first byte of GPS Data
		//&GPS is pointer to whole struct
		GPSBytePtr = (unsigned char*)& GPS;

		for (int i = 0; i < sizeof(GPS); i++)
		{
			*(GPSBytePtr + i) = ReadData[i];
		}
		
		//Print header
		std::cout << "header: "<< std::hex << int(GPS.Local_Header[0])  << " ";
		std::cout << std::hex << int(GPS.Local_Header[1]) << " ";
		std::cout << std::hex << int(GPS.Local_Header[2]) << " ";
		std::cout << std::hex << int(GPS.Local_Header[3]) << std::endl;

		//Print others data
		std::cout << "Northings: " << GPS.Local_Northing << std::endl;
		std::cout << "Eastings:	" << GPS.Local_Easting << std::endl;
		std::cout << "Height: " << GPS.Local_Height << std::endl;
		std::cout << "CRC: " << GPS.Local_CRC << std::endl;
		std::cout << "CRC (Check): " << CalculateBlockCRC32(108, GPSBytePtr) << std::endl;
		std::cout << "\n" << std::endl;

		System::Threading::Thread::Sleep(1500);
	}

	Console::WriteLine("Process management terminated normally.");
	System::Threading::Thread::Sleep(200);

	Stream->Close();
	Client->Close();

	return 0;
}


/*
CRC : A cyclic redundancy check(CRC) is an error - detecting code commonly used in digital networks and storage devices
	  to detect accidental changes to raw data.Blocks of data entering these systems get a short check value attached, 
	  based on the remainder of a polynomial division of their contents.
*/

unsigned long CRC32Value(int i)
{
	int j;
	unsigned long ulCRC;
	ulCRC = i;
	for (j = 8; j > 0; j--)
	{
		if (ulCRC & 1)
			ulCRC = (ulCRC >> 1) ^ CRC32_POLYNOMIAL;
		else
			ulCRC >>= 1;
	}
	return ulCRC;
}

/* --------------------------------------------------------------------------
Calculates the CRC-32 of a block of data all at once
-------------------------------------------------------------------------- */
unsigned long CalculateBlockCRC32(unsigned long ulCount, /* Number of bytes in the data block */ //108
	unsigned char* ucBuffer) /* Data block */
{
	unsigned long ulTemp1;
	unsigned long ulTemp2;
	unsigned long ulCRC = 0;
	while (ulCount-- != 0)
	{
		ulTemp1 = (ulCRC >> 8) & 0x00FFFFFFL;
		ulTemp2 = CRC32Value(((int)ulCRC ^ *ucBuffer++) & 0xff);
		ulCRC = ulTemp1 ^ ulTemp2;
	}
	return(ulCRC);
}

//Note: Data Block ucBuffer should contain all data bytes of the full data record except the checksum bytes.

/*
int main()
{
	//GPS dont need to authenicate and send request
	int PortNumber = 24000; //GPS port number
	// Pointer to TcpClent type object on managed heap
	TcpClient^ Client;
	// arrays of unsigned chars to receive data
	array<unsigned char>^ ReadData;
	// String to store received data for display
	System::String^ ResponseData;

	// Creat TcpClient object and connect to it
	Client = gcnew TcpClient("192.168.1.200", PortNumber);
	// Configure connection
	Client->NoDelay = true;
	Client->ReceiveTimeout = 500;//ms
	Client->SendTimeout = 500;//ms
	Client->ReceiveBufferSize = 1024;
	Client->SendBufferSize = 1024;

	// unsigned char arrays of 16 bytes each are created on managed heap
	ReadData = gcnew array<unsigned char>(2500);

	// Get the network streab object associated with clien so we 
	// can use it to read and write
	NetworkStream^ Stream = Client->GetStream();

	//Remove ":" from the string
//ResponseData = ResponseData->Replace(":", "");


	//Loop
	while (!_kbhit())
	{
		if (Stream->DataAvailable) {
			// Wait for the server to prepare the data, 1 ms would be sufficient, but used 10 ms
			System::Threading::Thread::Sleep(10);
			// Read the incoming data (sends as binary)
			Stream->Read(ReadData, 0, ReadData->Length);
			// Convert incoming data from an array of unsigned char bytes to an ASCII string
			ResponseData = System::Text::Encoding::ASCII->GetString(ReadData);
			// Print the received string on the screen
			Console::WriteLine(ResponseData);
		}
	}

	Stream->Close();
	Client->Close();

	Console::ReadKey();
	Console::ReadKey();


	return 0;
}*/