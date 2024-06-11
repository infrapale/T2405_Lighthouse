
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
#include <avr_watchdog.h>
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
AVR_Watchdog watchdog(4);
// #define doggieTickle() resetTime = millis(); 

typedef struct
{
  float value;
  bool  available;
} reading_st;

typedef struct  {
  reading_st water_temp;
  reading_st bmp180_temp;
  reading_st dht22_temp;
  reading_st ldr_1;
  byte measure_indx;
} measure_data_struct_st;

measure_data_struct_st meas =
{
  .water_temp   = {0.0, false},
  .bmp180_temp  = {0.0, false},
  .dht22_temp   = {0.0, false},
  .ldr_1        = {0.0, false}
};

int16_t packetnum = 0;  // packet counter, we increment per xmission
byte rad_turn = 0;
//byte read_turn = 0;
uint8_t radio_buff[RH_RF69_MAX_MESSAGE_LEN];
byte tx_buff_ptr;

boolean msgReady; 
SmartLoop Smart = SmartLoop(1);
SimpleTimer timer;

void setup() {
    cli();
    Wire.begin();
    delay(4000);
    watchdog.set_timeout(4);
    sei();
    Smart.begin(9600);
    //while (!Serial); // wait until serial console is open, remove if not tethered to computer
    Serial.println("T2405 LightHouseMain");
    Serial.println(__DATE__); Serial.println(__TIME__);
    //watchdogSetup();
    meas.measure_indx = 0;
    SensorComInitialize();
 
    InitRfm69();
    setup_relays();
    //test_relays();
    // Initialize radio
    timer.setInterval(10, run_10ms);
    timer.setInterval(1000, run_1000ms);
    timer.setInterval(10000, run_10s);
    ///timer.setInterval(600000, run_10_minute);    
    timer.setInterval(600000, run_10_minute);   
    InitRadioReceive();

    // Serial.println("Jam"); while(true) ;


}

void loop() {
    byte i;

    SensorComMonitor();
    timer.run(); 
    watchdog.clear();
    //check if something was received (could be an interrupt from the radio)
    ReadRadioMsg();

}

int ConvertSensorToJson(char *zone, char *sensor, float value, char *remark ){
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
       // Serial.println(JsonString);
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
  //  if (++MyTime.second > 59 ){
  //     MyTime.second = 0;
  //     if (++MyTime.minute > 59 ){
  //        MyTime.minute = 0;
  //        if (++MyTime.hour > 23){
  //           MyTime.hour = 0;
  //        }
  //     }   
  //  } 
}


void run_10s(void){ 
  SensorComRequestReading();
}

void run_10_minute(void){
  //Serial.print("rad_turn = "); Serial.println(rad_turn);
  if (++rad_turn > 3) rad_turn = 0;
  switch(rad_turn)
  {
      case 0:
          if (meas.water_temp.available && (ConvertSensorToJson(ZONE,"T_Water",meas.water_temp.value,"") > 0) ) 
            radiate_msg(radio_buff);
          break;
      case 1:
          if (meas.bmp180_temp.available && (ConvertSensorToJson(ZONE,"T_bmp180",meas.bmp180_temp.value,"") > 0)) 
            radiate_msg(radio_buff);
          break;
      case 2:
          if (meas.dht22_temp.available && (ConvertSensorToJson(ZONE,"T_dht22",meas.dht22_temp.value,"") > 0 )) 
            radiate_msg(radio_buff);
          break;
      case 3:
          if (meas.ldr_1.available && (ConvertSensorToJson(ZONE,"ldr1",meas.ldr_1.value,"") > 0 )) 
            radiate_msg(radio_buff);
          break;
      default:
          rad_turn = 0;
          break;
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
