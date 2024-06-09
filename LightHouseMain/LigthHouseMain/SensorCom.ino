#define SCOM_BUFF_LEN 20


byte SensorComState;
char SensorComBuff[SCOM_BUFF_LEN];
short int SensorComBuffIndx =0;
short int i;
boolean SensorComFlag;

boolean SensorComGetFlag(void)
{
  return SensorComFlag;
}

void SensorComInitialize(void)
{ 
  SensorComState = 0;
  SensorComBuffIndx =0;
  for (i=0;i<SCOM_BUFF_LEN;i++){
     SensorComBuff[i]= 0;
  }
  SensorComFlag = false;
}

void SensorComSendStr(char *b){
  //delay(1000);
  //Serial.println(b);
  Serial.print('<');
  Serial.print(b);
  Serial.println(">");
 
}


void SensorComDebug(void){
 
 if (Serial.available()>0){
     char c;
     c = Serial.read();
     Serial.print(c);
  }
}



void SensorComRequestReading(){
    if (++meas.measure_indx > 4) meas.measure_indx=0;
    switch (meas.measure_indx){
        case 0: SensorComSendStr("?TLake"); break;
        case 1: SensorComSendStr("?BMP180T"); break;
        case 2: SensorComSendStr("?DHT22T"); break;
        case 3: SensorComSendStr("?LDR1"); break;
        case 4: AllMeasDone = true; break;
        default:  meas.measure_indx=0; break;
    }
}

void SensorComMonitor(void) {
  char c;
  int buff_len;
  // SensorCom.print("X");
  // SensorCom.listen();
  
  if (Serial.available()>0){
     c = Serial.read();
     //Serial.print(c);
     switch(SensorComState){
     case 0:
       if (c=='<') {
         SensorComState = 1;
         //Serial.print("State=1");
       }
       break;
     case 1:
       if (c=='>') {
         SensorComState = 2;
         //Serial.print("State=2");
       } else {
         SensorComBuff[SensorComBuffIndx] = c;
         SensorComBuffIndx++;
         //Serial.print(SensorComBuffIndx);
         if (SensorComBuffIndx >= SCOM_BUFF_LEN) {
            SensorComInitialize;
         }
       }     
       break;
     case 2:
       if (c==CR) {
          SensorComState = 3;
         //Serial.print("State=3");

       }
       else {
          SensorComInitialize();
       }
       break;
     case 3:
        // <*Lake=nn.c>  <*TLake=17.9>
        if (c==LF) { 
            // Serial.print("Message: ");
            // Serial.print(SensorComBuff); 
             
            for (i=0; SensorComBuff[i] != 0; i++);
            buff_len = i;
            //Serial.println(buff_len);
            
            if(SensorComChkMsg(SensorComBuff, "*TLake=") == 7) 
            {
              meas.water_temp.value = atof(SensorComBuff+7);
              meas.water_temp.available = true;
                // Serial.print(" >");Serial.print(SensorComBuff+7);Serial.print(" -- ");Serial.println(meas.water_temperature);            
            }
            if(SensorComChkMsg(SensorComBuff, "*T_BMP180=") == 10)  
            {
              meas.bmp180_temp.value = atof(SensorComBuff+10);
              meas.bmp180_temp.available = true;
            }
            if(SensorComChkMsg(SensorComBuff, "*T_DHT22=") == 9)  
            {
              meas.dht22_temp.value = atof(SensorComBuff+9);
              meas.dht22_temp.available = true;
            }
            if(SensorComChkMsg(SensorComBuff, "*LDR_1=") == 7)  
            {
              meas.ldr_1.value = atof(SensorComBuff+7); 
              meas.ldr_1.available = true;
            }
           
       }    
       SensorComInitialize();
       break;
     } 
  }    
}

int SensorComChkMsg( char *a, char *b){
  int i;
  boolean done;
  i= 0; done = false; 
  
  while (!done){
      if ((a[i]==0) || (b[i]==0) || (a[i]!=b[i]))  {
         done = true;
         return(i);        
      }
      else i++;
  }
}
