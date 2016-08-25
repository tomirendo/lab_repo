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
* __**DC and ADC**__ - Write DC values on 4 DAC ports, and read DC values from 4 ADC ports.
* __**AC+DC**__ - Outputs a sine wave with additional DC current on the 4 DAC ports.


## DC and ADC

The duck works in 2 states. The DC and ADC states can both Write DC values to any of the 4 ports, and read DC values from any of its 4 ADC ports. 

### Write DC
Signature:


	set duck.DC{port} {voltage}

* Port - Port to update
* Voltage - Voltage to write on port.

##### Examples:
	
	set duck.DC2 -4

Updates the output voltage on port number 2 to $-4 \ Volts$.


### Read ADC
Signature:

	duck.ADC{port}

* Port - ADC Port to read

##### Examples:
	
	duck.ADC2 

In the measuring list, will read port 2 of the ADC.

	
## AC+DC:

Enters AC+DC mode. In this mode the duck changes every DAC port to 0 voltage, and begin running a sine wave on all of its ports. The sine waves begin with both AC and DC parameters as 0 (so you will see a permanent 0 DC voltage from all of them). 

After running this command you can update the AC and DC parameters of each port using the commands below.


### Bugs
Make sure that we you do anything in AC+DC mode, not to have any duck.ADC* on your measuring list.

** This is a bug in the way the duck communicates with MeasApp, hopefuly this will be fixed soon.** 

### Begin AC+DC
Signature:


	set duck.AC {Frequency} {points on graph}

* Frequency - in Hz
* Points on graph - number of voltage points the DAC will actually produce. More points will produce a more precise signal.

##### Examples:
	
	set duck.AC 17 80

runs AC+DC with frquency of 17Hz and 80 points on the graph. The default values of the AC and DC are 0 (more details are below).

## Update AC,DC on a running AC+DC signal

If the duck is running an AC+DC signal on a port, you can update the signal parameters using the set or sweep commands.

The Min\Max voltage for the duck is $\pm 10 volt$. updating the AC or DC of any port would not respond if $|AC| + |DC| > 10 $.
#### AC
Updates the AC voltage of the a port.
signature:

	set duck.AC{port}AC {voltage}
	sweep duck.AC{port}AC {begin_voltage} {end_voltage} {steps}

* Port - port to update
* Voltage - The AC voltage in RMS on the current AC+DC port

##### Example:
	
	set duck.AC2AC 0.03

sets the AC voltage to $30 milivolt$ on port number 2.

#### DC
Updates the DC voltage of the output port currntly running AC+DC signal.
signature:
	
	set duck.AC{port}DC {voltage}  
	sweep duck.AC{port}DC {begin_voltage} {end_voltage} {steps}

* Port - port to update
* Voltage - The DC voltage on the port

##### Example 1:
	
	set duck.AC2DC -9

Sets the DC voltage on port number 2 to $-9 \ volt$

##### Example 2:	

	sweep duck.AC2DC -9 9 100

sweeps from -9 to 9 volt in 100 steps on port number 2.