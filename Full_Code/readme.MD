# Duck Documentation
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



### To do
* On\Off read


### Extras
* ?Display, Leds?