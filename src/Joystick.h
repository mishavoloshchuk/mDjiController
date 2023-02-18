#pragma once

#include <Windows.h>
#include <stdio.h>

#define HID_USAGE_X		0x30
#define HID_USAGE_Y		0x31
#define HID_USAGE_Z		0x32
#define HID_USAGE_RX	0x33
#define HID_USAGE_RY	0x34
#define HID_USAGE_RZ	0x35

class Joystick
{
private:
	bool btns[7], rWheelLeft, rWheelRight;
	short wheel_prev; // Previous wheel position
	bool connected;
	unsigned int interfaceId;
	bool log = true;
	int fMode = 0;
public:
	Joystick(int interfaceId, int logging);
	~Joystick();
	void update(int l_hor, int l_ver, int r_hor, int r_ver, int toggle_left, int buttons, short wheel, int camera);
	bool isConnected() { return connected;  }
};