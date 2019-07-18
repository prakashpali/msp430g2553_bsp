#include "pins_arduino.h"
// 10 (SS), 11 (MOSI), 12 (MISO), 13 (SCK)

char buf [100];
volatile byte pos;
volatile boolean process_it;

void setup (void)
{
 Serial.begin (115200);   // debugging
 Serial.println ("Uart Started");

 // have to send on master in, *slave out*
 pinMode(MISO, OUTPUT);
 
 // turn on SPI in slave mode
 SPCR |= _BV(SPE);
 
 // turn on interrupts
 SPCR |= _BV(SPIE);
 
 pos = 0;
 process_it = false;
}  // end of setup


// SPI interrupt routine
ISR (SPI_STC_vect)
{
byte c = SPDR;
 
 // add to buffer if room
 if (pos < sizeof buf)
   {
   buf [pos++] = c;
   //Serial.println (c);
   
   // example: newline means time to process buffer
   if (c == '\n')
    {
     process_it = true;
    }
     
   }  // end of room available
}

// main loop - wait for flag set in interrupt routine
void loop (void)
{
 if (process_it)
   {
   buf [pos] = 0;  
   Serial.println (buf);
   pos = 0;
   process_it = false;
   }  // end of flag set
   
}  // end of loop
