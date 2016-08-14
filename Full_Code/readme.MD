# Duck Documentation
## DAC-ADC parts and assembly

The product is based on openDACs's recipe. For more details:
http://opendacs.com/dac-adc-homepage/


#Communication
We copied most of OpenDAC's instructions that you can read about here:

http://opendacs.com/dac-adc-homepage/how-to-communicate-with-the-dac-adc/


The duck has two new commands:

* SINE
* SINE_READ	


## Sine Read
Command signature:
	SINE_READ, DAC_CHANNEL, ADC_CHANNEL, DC_CURRENT, AC_CURRENT (RMS), Frequency (Hz), POINTS_ON_GRAPH

#### DAC_CHANNEL
- Output channel. In addition to the requested channel, the same signal would come out of channel 3 as reference channel. The AC current of the reference channel can be changed using the RF command (more details below).
- currently the reference channel outputs no DC current?

#### ADC_CHANNEL



## To do
* Buggy at high voltages
* On\Off read
* simpler commands to begin voltage (Set DC command)
* DC current on reference channel?



## Extras
* ?Display, Leds?