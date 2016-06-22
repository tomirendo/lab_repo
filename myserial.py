from serial import Serial

def init_serial(serial_device):
    serial_device.flush()
    print(serial_device.readline().decode())
    
def run_operation(serial_device, op = "NOP", ver = False):
    if isinstance(op, str):
        op = op.encode()
    serial_device.write(op + b"\r")
    data = serial_device.readline().decode()
    if ver:
        print("OP {} returned {}".format(op.decode(),data))
    return data

operators = ["NOP", 
             "SET", 
             "GET_ADC", 
             "RAMP1", 
             "RAMP2", 
             "BUFFER_RAMP", 
             "RESET", "TALK", "CONVERT_TIME", 
             "*IDN?", "*RDY?"]