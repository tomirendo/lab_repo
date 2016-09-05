//Ardunio *DUE*code for controlling EVAL-AD7734 ADC and EVAL-AD5764 DAC
//Created by Andrea Young
//Modified by Carlos Kometter 7/7/2015
#include "SPI.h" // necessary library for SPI communication
#include <vector>
#include "math.h"
//#include "WaveTable.hpp"

#define SECONDS_IN_MILISECOND 0.001
#define MILISECONDS_IN_SECOND 1000
#define MICROSECONDS_IN_SECOND 1000000
#define PRE_ITERATIONS 3
#define NUMBER_OF_PORTS 4

int adc=52; //The SPI pin for the ADC
int dac=4;  //The SPI pin for the DAC
int ldac=6; //Load DAC pin for DAC. Make it LOW if not in use. 
int clr=5;  // Asynchronous clear pin for DAC. Make it HIGH if you are not using it
int reset=44 ; //Reset on ADC
int drdy=48; // Data is ready pin on ADC
int led = 32;
int data=28;//Used for trouble shooting; connect an LED between pin 13 and GND
int err=30;
const int Noperations = 12;
//hwm::WaveTable table(1024);
String operations[Noperations] = {"NOP", "SET", "GET_ADC", "RAMP1", "RAMP2", "BUFFER_RAMP", "RESET", "TALK", "CONVERT_TIME", "*IDN?", "*RDY?", "SINE"};

namespace std {
  void __throw_bad_alloc()
  {
    Serial.println("Unable to allocate memory");
  }

  void __throw_length_error( char const*e )
  {
    Serial.print("Length Error :");
    Serial.println(e);
  }
}

int twoByteToInt(byte DB1,byte DB2) // This gives a 16 bit integer (between +/- 2^16)
{
  return ((int)((DB1<<8)| DB2));
}
float map2(long x, long in_min, long in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void ID()
{
  Serial.println("DAC-ADC_AD7734-AD5764");
}

void RDY()
{
  Serial.println("READY");
}

void error()
{
  digitalWrite(err,HIGH);
  delay(3000);
  digitalWrite(err,LOW);
  delay(500);
}

void debug()
{
  digitalWrite(data,HIGH);
  delay(3000);
  digitalWrite(data,LOW);
  delay(3000);
}

void setup()
{
  Serial.begin(115200);
  Serial.write("\n");
  
  pinMode(ldac,OUTPUT);   
  digitalWrite(ldac,LOW); //Load DAC pin for DAC. Make it LOW if not in use. 
  pinMode(clr, OUTPUT);
  digitalWrite(clr,HIGH); // Asynchronous clear pin for DAC. Make it HIGH if you are not using it
  pinMode(reset, OUTPUT);
  pinMode(drdy, INPUT);  //Data ready pin for the ADC.  
  pinMode(led, OUTPUT);  //Used for blinking indicator LED
  digitalWrite(led, HIGH);
  pinMode(data, OUTPUT);

  digitalWrite(reset,HIGH);  digitalWrite(data,LOW); digitalWrite(reset,LOW);  digitalWrite(data,HIGH); delay(5);  digitalWrite(reset,HIGH);  digitalWrite(data,LOW);//Resets ADC on startup.  

  SPI.begin(adc); // wake up the SPI bus for ADC
  SPI.begin(dac); // wake up the SPI bus for ADC
  
  SPI.setBitOrder(adc,MSBFIRST); //correct order for AD7734.
  SPI.setBitOrder(dac,MSBFIRST); //correct order for AD5764.
  SPI.setClockDivider(adc,84);  //This can probably be sped up now that the rest of the code is better optimized. Limited by ADC
  SPI.setClockDivider(dac,84);  //This can probably be sped up now that the rest of the code is better optimized. Limited by ADC
  SPI.setDataMode(adc,SPI_MODE3); //This should be 3 for the AD7734
  SPI.setDataMode(dac,SPI_MODE1); //This should be 1 for the AD5764

  // Disables DAC_SDO to avoid interference with ADC
  SPI.transfer(dac,1,SPI_CONTINUE);
  SPI.transfer(dac,0,SPI_CONTINUE);
  SPI.transfer(dac,1);

  //Yotam debug led
  pinMode(14, OUTPUT);
  digitalWrite(14, HIGH); 
}

void blinker(int s){
    digitalWrite(data,HIGH);
    delay(s);
    digitalWrite(data,LOW);
    delay(s);
}

void sos(){
    blinker(50);
    blinker(50);
    blinker(50);
    blinker(500);blinker(500);blinker(500);blinker(50);blinker(50);blinker(50);}

int indexOfOperation(String operation)
{
  for(int index = 0; index < Noperations; index++)
  {
    if(operations[index] == operation)
    {
      return index;
    }
  }
  return 0;
}

void waitDRDY() {while (digitalRead(drdy)==HIGH){}}

void resetADC() //Resets the ADC, and sets the range to default +-10 V 
{
  digitalWrite(data,HIGH);digitalWrite(reset,HIGH);digitalWrite(reset,LOW);digitalWrite(reset,HIGH);
  SPI.transfer(adc,0x28);
  SPI.transfer(adc,0);
  SPI.transfer(adc,0x2A);
  SPI.transfer(adc,0);
}

void talkADC(std::vector<String> DB)
{
  int comm;
  comm=SPI.transfer(adc,DB[1].toInt());
  Serial.println(comm);
  Serial.flush();
}

int numberOfChannels(byte DB) // Returns the number of channels to write
{
  int number = 0;

  for(int i = 0; i <= 3; i++)
  {
    if(((DB >> i) & 1) == 1)
    {
       number++;
    }
  }
  return number;
}

std::vector<int> listOfChannels(byte DB) // Returns the list of channels to write
{
  std::vector<int> channels;

  int channel = 0;
  for(int i = 3; i >= 0; i--)
  {
    if(((DB >> i) & 1) == 1)
    {
       channels.push_back(channel);
    }
    channel++;
  }
  return channels;
}

void writeADCConversionTime(std::vector<String> DB)
{
  int adcChannel=DB[1].toInt();
  byte cr;

  byte fw = ((byte)((DB[2].toInt()*6.144-249)/128))|128;

  SPI.transfer(adc,0x30+adcChannel);
  SPI.transfer(adc,fw);
  delayMicroseconds(100);
  SPI.transfer(adc,0x70+adcChannel);
  cr=SPI.transfer(adc,0); //Read back the CT register

  int convtime = ((int)(((cr&127)*128+249)/6.144)+0.5);
  Serial.println(convtime);
}



void getSingleReading(int adcchan)
{
  Serial.flush();
  int statusbyte=0;
  byte o2;
  byte o3;
  int ovr;
  if(adcchan <= 3)
  {
    SPI.transfer(adc,0x38+adcchan);   // Indicates comm register to access mode register with channel
    SPI.transfer(adc,0x48);           // Indicates mode register to start single convertion in dump mode
    waitDRDY();                       // Waits until convertion finishes
    SPI.transfer(adc,0x48+adcchan);   // Indcates comm register to read data channel data register
    statusbyte=SPI.transfer(adc,0);   // Reads Channel 'ch' status
    o2=SPI.transfer(adc,0);           // Reads first byte
    o3=SPI.transfer(adc,0);           // Reads second byte
    ovr=statusbyte&1;
    switch (ovr)
    {
      case 0:
      int decimal;
      decimal = twoByteToInt(o2,o3);
      float voltage;
      voltage = map2(decimal, 0, 65536, -10.0, 10.0);
      Serial.println(voltage,4);
      break;
      
      case 1:
      Serial.println(0,4);
      break;   
    }
  }
}
float getSingleReadingWithoutPrint(int adcchan){
    //Yotam
   Serial.flush();
  int statusbyte=0;
  byte o2;
  byte o3;
  int ovr;
  if(adcchan <= 3)
  {
    SPI.transfer(adc,0x38+adcchan);   // Indicates comm register to access mode register with channel
    SPI.transfer(adc,0x48);           // Indicates mode register to start single convertion in dump mode
    waitDRDY();                       // Waits until convertion finishes
    SPI.transfer(adc,0x48+adcchan);   // Indcates comm register to read data channel data register
    statusbyte=SPI.transfer(adc,0);   // Reads Channel 'ch' status
    o2=SPI.transfer(adc,0);           // Reads first byte
    o3=SPI.transfer(adc,0);           // Reads second byte
    ovr=statusbyte&1;
    switch (ovr)
    {
      case 0:
      int decimal;
      decimal = twoByteToInt(o2,o3);
      float voltage;
      voltage = map2(decimal, 0, 65536, -10.0, 10.0);
            return voltage;
      //Serial.println(voltage,4);
      break;
      
      case 1:
            return 0;
      break;
    }
  } 
}

void readADC(byte DB)
{
  int adcChannel=DB;
  switch (adcChannel)
  {
    case 0:
    getSingleReading(1);
    break;
    case 1:
    getSingleReading(3);
    break;
    case 2:
    getSingleReading(0);
    break;
    case 3:
    getSingleReading(2);
    break;

    default:  
    break;
  }
}


float readADCWithoutPrint(byte DB)
{
  int adcChannel=DB;
  switch (adcChannel)
  {
    case 0:
    return getSingleReadingWithoutPrint(1);
    break;
    case 1:
    return getSingleReadingWithoutPrint(3);
    break;
    case 2:
    return getSingleReadingWithoutPrint(0);
    break;
    case 3:
    return getSingleReadingWithoutPrint(2);
    break;

    default:  
    break;
  }
}



void intToTwoByte(int s, byte * DB1, byte * DB2)
{
    *DB1 = ((byte)((s>>8)&0xFF));
    *DB2 = ((byte)(s&0xFF)); 
}


float twoByteToVoltage(byte DB1, byte DB2)
{
  int decimal;
  float voltage;

  decimal = twoByteToInt(DB1,DB2);

  if (decimal <= 32767)
  {
    voltage = decimal*10.0/32767;
  }
  else
  {
    voltage = -(65536-decimal)*10.0/32768;
  }
  return voltage;
}


void voltageToTwoByte(float voltage, byte * DB1, byte * DB2)
{
  int decimal;
  if (voltage > 10 || voltage < -10)
  {
    *DB1 = 128;
    *DB2 = 0;
    error();
  }  
  else if (voltage >= 0)
  {
    decimal = voltage*32767/10;
  }
  else
  {
    decimal = voltage*32768/10 + 65536;
  }
  intToTwoByte(decimal, DB1, DB2);
}

float dacDataSend(int ch, float voltage)
{
  byte b1;
  byte b2;

  voltageToTwoByte(voltage, &b1, &b2);
  
  SPI.transfer(dac,16+ch,SPI_CONTINUE); // Indicates to DAC to write channel 'ch' in the data register 
  SPI.transfer(dac,b1,SPI_CONTINUE);   // writes first byte
  SPI.transfer(dac,b2);                // writes second byte

  return twoByteToVoltage(b1, b2);
}

float writeDAC(int dacChannel, float voltage)
{
  switch(dacChannel)
  {
    case 0:
    return dacDataSend(2,voltage);
    break;

    case 1:
    return dacDataSend(0,voltage);
    break;

    case 2:
    return dacDataSend(3,voltage);
    break;

    case 3:
    return dacDataSend(1,voltage);
    break;

    default:
    break;
  }
}

void dacDataReceive(int ch)
{
  Serial.flush();
  byte o2;
  byte o3;
  
  // Enables DAC-SDO
  SPI.transfer(dac,1,SPI_CONTINUE);
  SPI.transfer(dac,0,SPI_CONTINUE);
  SPI.transfer(dac,0);

  SPI.transfer(dac,144+ch,SPI_CONTINUE);  // Indicates to DAC to read channel 'ch' from the data register
  SPI.transfer(dac,0,SPI_CONTINUE);       // Don't care
  SPI.transfer(dac,0,SPI_LAST);           // Don't care
  SPI.transfer(dac,0,SPI_CONTINUE);
  o2 = SPI.transfer(dac,0,SPI_CONTINUE);  // Reads first byte
  o3 = SPI.transfer(dac,0);               // Reads second byte 

  Serial.write(o2);
  Serial.write(o3);

  //Disables DAC-SDO
  SPI.transfer(dac,1,SPI_CONTINUE);
  SPI.transfer(dac,0,SPI_CONTINUE);
  SPI.transfer(dac,1);
}

void readDAC(std::vector<String> DB)
{
  int dacChannel=DB[1].toInt();
  dacDataReceive(dacChannel);
}


void bufferRamp(std::vector<String> DB)
{
  String channelsDAC = DB[1];
  int NchannelsDAC = channelsDAC.length();
  String channelsADC = DB[2];
  int NchannelsADC = channelsADC.length();
  std::vector<float> vi;
  std::vector<float> vf;
  for(int i = 3; i < NchannelsDAC+3; i++)
  {
    vi.push_back(DB[i].toFloat());
    vf.push_back(DB[i+NchannelsDAC].toFloat());
  }
  int nSteps=(DB[NchannelsDAC*2+3].toInt());
  byte b1;
  byte b2;
 
  for (int j=0; j<nSteps;j++)
  {
    digitalWrite(data,HIGH);
    for(int i = 0; i < NchannelsDAC; i++)
    {
      writeDAC(channelsDAC[i]-'0',vi[i]+(vf[i]-vi[i])*j/(nSteps-1));
    }
    delayMicroseconds(DB[NchannelsDAC*2+4].toInt());
    for(int i = 0; i < NchannelsADC; i++)
    {
      readADC(channelsADC[i]-'0');
    }
    
  }
  digitalWrite(data,LOW);
}

void autoRamp1(std::vector<String> DB)
{
  float v1 = DB[2].toFloat();
  float v2 = DB[3].toFloat();
  int nSteps = DB[4].toInt();
  int dacChannel=DB[1].toInt();

  for (int j=0; j<nSteps;j++)
  {
    int timer = micros();
    digitalWrite(data,HIGH);
    writeDAC(dacChannel, v1+(v2-v1)*j/(nSteps-1));
    digitalWrite(data,LOW);
    while(micros() <= timer + DB[5].toInt());
  }
}

std::vector<String> split_string_by_comma(String s) {
  std::vector<String> v;
  int comma_index =  s.indexOf(',');
  while (comma_index >= 0){
    v.push_back(s.substring(0, comma_index));
    s = s.substring(comma_index+1);
    comma_index =  s.indexOf(',');
  }
  v.push_back(s);
  return v;

}
int wait_time(int steps, double freq){
  return  (1/(steps*freq))*MICROSECONDS_IN_SECOND;
}
double round_freq(int steps, int waiting_time){
  return (1.0*MICROSECONDS_IN_SECOND)/steps / waiting_time;
}
double voltage_step(double voltage_per_second, int steps, double freq){
    return voltage_per_second / steps / freq;
}
double radian_per_step(int steps){
  return 2*3.1415926 / steps;
}

void sine(double frequency, int steps, double voltage_per_second){
    //Yotam
    //Runs sine function with initial amplitude and DC current (mid).
    //Because of integer rounding errors, prints the real frequency.
    double mid[] = {0,0,0,0};
    double amp[] = {0,0,0,0};
    //Optimization
    double prev_value[] = {0,0,0,0};
    double voltage_per_step = voltage_step(voltage_per_second, steps, frequency);
    int waiting_time = wait_time(steps, frequency);
    double real_freq = round_freq(steps, waiting_time);
    double single_step_rad = radian_per_step(steps);

    Serial.print("Sine with read is running real freq : ");
    Serial.println(real_freq);
    double current_radian = 0;
    int timer;
    double sine_value, value_to_reference;

    //Steps to new DC
    double target_dc[] = {0,0,0,0};
    
    //Online updates
    String update, command;
    char byte;
    int port;

    //Yotam Debug Pin
    digitalWrite(14, LOW);

    
    
  while (1){
       if (Serial.available()){
          update = "";
          command = "";
          do {
              byte = Serial.read();
              
              if (byte == '\xff'){
                  ; //Ignore
              } else if (byte == ' ') {
                  command = update;
                  update = "";
              } else {
                  update.concat(byte);
              }
              
          } while (byte != '\r');
          
          while (Serial.available()){
              Serial.read();
          }
          
          command.trim();
          update.trim();
          /*
            Command syntax is:
              COMMAND value:port

            for example :
              DC -3.4:2

            sets the DC value of port 2 to -3.4
          */
          update = update.substring(0, update.indexOf(':'));
          if ((command == "DC") || (command == "AC")){
              int port =  update.substring(update.indexOf(':')+1).toInt();

          if (command == "DC") {
            if (abs(target_dc[port])+ abs(amp[port]) <= 10){
              target_dc[port] = update.toFloat();
            }

          } else if (command == "AC"){
            if (abs(update.toFloat()) + abs(mid[port]) <= 10){
                amp[port] = update.toFloat();
            }
          }
          } else if (update.startsWith("SINE,")){
            std::vector<String> v;
            v = split_string_by_comma(update.substring(5));

            if ((v.size() == 3)){
              frequency = v[0].toFloat();
              steps = v[1].toInt();
              voltage_per_second = v[2].toFloat();
              voltage_per_step = voltage_step(voltage_per_second, steps, frequency);
              waiting_time = wait_time(steps, frequency);
              real_freq = round_freq(steps, waiting_time);
              single_step_rad = radian_per_step(steps);

              Serial.print("Sine with read is running real freq : ");
              Serial.println(real_freq);
            } 

          }
        
        }
      
      
      
      //Applying sine Current
      timer = micros();
      sine_value =  sin(current_radian);

      //Ramping
      double curr_value;
      for (int i = 0; i<NUMBER_OF_PORTS; i++){

        if (abs(mid[i] - target_dc[i]) > voltage_per_step){
          if (mid[i] < target_dc[i]){
            mid[i] += voltage_per_step;
          } else {
            mid[i] -= voltage_per_step;
          }
        } else {
          mid[i] = target_dc[i];
        }
        curr_value = sine_value*amp[i] + mid[i]; 
        if (curr_value != prev_value[i]){
          writeDAC(i, curr_value);
          prev_value[i] = curr_value;
        }
      }

      //Write Reference sine
      //writeDAC(3, sine_value*ref_amp + mid); 
      //writeDAC(3, sine_value*ref_amp); 

      
      current_radian += single_step_rad;
      while(micros() <= timer + waiting_time);
  }
}


void sine_buffer(int dac_channel, int adc_channel, float mid, float amp, float frequency,
                 int steps, int iterations){
    //Yotam
    /*
     Just like Sine, but running n times and reading along.

     
     */
    int waiting_time = (1/(steps*frequency))*MICROSECONDS_IN_SECOND;
    double real_freq =(1.0*MICROSECONDS_IN_SECOND)/steps / waiting_time;
    double single_step_rad = 2*3.1415926 / steps;
    double current_radian = 0;
    int timer;
    double value_to_write;
    float data[steps*iterations];
    float reference_data[steps*iterations];
    float orthogonal_reference_data[steps*iterations];
    //Run DEFAULT times without recording
     for (int i = 0; i < PRE_ITERATIONS * steps; i++){
      timer = micros();
      value_to_write =  sin(current_radian)*amp + mid;
      writeDAC(dac_channel, value_to_write);
      current_radian += single_step_rad;
      while(micros() <= timer + waiting_time);
  }
    current_radian = 0;
    for (int i = 0; i < iterations* steps; i++){
      timer = micros();
      value_to_write =  sin(current_radian)*amp + mid;
      writeDAC(dac_channel, value_to_write);
        current_radian += single_step_rad;
        reference_data[i] = value_to_write;
        orthogonal_reference_data[i] = sin(current_radian + PI/2)*amp + mid;
        data[i]=readADCWithoutPrint(adc_channel);
      while(micros() <= timer + waiting_time);
  }
    Serial.println("BEGIN_PRINT_DATA");
    for(int i = 0; i< steps*iterations; ++i){
        Serial.println(data[i],4);
    }
    Serial.println("BEGIN_PRINT_REF");
    for(int i = 0; i< steps*iterations; ++i){
        Serial.println(reference_data[i],4);
    }
    Serial.println("BEGIN_ORTHOGONAL_REF_DATA");
    for(int i = 0; i< steps*iterations; ++i){
        Serial.println(orthogonal_reference_data[i],4);
    }
    
    Serial.println("BUFFER_SINE_FINISHED");
}
void autoRamp2(std::vector<String> DB)
{
  float vi1 = DB[3].toFloat();
  float vi2 = DB[4].toFloat();
  float vf1 = DB[5].toFloat();
  float vf2 = DB[6].toFloat();
  int nSteps = DB[7].toInt();
  byte b1;
  byte b2;
  int dacChannel1=DB[1].toInt();
  int dacChannel2=DB[2].toInt();

  for (int j=0; j<nSteps;j++)
  {
    int timer = micros();
    digitalWrite(data,HIGH);
    writeDAC(dacChannel1, vi1+(vf1-vi1)*j/(nSteps-1));
    writeDAC(dacChannel2, vi2+(vf2-vi2)*j/(nSteps-1));
    while(micros() <= timer + DB[8].toInt());
    digitalWrite(data,LOW);
  }
}

//void autoRamp(std::vector<byte> DB)
//{
//  int v1=twoByteToVoltage(DB[1],DB[2]);
//  int v2=twoByteToVoltage(DB[3],DB[4]);
//  int nSteps=(DB[5]);
//  byte b1;
//  byte b2;
//  int dacChannel=(DB[0])&7;
//
//  for (int j=0; j<nSteps;j++)
//  {
//    digitalWrite(led,LOW);
//    voltageToTwoByte(v1+(v2-v1)*j/(nSteps-1), &b1, &b2);
//    dacDataSend(dacChannel,b1,b2);
//    delayMicroseconds(50);
//    readADC(DB[0]);
//    digitalWrite(led,HIGH);
//  }
//  digitalWrite(led,LOW);
//}





void router(std::vector<String> DB)
{
  //Input : String of arguments
  //Output : 
  int operation = indexOfOperation(DB[0]);
  switch ( operation )
  {
    case 0:
    Serial.println("NOP");
    break;

    case 1: // Write DAC **IMPLEMENTED, WORKS
    if(DB[2].toFloat() < -10 || DB[2].toFloat() > 10)
    {
      Serial.println("VOLTAGE_OVERRANGE");
      break;
    }
    float v;
    v = writeDAC(DB[1].toInt(),DB[2].toFloat());
    Serial.print("DAC ");
    Serial.print(DB[1]);
    Serial.print(" UPDATED TO ");
    Serial.print(v,4);
    Serial.println("V");
    break;
    
    case 2: // Read ADC 
    readADC(DB[1].toInt());
    break;

//    case 3: // not working with current shield
//    readDAC(DB);
//    break;

    case 3:
    autoRamp1(DB);
    Serial.println("RAMP_FINISHED");
    break;

    case 4:
    autoRamp2(DB);
    Serial.println("RAMP_FINISHED");
    break;
    
    case 5: // Autoramp
    bufferRamp(DB);
    Serial.println("BUFFER_RAMP_FINISHED");
    break;

    case 6:
    resetADC();
    break;
    
    case 7:
    talkADC(DB);
    break;
    
    case 8: // Write conversion time registers
    writeADCConversionTime(DB);
    break;

    case 9: // ID
    ID();
    break;

    case 10:
    RDY();
    break;
          
      case 11:
          //Two parameters are not required
          sine(DB[1].toFloat(), DB[2].toFloat(), DB[3].toFloat());
          break;

    default:
    break;
  }
}

void loop() 
{
  Serial.flush();
  String inByte = "";
  std::vector<String> comm;
  if(Serial.available())
  {
    char received;
    while (received != '\r')
    {
      if(Serial.available())
      {
        received = Serial.read();
          if (received == '\n' || received == ' ')
          {}
          else if (received == ',' || received == '\r')
          {
            comm.push_back(inByte);
            inByte = "";
          }
          else
          {
            inByte += received;
        }        
      }
    }
  router(comm);
  }
}


