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


The duck version currently has 1 additional command:

* SINE


## Sine
Command signature:

	SINE,{DAC_CHANNEL},{Frequency},{POINTS_ON_GRAPH}

* DAC_CHANNEL
 - Output channel. In addition to the requested channel, the same signal would come out of channel 3 as reference channel. The AC current of the reference channel can be changed using the RF command (more details below).

#### ADC_CHANNEL



### To do
* On\Off read


### Extras
* ?Display, Leds?