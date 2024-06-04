// T170  Lighthouse remote sensor module
// DHT22 Temperature, humidity
// BMP 180 Pressure and temperature
// LDR Light 
// 2018-06-03 
// Reply to "<?T1>cr lf"
// Rply = <*Lake=nn.c> cr lf 
#define DHTTYPE           DHT22     // DHT 22 (AM2302)
#define DHTPIN            8        // Pin which is connected to the DHT sensor.
#define LDR_PIN           A0
#include <SFE_BMP180.h>
#include <Wire.h>
#include <DHT_U.h>
#include <SimpleTimer.h> 

#define ALTITUDE 120.0 
// Watchdog
unsigned long resetTime = 0;
#define doggieTickle() resetTime = millis(); 

SFE_BMP180 pressure;
SimpleTimer timer;
DHT_Unified dht(DHTPIN, DHTTYPE);
sensors_event_t Sensor1; 

float temp_dht22;
float hum_dht22;
double pres_bmp180;
double temp_bmp180;
float light_1;
boolean bmp180_ok;
byte rd_sensor_indx;
byte scheduled_sensor = 1;
unsigned int rdState; 

void setup()
{
  delay(4000);
  Serial.begin(9600);  // start serial for output
  Serial.println("T170 outdoor sensors");
  watchdogSetup();
  rdState= 0;
  if (pressure.begin()){
    Serial.println("BMP180 init success");
    bmp180_ok = true;
  }  
  else  {
    // Oops, something went wrong, this is usually a connection problem,
    // see the comments at the top of this sketch for the proper connections.
    Serial.println("BMP180 init fail\n\n");
    bmp180_ok = false;
    //while(1); // Pause forever.
  }
  if ( bmp180_ok) {
     Serial.print("provided altitude: ");
     Serial.print(ALTITUDE,0);
     Serial.print(" meters, ");
  }
  dht.begin();

  timer.setInterval(1000, run_1s);
  timer.setInterval(10000, run_10s);
  rd_sensor_indx = 0;
}

void loop()
{
  byte sensor_indx;
  //t01 = temp_01.rd_temp();
  sensor_indx = RdMsg();
  switch (sensor_indx) {
     case 0: break;
     case 1:
       Serial.print("<*T_BMP180=");
       Serial.print(temp_bmp180,1);
       Serial.println(">");
       break;
     case 2:
       Serial.print("<*P_BMP180=");
       Serial.print(pres_bmp180,0);
       Serial.println(">");
       break;
     case 3:
       Serial.print("<*T_DHT22=");
       Serial.print(temp_dht22,1);
       Serial.println(">");
       break;      
      case 4:
       Serial.print("<*H_DHT22=");
       Serial.print(hum_dht22,1);
       Serial.println(">");
       break;      
      case 5:
       Serial.print("<*LDR_1=");
       Serial.print(light_1,3);
       Serial.println(">");
       break;   
  }
  timer.run();
}  

void run_10s(void){
   sensor_scheduler(); 
}
void run_1s(void){
   doggieTickle();
}


byte RdMsg(void){
  char c;

  if (Serial.available()>0 )  {
     c = Serial.read();
     switch(rdState){
     case 0: if ( c == '<') rdState++; rd_sensor_indx=0; break;
     case 1: if ( c == '?') rdState++; else  rdState=0; break;
     case 2: 
       switch  ( c ) {
          case 'B': rdState++; break;
          case 'D': rdState=10; break;
          case 'L': rdState=20; break;
          default: rdState= 0;break;
        } 
        break;
     case 3: if ( c == 'M') rdState++; else  rdState=0; break;
     case 4: if ( c == 'P') rdState++; else  rdState=0; break;
     case 5: if ( c == '1') rdState++; else  rdState=0; break;
     case 6: if ( c == '8') rdState++; else  rdState=0; break;
     case 7: if ( c == '0') rdState++; else  rdState=0; break;
     case 8: 
        switch  ( c ) {
           case 'T': rdState=100; rd_sensor_indx=1; break;
           case 'P': rdState=100; rd_sensor_indx=2; break;
           default: rdState = 0; break;
        }
        break;
     case 10: if ( c == 'H') rdState++; else  rdState=0; break;
     case 11: if ( c == 'T') rdState++; else  rdState=0; break;
     case 12: if ( c == '2') rdState++; else  rdState=0; break;
     case 13: if ( c == '2') rdState++; else  rdState=0; break;
     case 14: 
        switch  ( c ) {
           case 'T': rdState=100; rd_sensor_indx=3; break;
           case 'H': rdState=100; rd_sensor_indx=4; break;
           default: rdState = 0; break;
        }
        break;
     case 20: if ( c == 'D') rdState++; else  rdState=0; break;
     case 21: if ( c == 'R') rdState++; else  rdState=0; break;
     case 22: if ( c == '1') { 
          rdState=100; 
          rd_sensor_indx=5; 
       } 
       else {  
         rdState=0; 
       }  
       break;
     
     case 100: if ( c == '>') rdState++; else  rdState=0; break;
     case 101: if ( c == '\r') rdState++; else  rdState=0; break;
     case 102: if ( c == '\n') rdState=999; else  rdState=0; break;
     } 
     //Serial.print(c); Serial.print("-");  Serial.println(rdState);
  }
  
  if (rdState == 999 ){
     rdState = 0;
     return (rd_sensor_indx);
  }
  else return (0);
     
}

void sensor_scheduler(void){
   //Serial.print(scheduled_sensor);
   switch(scheduled_sensor){
     case 1: rd_bmp180_temperature(); break;
     case 2: rd_bmp180_pressure(); break;
     case 3: rd_dht22_temperature();break;
     case 4: rd_dht22_humidity(); break;
     case 5: light_1 = float(analogRead(LDR_PIN))/1024; break;
   
   }
   if (++scheduled_sensor > 5) scheduled_sensor = 1;

}




