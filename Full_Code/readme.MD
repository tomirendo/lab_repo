# Duck Documentation

<!---
	This file looks much better if you open it from here:
		https://github.com/tomirendo/lab_repo/blob/master/Full_Code/readme.MD

	Alternatively, you can open this file locally in Chrome if you have the "MarkDown Preview Plus" plugin with "Allow access to file URLs" enabled (can be done here: "chrome://extensions/")

	To see the Math properly, check "Enable LaTeX delimiters" in the "MarkDown Preview Plus" plugin settings.
	
-->

### DAC-ADC parts and assembly

![](http://i.imgur.com/j1jujAp.jpg=100x20)

The product is based on openDACs's recipe. For more details:
http://opendacs.com/dac-adc-homepage/

### Output Voltage
The minimum step of the output voltage is approx:

$$ \Delta V = \frac{Max - Min}{2^{bits}} = \frac{10 - (-10)}{2^{16}} \approx 0.3\cdot10^{-3} volt $$

with this output voltage we can get a clear signal for $AC \approx 30 \cdot 10 ^{-3}\  volt$, and a good signal for $AC \approx 3 \cdot 10 ^-3\  volt$ (RMS).
### Communication
We copied most of OpenDAC's instructions that you can read about here:
http://opendacs.com/dac-adc-homepage/how-to-communicate-with-the-dac-adc/

The device communicates through the serial port (baudrate 115200) and accepts commands like:

	RAMP1,2,-4,4.8,50,10\r

Ramps the voltage on port 2 from $-4 \ volt$ to $4.8 \ volt$ in 50 steps with 10 microseconds of delay. Commands always end with '\r' (13 in ASCII). The device ignores any '\n's.

### The Duck version

The duck version currently has 1 additional command:

* SINE

After the initial connection to the device, the Duck sends a single '\n' that should be cleared from its output (or just read but the software).

## Sine
Command signature:

	SINE,{DAC_CHANNEL},{DC_VOLTAGE},{AC_AMPLITUDE},{FREQUENCY},{POINTS_ON_GRAPH}\r

* DAC_CHANNEL - Output channel. In addition to the requested channel, the same signal would come out of channel 3 as reference channel. The AC current of the reference channel can be changed using the RF command (more details below).
* DC_VOLTAGE - The initial DC voltage out of the output channel. This doesn't affect the reference port.
* AC_AMPLITUDE - The initial AC voltage amplitude out of both the output channel and the reference channel.
* FREQUENCY -  The frequency of the AC signal in Hz
* POINTS_ON_GRAPH - The amount of data point the arduino will draw to simulate an AC current. More points would amount to a clearer signal. Too many point would slow the process down and result in a lower frequency.

The calculation of the amplitude at a given point is:
$A_{mp} = DC_VOLTAGE \cdot sin(step / POINTS_ON_GRAPH) \cdot AC_AMPLITUDE$
#### Output 
The command outputs:

        Sine with read is running real freq : {real_freq}

Ending with a line break. {real_freq} is replace with the real expected frequency that results from the rounding errors.

#### Example 

    SINE,2,1,0.03,17,80\r

Would produce a DC voltage of $1 \ volt$ on both port 2 and port 3 (reference), with additional AC current with amplitude $30 \ milivolt$ using 80 points on the graph.

### The SINE state
The SINE command differs from other DAC-ADC command since it never stops. You can only use a small set of commands to update the SINE signal that runs on the selected port. In order to change the status of the device (to produce a RAMP or even a different SINE signal) the Duck should be restarted (In matlab, simply using fclose(serial_device) and the fopen(serial_device))

#### AC
#### DC
#### RF

### To do
* On\Off read


### Extras
* ?Display, Leds?