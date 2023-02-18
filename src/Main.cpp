
#include <Windows.h>
#include <stdio.h>
#include <string>
#include <iostream>

#include "Serial.h"
#include "Joystick.h"

//#pragma comment(lib, "User32.lib")

short littleEndiansToShort(int first, int second) {
	if (first < 0) {
		first = 256 + first;
	}

	short combined = second << 8 | first;
	return combined;

}

void run(char* portName, int stickId, int logging) {
	char initData[] = { 0x55, 0xaa, 0x55, 0xaa, 0x1e, 0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x80, 0x00, 0x04, 0x04, 0x74, 0x94, 0x35, 0x00, 0xd8, 0xc0, 0x41, 0x00, 0x30, 0xf6 };
	char pingData[] = { 0x55, 0x0D, 0x04, 0x33, 0x0A, 0x03, 0x04, 0x00, 0x40, 0x00, 0x0E, 0xD0, 0xE3, 0x55, 0x0D, 0x04, 0x33, 0x0A, 0x0E, 0x05, 0x00, 0x40, 0x06, 0x27, 0x58, 0x35 };

	char incomingData[256] = "";
	int dataLength = 256;
	int readResult = 0;
	bool shouldRun = true;

	Joystick j(stickId, logging);
	if (!j.isConnected()) {
		printf("Couldn't connect to vJoy, quitting...\n");
		return;
	}
	printf("\n");

	Serial s(portName);
	if (!s.IsConnected()) {
		printf("Couldn't connect to COM port, quitting...\n");
		return;
	}

	printf("\nEverything is ready\n\n");
	s.WriteData(initData, 34);

	printf("Running...\nPress SHIFT + ESC to quit\n");
	Sleep(2000);

	while (s.IsConnected() && shouldRun)
	{	
		s.WriteData(pingData, 39); // write this once in a while, otherwise it stops sending? :O
		readResult = s.ReadData(incomingData, dataLength);

		if (readResult == 58 && incomingData[0] == 0x55) { // probably positioning data
			short right_vertical = littleEndiansToShort(incomingData[16], incomingData[17]);
			short right_horizontal = littleEndiansToShort(incomingData[18], incomingData[19]);

			short left_vertical = littleEndiansToShort(incomingData[20], incomingData[21]);
			short left_horizontal = littleEndiansToShort(incomingData[22], incomingData[23]);

			short left_lever = incomingData[28];
			short buttons = incomingData[29];
			short wheel = incomingData[26];

			short camera = littleEndiansToShort(incomingData[24], incomingData[25]);

			// update our virtual joystick
			j.update(left_horizontal, left_vertical, right_horizontal, right_vertical, left_lever, buttons, wheel, camera);
		}
		Sleep(10);


		if ( GetKeyState(VK_ESCAPE) <= -127 && GetKeyState(VK_SHIFT) <= -127) {
			shouldRun = false;
			printf("\n\nDetected SHIFT + ESC, quitting...\n");
		}
	}

}

int main() {
	std::string in;
	int portNr, stickId, logging;


	printf("Select port number (default COM \"3\"): ");
	std::getline(std::cin, in);
	portNr = atoi(in.c_str());

	if (portNr < 1) {
		portNr = 3;
	}

	char port[100];
	sprintf_s(port, "COM%d", portNr);


	printf("Select vJoy configuration (default \"1\"): ");
	std::getline(std::cin, in);
	stickId = atoi(in.c_str());

	if (stickId < 1) {
		stickId = 1;
	}


	printf("Disable logging? 1 for off (default \"0\"): ");
	std::getline(std::cin, in);
	logging = atoi(in.c_str());

	if (logging != 0) {
		logging = 1;
	}

	printf("Starting...\n\n");
	run(port, stickId, logging);
	printf("Closing down...\n\n");


	system("pause");
	return 0;
}