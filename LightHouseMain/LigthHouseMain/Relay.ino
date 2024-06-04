// T151 2xBiRelay
#define RELAY_1A A3
#define RELAY_1B A2
#define RELAY_2A A1
#define RELAY_2B A0

#define MAX_RELAY  2 
#define SWITCH_TIME_x10ms 5

struct relay_status_struct {
   boolean is_on;
   byte cntr_10ms;
};

relay_status_struct relay_status[MAX_RELAY]; 



byte relay_off_on[MAX_RELAY][2]={
    {RELAY_1A,RELAY_1B},
    {RELAY_2A,RELAY_2B},      
};

void setup_relays() {
   byte i;
   for (i=0;i < MAX_RELAY;i++){
      pinMode( relay_off_on[i][0], OUTPUT); 
      pinMode( relay_off_on[i][1], OUTPUT); 
      digitalWrite(relay_off_on[i][0],LOW);
      digitalWrite(relay_off_on[i][1],LOW);

      relay_status[i].is_on = false; 
      relay_status[i].cntr_10ms = 0; 

   }
}

void  toggle_relay(byte relay_indx) {
      if (relay_status[relay_indx].is_on) { 
         relay_status[relay_indx].is_on = false;
      } else { 
         relay_status[relay_indx].is_on = true; 
      }   
      //Serial.print("Relay: ");Serial.print(relay_indx);Serial.print(" = ");Serial.println(relay_status[relay_indx].is_on);
      relay_status[relay_indx].cntr_10ms = SWITCH_TIME_x10ms; 
};

void turn_on_relay(byte relay_indx) {
      relay_status[relay_indx].is_on = true; 
      relay_status[relay_indx].cntr_10ms = SWITCH_TIME_x10ms; 
};
void turn_off_relay(byte relay_indx) {
      relay_status[relay_indx].is_on = false; 
      relay_status[relay_indx].cntr_10ms = SWITCH_TIME_x10ms; 
};

void relay_do_every_10ms(void){
   byte i;

   for(i=0;i<MAX_RELAY;i++){
      if (relay_status[i].cntr_10ms > 0){
         relay_status[i].cntr_10ms--;
         if (relay_status[i].cntr_10ms > 0){
            if (relay_status[i].is_on) digitalWrite(relay_off_on[i][1],HIGH);
            else digitalWrite(relay_off_on[i][0],HIGH); 
         } 
         else {
            digitalWrite(relay_off_on[i][0],LOW);
            digitalWrite(relay_off_on[i][1],LOW);
         }
      }
   } 
}
