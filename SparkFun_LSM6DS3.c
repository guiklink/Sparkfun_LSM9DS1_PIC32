#include "SparkFun_LSM6DS3.h"
#include <xc.h>


static int imu_read(int register_address) { 

  int offset = 1 << 15;           // First but is 1 for read operations
  int address = read << 8;         // Put the register address in the proper positon (xxxx xxxx 0000 0000)

  SPI4BUF = read;                 // send the command

  while (!SPI4STATbits.SPIRBF) { ; } // wait for the response
  
  SPI4BUF;                        // garbage was transfered over, ignore it
  SPI4BUF = 5;                    // write garbage, but the corresponding read will have the data
 
  while (!SPI4STATbits.SPIRBF) { ; }

  return SPI4BUF;
}


void imu_init(void) {
  // SPI initialization for reading from the encoder chip
  SPI4CON = 0; 		          // stop and reset SPI4
  SPI4BUF;		              // read to clear the rx buffer
  SPI4BRG = 0x3; 	          // bit rate to 8MHz, SPI4BRG = 80000000/(2*desired)-1      0000 0000 0000 0100
  SPI4STATbits.SPIROV = 0;  // clear the overflow
  SPI4CONbits.MSTEN = 1;    // master mode
  SPI4CONbits.MSSEN = 1;    // Slave select enable (automatically bring CS low)
  SPI4CONbits.MODE16 = 1;   // 16 bit mode
  SPI4CONbits.MODE32 = 0; 
  SPI4CONbits.SMP = 1;      // sample at the end of the clock
  SPI4CONbits.ON = 1;       // turn spi on
}


// OLD FUNCTIONS

static int encoder_command(int read) { // send  a command to the encoder chip
  SPI4BUF = read;                 // send the command

  while (!SPI4STATbits.SPIRBF) { ; } // wait for the response
  
  SPI4BUF;                        // garbage was transfered over, ignore it
  SPI4BUF = 5;                    // write garbage, but the corresponding read will have the data
 
  while (!SPI4STATbits.SPIRBF) { ; }

  return SPI4BUF;
}

int encoder_ticks(void) {
  encoder_command(1);	        // we need to read twice to get a valid reading
  return encoder_command(1);
}

void encoder_reset() {        // reset the encoder position
  encoder_command(0);
}

int encoder_angle() {         // read the encoder angle in 1/10 degrees
  int tick_counter_ang;

  tick_counter_ang = encoder_ticks() - 32768; // This is the encoder 0 dg/10

  tick_counter_ang = (int)((float)tick_counter_ang * 2.694);

  // 334 * 4 ticks / 360 degrees
  // 334 encoders by line      
  return tick_counter_ang;
}          

