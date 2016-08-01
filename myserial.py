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


def buffer_sine(serial_device, dac_channel, adc_channel, mid_voltage,
                amplitude, frequency, steps, iterations = 1):
    """
    if isinstance(str, dac_channels):
        dac_channels = "".join(dac_channels.split(","))
    if isinstance(str, adc_channels):
        adc_channels = "".join(adc_channels.split(","))
    """
    command = "BUFFER_SINE,{},{},{},{},{},{},{}\r".format(dac_channel, adc_channel, mid_voltage,
                amplitude, frequency, steps, iterations)
    serial_device.write(command.encode())
    reads= [serial_device.readline()]
    while reads[-1] != "BUFFER_SINE_FINISHED":
        reads.append(serial_device.readline().decode().replace("\r\n",""))
    return [float(i) for i in reads[:-1]]

def buffer_ramp(serial_device, dac_channel, adc_channel,
                begin_voltage, end_voltage, number_of_steps, 
                delay_in_microsecs):
    """
    if isinstance(str, dac_channels):
        dac_channels = "".join(dac_channels.split(","))
    if isinstance(str, adc_channels):
        adc_channels = "".join(adc_channels.split(","))
    """
    command = "BUFFER_RAMP,{},{},{},{},{},{}\r".format(dac_channel, adc_channel,
                begin_voltage, end_voltage, number_of_steps, 
                delay_in_microsecs)
    serial_device.write(command.encode())
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