//*********************************************************************************************
//   RFM69 Radio Routines
//*********************************************************************************************
//#include <RFM69.h>    //get it here: https://www.github.com/lowpowerlab/rfm69
// Change to 434.0 or other frequency, must match RX's freq!
#define RF69_FREQ   434.0  //915.0

#define IS_RFM69HCW    true // set to 'true' if you are using an RFM69HCW module
#define RECEIVER      BROADCAST    // The recipient of packets
#define RFM69_CS      10
#define RFM69_INT     2
#define RFM69_IRQN    0  // Pin 2 is IRQ 0!
#define RFM69_RST     9
#define WILDCARD_CHAR '$'

RH_RF69 rf69(RFM69_CS, RFM69_INT);
int  rr_state;
byte relay_indx;
char relay_func;
uint8_t len = sizeof(radio_buff);

void InitRfm69(void){
     // Hard Reset the RFM module
    pinMode(RFM69_RST, OUTPUT);
    digitalWrite(RFM69_RST, HIGH);
    delay(100);
    digitalWrite(RFM69_RST, LOW);
    delay(100);

     if (!rf69.init()) {
    Serial.println("RFM69 radio init failed");
    while (1);
  }
  Serial.println("RFM69 radio init OK!");
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for low power module)
  // No encryption
  if (!rf69.setFrequency(RF69_FREQ)) {
    Serial.println("setFrequency failed");
  }

  // If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power with the
  // ishighpowermodule flag set like this:
  rf69.setTxPower(20, true);  // range from 14-20 for power, 2nd arg must be true for 69HCW
  uint8_t key[] ="VillaAstrid_2003"; //exactly the same 16 characters/bytes on all nodes!
  rf69.setEncryptionKey(key);
  Serial.print("RFM69 radio @");  Serial.print((int)RF69_FREQ);  Serial.println(" MHz");
  pinMode(LED, OUTPUT);

}

void InitRadioReceive(void){
   rr_state = 0;
}


void radiate_msg( char *radio_msg ) {
    if (radio_msg[0] != 0){
       rf69.send((uint8_t *)radio_msg, strlen(radio_msg));
       rf69.waitPacketSent();
       // Serial.println(radio_msg);      
    }
}

 
void ReadRadioMsg(){
   byte i;
   boolean do_continue;
   if (rf69.recv(radio_buff, &len)) {
      if (!len) return;
      radio_buff[len] = 0;
      Serial.print("Received [");
      Serial.print(len);
      Serial.print("]: ");
      Serial.println((char*)radio_buff);
      Serial.print("RSSI: ");
      Serial.println(rf69.lastRssi(), DEC);
      do_continue = true;
     }
    else do_continue = false;

    if (do_continue) {
      if (radio_buff[0] != '<' ) do_continue = false;
    }
    if (do_continue) {
      if (radio_buff[1] != '*' ) do_continue = false;
    }
    if (do_continue) {
      if (radio_buff[2] != 'R' ) do_continue = false;
    }
    if (do_continue) {
      if (radio_buff[3] != 'L' ) do_continue = false;
    }
    if (do_continue) {
      if (radio_buff[4] != 'H' ) do_continue = false;
    }
    if (do_continue) {
      if (radio_buff[5] != '_' ) do_continue = false;
    }
    if (do_continue) {
       switch(radio_buff[6]){
          case '1': relay_indx = 0; break;
          case '2': relay_indx = 1; break;
          default: do_continue = false; break;
        } 
    }
    if (do_continue) {
      if (radio_buff[7] != '=' ) do_continue = false;
    }
    if (do_continue) {
       switch(radio_buff[8]){
           case 'T': toggle_relay(relay_indx); break;
           case '1': turn_on_relay(relay_indx); break;
           case '0': turn_off_relay(relay_indx); break;         
        }
    }
}
