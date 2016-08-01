from serial import Serial

def init_serial(serial_device):
    serial_device.flush()
    serial_device.reset_output_buffer()
    print(serial_device.readline().decode())
    
def run_operation(serial_device, op = "NOP", ver = False):
    if isinstance(op, str):
        op = op.encode()
    serial_device.write(op + b"\r")
    data = serial_device.readline().decode()
    if ver:
        print("OP {} returned {}".format(op.decode(),data))
    return data

def run_without_read(serial_device, op = "NOP", ver = False):
    if isinstance(op, str):
        op = op.encode()
    serial_device.write(op + b"\r")
    if ver:
        print("Done running {}".format(op))


def test_sine(serial_device):
    """
    if isinstance(str, dac_channels):
        dac_channels = "".join(dac_channels.split(","))
    if isinstance(str, adc_channels):
        adc_channels = "".join(adc_channels.split(","))
    """
    serial_device.write(b"*RDY?\r")
    reads= [serial_device.readline()]
    while reads[-1] != "BUFFER_SINE_FINISHED":
        reads.append(serial_device.readline().decode().replace("\r\n",""))
    return [float(i) for i in reads[:-1]]

def buffer_ramp(serial_device, dac_channels, adc_channels):
    """
    if isinstance(str, dac_channels):
        dac_channels = "".join(dac_channels.split(","))
    if isinstance(str, adc_channels):
        adc_channels = "".join(adc_channels.split(","))
    """
    serial_device.write(b"BUFFER_RAMP,2,2,-4.2,2.3,100,1000\r")
    reads= [serial_device.readline()]
    while reads[-1] != "BUFFER_RAMP_FINISHED":
        reads.append(serial_device.readline().decode().replace("\r\n",""))
    return [float(i) for i in reads[:-1]]

operators = ["NOP", 
             "SET", 
             "GET_ADC", 
             "RAMP1", 
             "RAMP2", 
             "BUFFER_RAMP", 
             "RESET", "TALK", "CONVERT_TIME", 
             "*IDN?", "*RDY?"]