from serial import Serial

def init_serial(serial_device):
    serial_device.flush()
    serial_device.reset_output_buffer()
    print(serial_device.readline().decode())
    
def run_operation(serial_device, op = "NOP", ver = False, read = True):
    if isinstance(op, str):
        op = op.encode()
    serial_device.write(op + b"\r")
    if read:
        data = serial_device.readline().decode()
        if ver:
            print("OP {} returned {}".format(op.decode(),data))
        return data
    if ver:
        print("OP {} was sent".format(op.decode()))

class Duck:
    def __init__(self, *serial_device_parameters):
        self.serial_device = Serial(*serial_device_parameters)
        init_serial(self.serial_device)

    def __enter__(self):
        self.serial_device.__enter__()
        return self

    def __exit__(self, *exp):
        self.serial_device.__exit__(*exp)

    def run(self, op = "NOP", verbose = False, read = True):
        run_operation(self.serial_device, op, verbose, read)


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
    from collections import namedtuple
    from numpy import arange
    from pandas import DataFrame
    result = namedtuple("SineBufferReading","data, reference, orthogonal_reference")
    command = "BUFFER_SINE,{},{},{},{},{},{},{}\r".format(dac_channel, adc_channel, mid_voltage,
                amplitude, frequency, steps, iterations)
    serial_device.write(command.encode())
    while not serial_device.readline().startswith(b"BEGIN_PRINT_DATA"):
        pass
    data = [serial_device.readline()]
    while data[-1] != "BEGIN_PRINT_REF":
        data.append(serial_device.readline().decode().replace("\r\n",""))
    ref = [serial_device.readline()]
    while ref [-1] != "BEGIN_ORTHOGONAL_REF_DATA":
        ref.append(serial_device.readline().decode().replace("\r\n",""))
    orth_ref = [serial_device.readline()]
    while orth_ref[-1] != "BUFFER_SINE_FINISHED":
        orth_ref.append(serial_device.readline().decode().replace("\r\n",""))
    ref, data, orth_ref = ref[:-1], data[:-1], orth_ref[:-1]
    result_as_list = [result(*list(map(float,i))) for i in zip(data,ref,orth_ref)]
    waiting_time = 1/(steps * frequency)
    df = DataFrame(result_as_list, index = arange(0, len(data)) * waiting_time)
    return df

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