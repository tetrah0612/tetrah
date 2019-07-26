#include <SMObject.h>
#include <SMStructs.h>
#include <conio.h>
#include <Windows.h>
#include <iostream>
#include <tlhelp32.h>
#include <tchar.h>

#define CRIT_MASK 0x17
#define NONCRIT_MASK 0x08
#define NUM_UNITS 4

using namespace std;
using namespace System;
using namespace System::Threading;

//Function declaration
bool IsProcessRunning(const char* processName);

//defining start up sequence
// Start up sequence
TCHAR* Units[10] = //
{
	TEXT("LaserModule.exe"),
	TEXT("GPSModule.exe"),
	TEXT("XBoxModule.exe"),
	TEXT("VehicleModule.exe"),
	//TEXT("DisplayModule.exe"),
};

int main(){

	//Variables
	int Non_Crit_Mask_Count = 0;
	int Crit_Mask_Count = 0;
	// Module execution based variable declarations
	STARTUPINFO s[10];
	PROCESS_INFORMATION p[10];

	////////////////////////////////////////////////////////////
	//CREATE AND ACCESS SHARED MEMORY
	////////////////////////////////////////////////////////////

	SMObject PMObj(_TEXT("PMObj"), sizeof(ProcessManagementStruct));
	SMObject LaserObj(_TEXT("LaserObj"), sizeof(LaserStruct));
	SMObject GPSObj(_TEXT("GPSObj"), sizeof(GPSStruct));
	SMObject VehicleObj(_TEXT("VehicleObj"), sizeof(VehicleStruct));
	SMObject XboxObj(_TEXT("XboxObj"), sizeof(RemoteStruct));


	//pointer to structure
	ProcessManagementStruct* PMSMPtr = nullptr;
	RemoteStruct* XboxSMPtr = nullptr;
	XboxSMPtr = (RemoteStruct*)XboxObj.pData;

	//creating shared memory and success?
	//Set up shared memory
	PMObj.SMCreate();
	GPSObj.SMCreate();
	LaserObj.SMCreate();
	VehicleObj.SMCreate();
	XboxObj.SMCreate();


	XboxObj.SMAccess();
	if (XboxObj.SMAccessError) {
		Console::WriteLine("Shared memory access failed");
		return -3;
	}

	//acessing shared memory and success?
	//PM
	PMObj.SMAccess();
	if (PMObj.SMAccessError){
		Console::WriteLine("PM Shared memory access failed");
		return -2;
	}

	////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////

	// Starting the processes
	for (int i = 0; i < NUM_UNITS; i++)
	{
		// Check if each process is running
		if (!IsProcessRunning(Units[i]))
		{
			ZeroMemory(&s[i], sizeof(s[i]));
			s[i].cb = sizeof(s[i]);
			ZeroMemory(&p[i], sizeof(p[i]));
			// Start the child processes.

			if (!CreateProcess(NULL,   // No module name (use command line)
				Units[i],        // Command line
				NULL,           // Process handle not inheritable
				NULL,           // Thread handle not inheritable
				FALSE,          // Set handle inheritance to FALSE
				CREATE_NEW_CONSOLE,              // No creation flags
				NULL,           // Use parent's environment block
				NULL,           // Use parent's starting directory
				&s[i],            // Pointer to STARTUPINFO structure
				&p[i])           // Pointer to PROCESS_INFORMATION structure
				)
			{
				printf("%s failed (%d).\n", Units[i], GetLastError());
				_getch();
				return -1;
			}
		}
		std::cout << "Started: " << Units[i] << endl;
		Sleep(1000);
	}

	////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////

	//pointer typecasted to access 
	//pData is pointer to void
	PMSMPtr = (ProcessManagementStruct*)PMObj.pData;

	PMSMPtr->ShutDown.Flags.PM = 0;
	PMSMPtr->HeartBeats.Status = 0x00;

	////////////////
	//while PM is on (not shutdown)
	while (!PMSMPtr->ShutDown.Flags.PM){  

		PMSMPtr->HeartBeats.Flags.PM = 1;
		PMSMPtr->PMHeartBeats.Status = 0xFF; //Am alive.  Telling all modules

		cout << "PM:" << int(PMSMPtr->HeartBeats.Flags.PM) << "   ";
		cout << "Laser: " << int(PMSMPtr->HeartBeats.Flags.Laser) << "   ";
		//cout << "Display: " << int(PMSMPtr->HeartBeats.Flags.Display) << "   ";
		cout << "GPS: " << int(PMSMPtr->HeartBeats.Flags.GPS) << "   ";
		cout << "XBox: " << int(PMSMPtr->HeartBeats.Flags.Xbox) << "   ";
		cout << "Vehicle: " << int(PMSMPtr->HeartBeats.Flags.Vehicle) << std::endl;
		//Console::WriteLine("{0,10:F3}", PMSMPtr->HeartBeats.Flags.GPS);

		//Set all modules back to 0
		PMSMPtr->HeartBeats.Status = 0x00;

		//Check if the xbox button was pressed
		//if (XboxSMPtr->XboxTerminate) //if xbox gives terminate command
		//{
		//	break; //leave while loop
		//}

		System::Threading::Thread::Sleep(500);

		//Keep Looping
		if (_kbhit()){
			break;
		}
	}

	//Shutdown All
	PMSMPtr->ShutDown.Status = 0xFF;

	//at termination
	//shutdown modules in the specified order
	//PMSMPtr->ShutDown.Flags.Vehicle = 1;
	//System::Threading::Thread::Sleep(200);
	//PMSMPtr->ShutDown.Flags.Laser = 1;
	
	cout << "Terminating normally" << endl;

	return 0;
}

//Is process running function
bool IsProcessRunning(const char* processName)
{
	bool exists = false;
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry))
		while (Process32Next(snapshot, &entry))
			if (!_stricmp(entry.szExeFile, processName))
				exists = true;

	CloseHandle(snapshot);
	return exists;
}