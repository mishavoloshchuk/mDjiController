#include "Joystick.h"
#include "..\inc\vjoyinterface.h"

Joystick::Joystick(int id, int logging)
{
	interfaceId = id;
	if (logging == 0) {
		log = true;
	}
	else {
		log = false;
	}

	printf("Creating vJoy connection\n");

	if (!vJoyEnabled()) {
		printf("vJoy driver not enabled: Failed Getting vJoy attributes.\n");
		return;
	} else {
		printf("Driver found:\nVendor:%S, Product:%S, Ver:%S\n", TEXT(GetvJoyManufacturerString()), TEXT(GetvJoyProductString()), TEXT(GetvJoySerialNumberString()));
	};

	VjdStat status = GetVJDStatus(interfaceId);
	switch (status)
	{
	case VJD_STAT_OWN:
		printf("vJoy Device %d is already owned by this feeder\n", interfaceId);
		break;
	case VJD_STAT_FREE:
		printf("vJoy Device %d is free\n", interfaceId);
		break;
	case VJD_STAT_BUSY:
		printf("vJoy Device %d is already owned by another feeder\nCannot continue\n", interfaceId);
		return;
	case VJD_STAT_MISS:
		printf("vJoy Device %d is not installed or disabled\nCannot continue\n", interfaceId);
		return;
	default:
		printf("vJoy Device %d general error\nCannot continue\n", interfaceId);
		return;
	};

	if (!AcquireVJD(interfaceId)) {
		printf("Failed to acquire vJoy Interface number %d\n", interfaceId);
		return;
	}
	else {
		ResetVJD(interfaceId);
		connected = true;
		printf("Acquired vJoy Interface number %d\n", interfaceId);
	}
}

int pow(int a, int b) {
	int n = 1;
	for (int i = 0; i < b; i++) {
		n *= a;
	}
	return n;
}


void Joystick::update(int l_hor, int l_ver, int r_hor, int r_ver, int lever_left, int buttons, short wheel, int camera)
{
	// Read values from left lever
	// P-mode | A-mode | F-mode
	if (lever_left >= 32) {
		fMode = 2;
	}
	else if (lever_left >= 16) {
		fMode = 1;
	}
	else if (lever_left >= 0) {
		fMode = 0;
	}

	// The values from the controller are between ~750 and ~3450, so we just scale them between 0 and 32000 (maximum range in vJoyMonitor)
	l_hor -= 750;
	l_ver -= 750;
	r_hor -= 750;
	r_ver -= 750;

	l_hor *= 12;
	l_ver *= 12;
	r_hor *= 12;
	r_ver *= 12;

	camera *= 8;

	// The values are from 0 to 2 then it converts from 0 to 32768
	fMode *= 16384;

	// Read and process buttons data from buttons. Variable "buttons" if button pressed is the sum of exponentiation of number two (sum of button_(0+1)\(1+1)\(2+1)... = 2^2\2^4\2^8...). For example: if pressed button[3] and button[5], will equal 2^3 + 2^5 = 40
	if (buttons < 0) { // button[6] (RTH button) == -128
		buttons += 128;
		btns[6] = true;
	}
	else {
		btns[6] = false;
	}
	// Other buttons
	for (int i = 6; i > 0; --i) {
		if (buttons - pow(2, i) >= 0) {
			btns[i - 1] = true;
			buttons -= pow(2, i);
		}
		else {
			btns[i - 1] = false;
		}
	}

	// Read wheel scroll direction from wheel variable
	if (wheel > wheel_prev) { // If current wheel position higher than previous
		rWheelLeft = true;
		rWheelRight = false;
		wheel_prev = wheel;
	}
	else if (wheel < wheel_prev) {
		rWheelLeft = false;
		rWheelRight = true;
		wheel_prev = wheel;
	}
	else {
		rWheelLeft = rWheelRight = false;
	}

	if (log) {
		printf("L vert: %-5d | L hori: %-5d | R vert: %-5d | R hori: %-5d | btn1: %-d | btn2: %-d | btn3: %-d | btn4: %-d | btn5: %-d | btn6: %-d | btn7: %-d | R wheel left: %-d | R wheel right: %-d | camera: %-5d | flight mode: %-5d\n", l_ver, l_hor, r_ver, r_hor, btns[0], btns[1], btns[2], btns[3], btns[4], btns[5], btns[6], rWheelLeft, rWheelRight, camera, fMode);
	}

	// Send stick values to vJoy
	SetAxis(l_hor, interfaceId, HID_USAGE_X);
	SetAxis(l_ver, interfaceId, HID_USAGE_Y);
	SetAxis(r_hor, interfaceId, HID_USAGE_Z);
	SetAxis(r_ver, interfaceId, HID_USAGE_RX);
	SetAxis(camera, interfaceId, HID_USAGE_RY);
	SetAxis(fMode, interfaceId, HID_USAGE_RZ);
	
	// Send button values to vJoy
	SetBtn(btns[0], interfaceId, 1);    // C1
	SetBtn(btns[1], interfaceId, 2);    // C2
	SetBtn(btns[2], interfaceId, 3);    // Right wheel button
	SetBtn(btns[3], interfaceId, 4);    // Play button
	SetBtn(btns[4], interfaceId, 5);    // Camera shutter
	SetBtn(btns[5], interfaceId, 6);    // Rec
	SetBtn(btns[6], interfaceId, 7);   // Return To Home (RTH)
	SetBtn(rWheelLeft, interfaceId, 8); // Wheel scroll -
	SetBtn(rWheelRight, interfaceId, 9); // Wheel scroll +
}

Joystick::~Joystick()
{
	RelinquishVJD(interfaceId);
	printf("Disconnected from joystick\n");
}