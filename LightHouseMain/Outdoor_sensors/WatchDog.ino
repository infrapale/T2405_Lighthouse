#include <avr/wdt.h>


int firstTime = true;

#define TIMEOUTPERIOD 10000             // You can make this time as long as you want,
                                       // it's not limited to 8 seconds like the normal
                                       // watchdog
//#define doggieTickle() resetTime = millis();  // This macro will reset the timer
void(* resetFunc) (void) = 0; //declare reset function @ address 0

void watchdogSetup()
{
  cli();  // disable all interrupts
  wdt_reset(); // reset the WDT timer
  MCUSR &= ~(1<<WDRF);  // because the data sheet said to
  /*
  WDTCSR configuration:
  WDIE = 1 :Interrupt Enable
  WDE = 1  :Reset Enable - I won't be using this on the 2560
  WDP3 = 0 :For 1000ms Time-out
  WDP2 = 1 :bit pattern is 
  WDP1 = 1 :0110  change this for a different
  WDP0 = 0 :timeout period.
  */
  // Enter Watchdog Configuration mode:
  WDTCSR = (1<<WDCE) | (1<<WDE);
  // Set Watchdog settings: interrupte enable, 0110 for timer
  WDTCSR = (1<<WDIE) | (0<<WDP3) | (1<<WDP2) | (1<<WDP1) | (0<<WDP0);
  sei();
  //Serial.println("finished watchdog setup");  // just here for testing
}


ISR(WDT_vect) // Watchdog timer interrupt.
{ 
  if(millis() - resetTime > TIMEOUTPERIOD){
    Serial.println("Reboot Now!");  // just here for testing
    doggieTickle();                                          // take these lines out
    resetFunc();     // This will call location zero and cause a reboot.
  }
}

/*
void setup(){
  delay(4000);
  watchdogSetup();
  Serial.begin(9600);
  Serial.println("Hello, in setup");
}



void loop() {
  if (firstTime){
    firstTime = false;
    Serial.println("In loop waiting for Watchdog");
  }
  if(millis() - resetTime > 2000){
     doggieTickle();  // if you uncomment this line, it will keep resetting the timer.
  }
}
*/
