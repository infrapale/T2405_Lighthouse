// T169  Lake Temperature using SMT16030
// 2017-09-16 
// Reply to "<?>cr lf"
// Rply = <*Lake=nn.c> cr lf 

#include <SMT16030.h>

SMT16030 temp_01(7);
float t01;
byte rdState;

void setup()
{
  delay(2000);
  Serial.begin(9600);  // start serial for output
  temp_01.begin(20.0);
  Serial.println("T169");
  Serial.println("<?TLake>");
  rdState= 0;
}

void loop()
{
  t01 = temp_01.rd_temp();
  if (RdMsg()) {
     Serial.print("<*TLake=");
     Serial.print(t01,1);
     Serial.println(">");
  }
}  

boolean RdMsg(void){
  char c;

  if (Serial.available()>0 )  {
     c = Serial.read();
     switch(rdState){
     case 0: if ( c == '<') rdState++; break;
     case 1: if ( c == '?') rdState++; else  rdState=0; break;
     case 2: if ( c == 'T') rdState++; else  rdState=0; break;
     case 3: if ( c == 'L') rdState++; else  rdState=0; break;
     case 4: if ( c == 'a') rdState++; else  rdState=0; break;
     case 5: if ( c == 'k') rdState++; else  rdState=0; break;
     case 6: if ( c == 'e') rdState++; else  rdState=0; break;
     case 7: if ( c == '>') rdState++; else  rdState=0; break;
     case 8: if ( c == '\r') rdState++; else  rdState=0; break;
     case 9: if ( c == '\n') rdState++; else  rdState=0; break;
     }
  }
  if (rdState > 9 ){
     rdState = 0;
     return (true);
  }
  else return (false);
     
}
