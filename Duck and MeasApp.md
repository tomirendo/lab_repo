# Duck and MeasApp
<!---
	This file looks much better if you open it from here:
		https://github.com/tomirendo/lab_repo/blob/master/Duck%20and%20MeasApp.md

	Alternatively, you can open this file locally in Chrome if you have the "MarkDown Preview Plus" plugin with "Allow access to file URLs" enabled (can be done here: "chrome://extensions/")

	To see the Math properly, check "Enable LaTeX delimiters" in the "MarkDown Preview Plus" plugin settings.
	
-->
## Required Drivers
On windows (and probably on other platforms as well) the Arduino Due that manages that duck requires a driver. I am not sure which one so I would simply recommend installing the Arduino IDE :

https://www.arduino.cc/en/Main/Software

In case this driver doesn't solve the problem, you can install the specific Arduino Due Driver inside the Arduino IDE application (Tools -> Board : "NAME OF BOARD" -> Board manager. In the Board manager search for Arduino Due).

If it still doesn't work, use the information in this forum post:

http://forum.arduino.cc/index.php?topic=104698.0

## Communication with the duck using MeasApp

The duck has 2 states:
* DC and ADC on as many ports as needed
* AC+DC on a single port, and AC on a single reference port (port 3)



## DC and ADC

	
## Begin AC+DC on port:

Signature:


	set duck.AC{port number} {Frequency} {points on graph}

* Port Number - can either be 0, 1 or 2
* Frequency - in Hz
* Points on graph - number of voltage points the DAC will actually produce. More points will produce a more precise signal.

##### Examples:
	
	set duck.AC2 17 80

runs AC+DC on port 2, 17Hz and 80 points on the graph. The default values of the AC, DC and RF current are 0 (more details are below).


## Update AC,DC,RF on a running AC+DC signal

If the duck is running an AC+DC signal on a port, you can update the signal parameters using the set or sweep commands.

The Min\Max voltage for the duck is $\pm 10 volt$. 'AC' and 'DC' commands would not respond if $|AC| + |DC| > 10 $.
#### AC
Updates the AC voltage of the output port currntly running AC+DC signal.
signature:

	set duck.AC {voltage} 
	sweep duck.AC {begin_voltage} {end_voltage} {steps}

* Voltage - The AC voltage in RMS on the current AC+DC port

##### Example:
	
	set duck.AC 0.03

sets the AC voltage to $30 milivolt$ on the current AC+DC port.

#### DC
Updates the DC voltage of the output port currntly running AC+DC signal.
signature:
	
	set duck.DC {voltage}  
	sweep duck.DC {begin_voltage} {end_voltage} {steps}

* Voltage - The DC voltage on the AC+DC port

##### Example 1:
	
	set duck.DC -9

Sets the DC voltage on the AC+DC port to $-9 \ volt$

##### Example 2:	

	sweep duck.DC -9 9 100

sweeps from -9 to 9 volt in 100 steps.
	

#### RF 
Updates the AC reference voltage that comes out of port 3 (Right hand side of the box).

signature :
	
	set duck.RF {voltage}

* Voltage - The AC voltage on the reference port (3) in RMS.

##### example
	
	set duck.RF 1

sets the AC voltage out of the reference port (3) to $1\  volt$.

