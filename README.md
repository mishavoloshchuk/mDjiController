mDjiController
===============
 - Connect your DJI Remote Controller to your PC and use it to play simulators.
 - Currently confirmed working controllers: DJI Phantom 3 Advanced
 - For Mavic Mini or a Python interface, take a look at [justin97530/miniDjiController](https://github.com/justin97530/miniDjiController)
 - DJI Phantom 2 version of mDjiController - [Matsemann/mDjiController](https://github.com/Matsemann/mDjiController)
-----------------------------------------------------------------------------

This is a program that connects to your DJI Phantom Remote Controller (RC),
reads the stick positions and tells Windows that position. A simplified, wannabe driver.

Installation / Usage
------------

In order for it to work, the original drivers from DJI must be installed, and something called vJoy.

* Download and install vJoy from here: https://sourceforge.net/projects/vjoystick
* Driver installing. There is no official driver for DJI Phantom 3 remote controller, 
	but the driver included in DJI Flight Simulator:  https://www.dji.com/simulator
	You can install only game launcher without installing the game.
	Later, you can even delete the launcher.
	Or you can manually install the driver from "Driver" folder from zip (the driver is also taken from DJI Flight Simulator). You can install it from the "Device manager".

* Then find "Configure VJoy" in windows start menu and open it. Activate the "Enable vJoy" checkbox, and set number of buttons to 9.

* Connect your RC to your computer via USB and turn it on.

* Run mDjiController.exe, and select the correct COM port. By default you should try writing "3".
* For vJoy, write 1 unless you use vJoy already and have different configurations.

Then open a simulator and calibrate the controller.

How does it work?
-----------------

* It is possible to ask the controller for its status when it's connected. So this program continuously
	reads the status. It connects through COM. The output from the controller is just a list of numbers,
	but sampling many enough such lists it's easy to see a pattern for which number means what. The biggest issue
	was that the numbers are little endian encoded and uses two's complement, so it took some time to understand how
	each number behaved.
	
* vJoy is a virtual joystick that can be installed on your computer. Windows think it's a normal joystick. mDjiController
	takes the stick positions from the controller and tells Windows that this virtual joystick has the same positions.


TROUBLESHOOTING
---------------
* If something doesn't work, make sure you have enabled logging, it may tell you the error.

* COM can't connect
	Try with other numbers instead of 3. Open up device-manager.
	You should see a category named "Ports (COM & LPT) and then a device named "DJI USB Virtual COM (COMX)"
	The X should be the number you should use.
	If there is nothing there, the driver from DJI is not installed correctly.
	
	If you have any of the DJI Assistant Software installed, make sure they are not running, as this program
	cannot connect then.
	
* vJoy can't connect
	The error message should tell you if it's installed or not. Install the vJoy driver if it's not there.
	
* Not all sticks or buttons work
	*If they look correct in the log:*
	Your vJoy configuration may be wrong. Open "Configure vJoy" and reset configuration 1, or make a new one
	and tell mDjiController.exe to use that configuration.
	*If they are NOT correct in the log:*
	Your controller sends in a different format, so you need to figure that out, edit the code and recompile. 
	(A bit advanced)

	
