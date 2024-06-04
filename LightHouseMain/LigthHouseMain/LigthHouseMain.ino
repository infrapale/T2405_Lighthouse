
// Define message groups to be supported (Astrid.h)
#define NODE_ADDR MH2_RELAY
#define MENU_DATA
#define MH1_RELAY   //MH1_RELAY, MH2_RELAY, TK_RELAY   defined in Astrid.8
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <RH_RF69.h>
#include <rfm69_support.h>
//#include <AstridAddrSpace.h>
#include <Astrid.h>
#include <VillaAstridCommon.h>
#include <SimpleTimer.h> 
#include <SmartLoop.h>
#include <DHT_U.h>
//#include <AM2320.h>

//*********************************************************************************************
// *********** IMPORTANT SETTINGS - YOU MUST CHANGE/ONFIGURE TO FIT YOUR HARDWARE *************
//*********************************************************************************************

#define ZONE  "Dock"
#define SERIAL_BAUD   9600
#define LED           13  // onboard blinky

#define DHTPIN 6
#define DHTTYPE DHT22  
// Watchdog
unsigned long resetTime = 0;
#define doggieTickle() resetTime = millis(); 

struct measure_data_struct {
  float water_temperature;
  float bmp180_temp;
  float bmp180_pres;
  float dht22_temp;
  float dht22_hum;
  float ldr_1;
  byte measure_indx;
};

unit_type_entry Me ={"DOCK1","Terminal","T2405","T2505","T2405","18v06",'0'}; //len = 9,5,5,5,9,byte
time_type MyTime = {2017, 1,30,12,05,30}; 
int16_t packetnum = 0;  // packet counter, we increment per xmission
byte rad_turn = 0;
byte read_turn = 0;
uint8_t radio_buff[RH_RF69_MAX_MESSAGE_LEN];
byte tx_buff_ptr;
measure_data_struct meas;

boolean msgReady; 
boolean SerialFlag;
boolean TxDataReady;
boolean AllMeasDone = false;
DHT_Unified dht(DHTPIN, DHTTYPE);
sensors_event_t Sensor1; 
SmartLoop Smart = SmartLoop(1);
SimpleTimer timer;

void setup() {
    Wire.begin();
    delay(4000);
    Serial.begin(9600);
    //while (!Serial); // wait until serial console is open, remove if not tethered to computer
    Serial.println("T2405 LightHouseMain");
    watchdogSetup();

    Smart.begin(9600);
    //Serial.begin(SERIAL_BAUD);
    InitSoftCom();
    dht.begin();
 
    InitRfm69();
    setup_relays();
    // Initialize radio
    timer.setInterval(10, run_10ms);
    timer.setInterval(1000, run_1000ms);
    timer.setInterval(10000, run_10s);
    ///timer.setInterval(600000, run_10_minute);    
    timer.setInterval(6000, run_10_minute);   
    TxDataReady = false;
    InitRadioReceive();

}

void loop() {
    byte i;

    SoftComMonitor();
    timer.run(); 
   
    //check if something was received (could be an interrupt from the radio)
    ReadRadioMsg();

}

int ConvertFloatSensorToJsonRadioPacket(char *zone, char *sensor, float value, char *remark ){
    byte i;
    unsigned int json_len;
    //Serial.println("ConvertFloatSensorToJson");
    String JsonString; 
    JsonString = "{\"Z\":\"";
    JsonString += zone;
    JsonString += "\",";
    JsonString += "\"S\":\"";
    JsonString += sensor;  
    JsonString += "\",";
    JsonString += "\"V\":";
    JsonString += value;
    JsonString += ",";
    JsonString += "\"R\":\"";
    JsonString += remark;
    JsonString += "\"}";
    
    //Serial.println(JsonString);
    json_len = JsonString.length();
    if (json_len <= RH_RF69_MAX_MESSAGE_LEN){
       for (i=0; i<RH_RF69_MAX_MESSAGE_LEN; i++)radio_buff[i]=0;
       JsonString.toCharArray(radio_buff,RH_RF69_MAX_MESSAGE_LEN);
       // Serial.println(json_len);
       return( json_len );
    }
    else {
      Serial.print("JSON string was too long for the radio packet: "); 
      Serial.println(json_len);
      return(0);
    }
}


void run_10ms(void){
   Smart.HeartBeat10ms();
   if( Smart.Monitor()) msgReady = true;
   relay_do_every_10ms();
}
void run_1000ms(void){
   doggieTickle(); 
   if (++MyTime.second > 59 ){
      MyTime.second = 0;
      if (++MyTime.minute > 59 ){
         MyTime.minute = 0;
         if (++MyTime.hour > 23){
            MyTime.hour = 0;
         }
      }   
   } 
}

void ReadSensors(){
   int i;
   
   if (++meas.measure_indx > 7) meas.measure_indx=1;
      switch (meas.measure_indx){
         case 1: SendSoftCom("?TLake"); break;
         case 2: SendSoftCom("?BMP180T"); break;
         case 3: SendSoftCom("?BMP180P"); break;
         case 4: SendSoftCom("?DHT22T"); break;
         case 5: SendSoftCom("?DHT22H"); break;
         case 6: SendSoftCom("?LDR1"); break;
         case 7: AllMeasDone = true; break;
       }
}

void run_10s(void){ 
   ReadSensors();
}

void run_10_minute(void){
   if (++rad_turn > 6) rad_turn = 1;
   if (AllMeasDone) {
      switch(rad_turn){
          case 1:
              if (ConvertFloatSensorToJsonRadioPacket(ZONE,"T_Water",meas.water_temperature,"") > 0 ) radiate_msg(radio_buff);
              break;
          case 2:
              if (ConvertFloatSensorToJsonRadioPacket(ZONE,"T_bmp180",meas.bmp180_temp,"") > 0 ) radiate_msg(radio_buff);
              break;
          case 3:
              if (ConvertFloatSensorToJsonRadioPacket(ZONE,"P_bmp180",meas.bmp180_pres,"") > 0 ) radiate_msg(radio_buff);
              break;
          case 4:
              if (ConvertFloatSensorToJsonRadioPacket(ZONE,"T_dht22",meas.dht22_temp,"") > 0 ) radiate_msg(radio_buff);
              break;
          case 5:
              if (ConvertFloatSensorToJsonRadioPacket(ZONE,"H_dht22",meas.dht22_hum,"") > 0 ) radiate_msg(radio_buff);
              break;
          case 6:
              if (ConvertFloatSensorToJsonRadioPacket(ZONE,"ldr1",meas.ldr_1,"") > 0 ) radiate_msg(radio_buff);
              break;
      }
   }
}


char * floatToString(char * outstr, double val, byte precision, byte widthp){
 char temp[16];
 byte i;

 // compute the rounding factor and fractional multiplier
 double roundingFactor = 0.5;
 unsigned long mult = 1;
 for (i = 0; i < precision; i++)
 {
   roundingFactor /= 10.0;
   mult *= 10;
 }
 
 temp[0]='\0';
 outstr[0]='\0';

 if(val < 0.0){
   strcpy(outstr,"-\0");
   val = -val;
 }

 val += roundingFactor;

 strcat(outstr, itoa(int(val),temp,10));  //prints the int part
 if( precision > 0) {
   strcat(outstr, ".\0"); // print the decimal point
   unsigned long frac;
   unsigned long mult = 1;
   byte padding = precision -1;
   while(precision--)
     mult *=10;

   if(val >= 0)
     frac = (val - int(val)) * mult;
   else
     frac = (int(val)- val ) * mult;
   unsigned long frac1 = frac;

   while(frac1 /= 10)
     padding--;

   while(padding--)
     strcat(outstr,"0\0");

   strcat(outstr,itoa(frac,temp,10));
 }

 // generate space padding 
 if ((widthp != 0)&&(widthp >= strlen(outstr))){
   byte J=0;
   J = widthp - strlen(outstr);
   
   for (i=0; i< J; i++) {
     temp[i] = ' ';
   }

   temp[i++] = '\0';
   strcat(temp,outstr);
   strcpy(outstr,temp);
 }
 
 return outstr;
}
