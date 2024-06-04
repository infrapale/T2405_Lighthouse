//<#IO41x>, x=0|1
#include <SoftwareSerial.h>
#define rxPin 4
#define txPin 3
#define SCOM_BUFF_LEN 20
#define UNIT_ADDR_1 'I'
#define UNIT_ADDR_2 'O'
#define UNIT_ADDR_3 '1'





// SoftwareSerial softCom(rxPin, txPin); // RX, TX

byte softComState;
char softComBuff[SCOM_BUFF_LEN];
short int softComBuffIndx =0;
short int i;
boolean softComFlag;

boolean SoftComFlag(void){
  return softComFlag;
}

void InitSoftCom (void){
  
  // pinMode(rxPin, INPUT);
  // pinMode(txPin, OUTPUT);
  // softCom.begin(9600);
  softComState = 0;
  softComBuffIndx =0;
  for (i=0;i<SCOM_BUFF_LEN;i++){
     softComBuff[i]= 0;
  }
  // softCom.listen();
    softComFlag = false;
  
}

void SendSoftCom(char *b){
  //delay(1000);
  //Serial.println(b);
  Serial.print('<');
  Serial.print(b);
  Serial.println(">");
 
}


 void DebugSoftCom(void){
 
 if (Serial.available()>0){
     char c;
     c = Serial.read();
     Serial.print(c);
  }
 }
void SoftComMonitor(void) {
  char c;
  int buff_len;
  // softCom.print("X");
  // softCom.listen();
  
  if (Serial.available()>0){
     c = Serial.read();
     //Serial.print(c);
     switch(softComState){
     case 0:
       if (c=='<') {
         softComState = 1;
         //Serial.print("State=1");
       }
       break;
     case 1:
       if (c=='>') {
         softComState = 2;
         //Serial.print("State=2");
       } else {
         softComBuff[softComBuffIndx] = c;
         softComBuffIndx++;
         //Serial.print(softComBuffIndx);
         if (softComBuffIndx >= SCOM_BUFF_LEN) {
            InitSoftCom;
         }
       }     
       break;
     case 2:
       if (c==CR) {
          softComState = 3;
         //Serial.print("State=3");

       }
       else {
          InitSoftCom();
       }
       break;
     case 3:
        // <*Lake=nn.c>  <*TLake=17.9>
        if (c==LF) { 
            // Serial.print("Message: ");
            // Serial.print(softComBuff); 
             
            for (i=0; softComBuff[i] != 0; i++);
            buff_len = i;
            //Serial.println(buff_len);
            
            if(chkMsg(softComBuff, "*TLake=") == 7) {
                meas.water_temperature = atof(softComBuff+7);
                // Serial.print(" >");Serial.print(softComBuff+7);Serial.print(" -- ");Serial.println(meas.water_temperature);            
            }
           if(chkMsg(softComBuff, "*T_BMP180=") == 10)  meas.bmp180_temp = atof(softComBuff+10);
           if(chkMsg(softComBuff, "*P_BMP180=") == 10)  meas.bmp180_pres = atof(softComBuff+10);
           if(chkMsg(softComBuff, "*T_DHT22=") == 9)  meas.dht22_temp = atof(softComBuff+9);
           if(chkMsg(softComBuff, "*H_DHT22=") == 9)  meas.dht22_hum = atof(softComBuff+9);
           if(chkMsg(softComBuff, "*LDR_1=") == 7)  meas.ldr_1 = atof(softComBuff+7); 
           
            //if (softComBuff[0] =='#'){
             
            //if ((softComBuff[1] == UNIT_ADDR_1 ) && (softComBuff[2] == UNIT_ADDR_2 ) && 
            //     (softComBuff[3] == UNIT_ADDR_3 ) && (softComBuff[4] == 'I' )) {
            //    if ((softComBuff[5] >='1') && (softComBuff[5] <='4'))  {
            //       if ((softComBuff[6] =='0') || (softComBuff[6] =='1'))  {
            //          i = softComBuff[5]-'1' ; 
            //          //if ( softComBuff[6] =='1') inp[i]=HIGH; else inp[i]=LOW; 
            //          softComFlag = true;
            //          
            //       }
            //    }   
            // }
          
          //}
       
       }    
       InitSoftCom();
       break;
     } 
  }    
}

int chkMsg( char *a, char *b){
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
