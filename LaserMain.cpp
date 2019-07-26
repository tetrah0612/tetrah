#using <System.dll>

#include <conio.h>
#include <SMObject.h>
#include <SMStructs.h>
#include <Windows.h>
#include <iostream>
#include <sstream>
#include <String>
#include <math.h> 



//Either include metadata from System.dll by including it here or by going to project properties
// and then C++->Advanced->Forced #using file

#define WAIT_TIME 40
#define pi 3.1415926535

///////////////////////////////////////////
//////////////////////////////////////////
using namespace System::Threading;
using namespace System;
using namespace System::Net::Sockets;
using namespace System::Net;
using namespace System::Text;


//////////////////////////////////////////
/////////////////////////////////////////

value struct Point_Local{ //CLI struct
	double LocalX;
	double LocalY;
};

ref class Ref_Laser{ //must be a reference class because we are using gcnew
private:
	// Pointer to TcpClent type object on managed heap
	TcpClient^ Client;
	// Get the network stream object associated with client so we. can use it to read and write
	NetworkStream^ Stream;
	System::String^ Host_Name;
	double StartAngle; //print as double
	double Resolution; //print as double
	int Port_Num;
	int NumberData; //print as int
	//Data array
	array<System::String^>^ LaserDataArray; //to access individual stuff

	// arrays of unsigned chars to send and receive data
	array<unsigned char>^ SendData; // used for sending the string to acsss the connection

	void GetStartAngle() {
		Int32 value = System::Convert::ToInt32(LaserDataArray[23], 16); // located 23 spaces in array
		StartAngle = double(value) / 10000;
		//print value
		std::cout << "Start Angle: " << StartAngle << std::endl;
	}

	void GetResolution(){
		Resolution = 0.0;
		Int16 value = System::Convert::ToInt16(LaserDataArray[24], 16); // located 24 spaces in array
		Resolution = double(value) / 10000; // Angular step width
		//print value
		std::cout << "Resolution:" << Resolution << std::endl;
	}

	void RangeUpdate(){
		LaserRanges = gcnew array<Point_Local>(NumberData);
		double Angle = StartAngle;

		for (int i = 0; i < NumberData; i++){
			int16_t Point = System::Convert::ToInt16(LaserDataArray[26 + i], 16); //Data_1 to Data_N
			LaserRanges[i].LocalY = -double((Point * cos(Angle * (pi / 180)))); //mm
			LaserRanges[i].LocalX = double((Point * sin(Angle * (pi / 180)))); //mm
			Angle = Angle + Resolution; //Resolution is 0.5
		}
	}

public:
	array<Point_Local>^ LaserRanges;
	array<unsigned char>^ AllLaserData = gcnew array<unsigned char>(2048); //entire line of data
	Ref_Laser() {}
	~Ref_Laser() {};
	Ref_Laser(System::String^ hostName, int portNumber){
		Host_Name = hostName;
		Port_Num = portNumber;
		Client = gcnew TcpClient(Host_Name, Port_Num);

		// Client settings configuration
		Client->NoDelay = true;
		Client->ReceiveBufferSize = 1024;
		Client->ReceiveTimeout = 500; //ms
		Client->SendBufferSize = 1024;
		Client->SendTimeout = 500;//ms

		Stream = Client->GetStream();

		//authenticate with the server
		//send student number
		System::String^ StudentNumber = gcnew System::String("z5114385\n");
		array<unsigned char>^ zID = gcnew array<unsigned char>(128);
		zID = System::Text::Encoding::ASCII->GetBytes(StudentNumber);
		Stream->Write(zID, 0, zID->Length);

		//check response
		array<unsigned char>^ Response = gcnew array<unsigned char>(2048); //create array
		Stream->Read(Response, 0, Response->Length); //read in data and store into Response
		System::String^ r = System::Text::Encoding::ASCII->GetString(Response); //Response to ASCII and save to r
		System::Console::WriteLine(r); //write
	}

	int GetNumberData() {
		NumberData = 0;
		uint16_t value = System::Convert::ToUInt16(LaserDataArray[25], 16); // located 25 spaces in array
		NumberData = int(value)-1;
		//print value
		std::cout << "Number of Points: " << NumberData << std::endl;

		return NumberData;
	}

	array<Point_Local>^ GetSingleScan(){
		// String command to ask 
		System::String^ AskScan = gcnew System::String("sRN LMDscandata"); //String to send (write)
		// String to store received data for display
		System::String^ ResponseData;

		//Convert String to bytes to send
		SendData = System::Text::Encoding::ASCII->GetBytes(AskScan);

		// Write command asking for data
		Stream->WriteByte(0x02);
		Stream->Write(SendData, 0, SendData->Length);
		Stream->WriteByte(0x03);
		// Wait for the server to prepare the data, 1 ms would be sufficient, but used 10 ms
		System::Threading::Thread::Sleep(10);

		if (Client->Available != 0) {
			Int32 ReadBytes = Stream->Read(AllLaserData, 0, AllLaserData->Length); // Read the incoming data in AllLaserData variable

			if (ReadBytes == 0) {
				return nullptr;
			}
			else {
				// Convert incoming data from an array of unsigned char bytes to an ASCII string
				ResponseData = System::Text::Encoding::ASCII->GetString(AllLaserData);
				// Print the received string on the screen
				Console::WriteLine(ResponseData);
				//Store into array with deliminated by spaces
				LaserDataArray = ResponseData->Split(' ');  //parsing string into array

				//Get values and store for later use
				GetNumberData();
				GetStartAngle();
				GetResolution();
				
				RangeUpdate(); // update the cartesian of the points

				return LaserRanges;
			}
		}
	}
};

int main() {

	//Variables
	int PMHeartBeatCount = 0;

	//creating object
	SMObject LaserObj(_TEXT("LaserObj"), sizeof(LaserStruct));
	SMObject PMObj(_TEXT("PMObj"), sizeof(ProcessManagementStruct));

	//pointer to structure 
	ProcessManagementStruct* PMSMPtr = nullptr;
	LaserStruct* LaserSMPtr = nullptr;

	//Access
	PMObj.SMAccess();
	LaserObj.SMAccess();

	//pointer typecasted to access 
	PMSMPtr = (ProcessManagementStruct*)PMObj.pData;
	LaserSMPtr = (LaserStruct*)LaserObj.pData;

	//Create an object for Laser
	Ref_Laser^ LaserRef = gcnew Ref_Laser("192.168.1.200", 23000);

	PMSMPtr->ShutDown.Flags.Laser = 0;
	while (!PMSMPtr->ShutDown.Flags.Laser) {

		//put flag up regardless
		PMSMPtr->HeartBeats.Flags.Laser = 1;

		//check PM module flag
		//if (PMSMPtr->PMHeartBeats.Flags.Laser) {
		//	PMSMPtr->PMHeartBeats.Flags.Laser = 0;
		//	PMHeartBeatCount = 0;
		//}
		//else {
		//	if (++PMHeartBeatCount > WAIT_TIME) { //if PM is very critical and is not working. terminate all
		//		PMSMPtr->ShutDown.Status = 0xFF;
		//	}
		//}

		////Laser Printing Stuff
		LaserRef->GetSingleScan();
		LaserSMPtr->NumberPoints = LaserRef->GetNumberData();
		for (int i = 0; i < LaserSMPtr->NumberPoints; i++){
			LaserSMPtr->XRange[i] = LaserRef->LaserRanges[i].LocalX;
			LaserSMPtr->YRange[i] = LaserRef->LaserRanges[i].LocalY;
			std::cout << "X Range: " << LaserSMPtr->XRange[i] << " Y Range: " << LaserSMPtr->YRange[i] << std::endl;
		}

		System::Threading::Thread::Sleep(1000);
	}

	Console::WriteLine("Process management terminated normally.");
	System::Threading::Thread::Sleep(200);


	return 0;
}
