#include "SparkFun_LSM9DS1.h"

#include<xc.h>                      // processor SFR definitions
#include<sys/attribs.h>             // __ISR macro
#include "NU32.h"          // config bits, constants, funcs for startup and UART
// I2C Master utilities, using interrupts
// Master will use I2C2 SDA2 (A3) and SCL2 (A2)
// Connect these through resistors to Vcc (3.3V). 2.3k resistors recommended, but something close will do.
// Connect SDA2 to the SDA pin on a slave device and SCL2 to the SCL pin on a slave device

#define SAD_AG_0 0x6A
#define SAD_AG_1 0x6B
#define SAD_M_0 0x1C
#define SAD_M_1 0x1E

static volatile enum {IDLE, START, WRITE, READ, RESTART, SAK, SAK2, ACK, MACK, NACK, NMACK, SUB, STOP, ERROR} state = IDLE;   // keeps track of the current i2c state

static buffer_t to_write = 0;  // data to write
static buffer_t  to_read = 0;  // data to read
static volatile unsigned char sad = 0;                     // the 7-bit address to of the slave to access
static volatile unsigned char address = 0;                     // the 7-bit address to write to / read from
static volatile unsigned int n_write = 0;                          // number of data bytes to write
static volatile unsigned int n_read = 0;                           // number of data bytes to read
char msg[20];
static unsigned int timer, waitedTooLong;

void __ISR(_I2C_2_VECTOR, IPL1SOFT) I2C2SlaveInterrupt(void) {    // _I2C_1_VECTOR = 25
  static unsigned int write_index = 0, read_index = 0;   // indexes into the read/write arrays
  NU32_WriteUART1("Inside interruption \r\n");

  switch(state) {
    case START:                                 // start bit has been sent
      NU32_WriteUART1("START \r\n");
      write_index = 0;                          // reset indices
      read_index = 0;
      sprintf(msg, "  %d \r\n", sad << 1);
      NU32_WriteUART1(msg);
      I2C2TRN = sad << 1;                       // send the slave address, with write mode set (SAD + W)
      state = SAK;
      break;
    case SAK:
      //timer = 0;
      //waitedTooLong = 0;
      NU32_WriteUART1("SAK \r\n");
      /*while(I2C2STATbits.ACKSTAT && !waitedTooLong){
        timer++;
        if(timer > 1000000){
          NU32_WriteUART1("Waited too much \r\n");
          I2C2CONbits.RSEN = 1;               // send the restart 
          state = START;
          waitedTooLong = 1;
        }
      }*/            // wait for a slave ACK (from the AG or M)
      //if(!waitedTooLong)

      NU32_WriteUART1("SUB \r\n");
      I2C2TRN = address;                        // transfer register sub-address (SUB)
      state = SAK2;
      //state = SUB;
      break;
    /*case SUB:
      NU32_WriteUART1("SUB \r\n");
      I2C2TRN = address;                        // transfer register sub-address (SUB)
      state = SAK2;
      break;*/
    case SAK2:
    NU32_WriteUART1("SAK 2 \r\n");
      while(I2C2STATbits.ACKSTAT){;}            // wait for a slave ACK (from the AG or M)
      if(n_read == 0 && n_write > 0 && write_index < n_write) {     // there are bytes to write
        state = WRITE;
      }
      else if (n_write == 0 && n_read > 0 && read_index < n_read) { // Prepare to read data
        if(read_index == 0){
          NU32_WriteUART1("READ: SR \r\n");
          I2C2CONbits.RSEN = 1;               // send the restart to begin the read
          //I2C2CONbits.SEN = 1;               // send the restart to begin the read
          NU32_WriteUART1("READ: SAD + R \r\n");
          I2C2TRN = (sad << 1) | 1;           // send the slave address, with write mode set (SAD + R)
          while(I2C2STATbits.ACKSTAT){;}      // wait for a slave ACK (from the AG or M)
        }
        I2C2CONbits.RCEN = 1;                 // Enable data receiver
        //while(!I2C2STATbits.RBF){;}           // Wait to receive data
        state = READ;
      }
      else{
        I2C2CONbits.PEN = 1;                    // Initiate stop sequence 
        state = STOP;
      }
    break;
    case WRITE:                                
      NU32_WriteUART1("WRITE \r\n");
      I2C2TRN = to_write[write_index];          // write the data
      ++write_index;
      state = SAK2;
      break;
    case RESTART: // the restart has completed
      NU32_WriteUART1("RESTART \r\n");
      // now we want to read, send the read address
      state = ACK;                  // when interrupted in ACK mode, we will initiate reading a byte
      I2C2TRN = (address << 1) | 1; // the address is sent with the read bit sent
      break;
    case READ:
      sprintf(msg, "READ  %d \r\n", read_index);
      NU32_WriteUART1(msg);
      //NU32_WriteUART1("READ \r\n");
      to_read[read_index] = I2C2RCV;
      sprintf(msg, "READ value = %d \r\n", to_read[read_index]);
      NU32_WriteUART1(msg);
      ++read_index;
      if(read_index == n_read) {  // we are done reading, so send a nack
        NU32_WriteUART1("preparing to NMACK \r\n");
        state = NMACK;
        I2C2CONbits.ACKDT = 1;    // aplicable during master receive, 1 = send NACK | 0 = send ACK
      } else {
        NU32_WriteUART1("preparing to MACK \r\n");
        state = MACK;
        I2C2CONbits.ACKDT = 0;    // send back a MACK
      }
      I2C2CONbits.ACKEN = 1;      // initiate acknowledge sequence 
      break;
    case MACK:                    // just sent an MACK meaning we want to read more bytes
      NU32_WriteUART1("MACK \r\n");
      state = READ;
      I2C2CONbits.RCEN = 1;                     // Enables receive mode
      break;
    case NMACK:
      NU32_WriteUART1("NMACK \r\n");
      //issue a stop
      state = STOP;
      I2C2CONbits.PEN = 1;
      break;
    case STOP:
      NU32_WriteUART1("STOP \r\n");
      state = IDLE; // we have returned to idle mode, indicating that the data is ready
      break;
    default:
      // some error has occurred
      state = ERROR;
  }
  //IFS0bits.I2C2MIF = 0;       //clear the interrupt flag
  IFS1bits.I2C2MIF = 0;             // clear the interrupt flag
}

void i2C_setup()
{
	int en = __builtin_disable_interrupts();  // disable interrupts

	I2C2BRG = 90;                       // I2CBRG = [1/(2*Fsck) - PGD]*Pblck - 2
	                                    // Fsck is the frequency (usually 100khz or 400 khz), PGD = 104ns
	                                    // this is 400 khz mode
	                                    // enable the i2c interrupts
	IPC8bits.I2C2IP  = 1;            	// master has interrupt priority 1
	IEC1bits.I2C2MIE = 1;            	// master interrupt is enabled
	IFS1bits.I2C2MIF = 0;            	// clear the interrupt flag
	I2C2CONbits.ON = 1;                 // turn on the I2C2 module


	if(en & 1)          // if interrupts were enabled when calling this function, enable them
  	{
    	__builtin_enable_interrupts();
  	}
}

int i2c_write_read(unsigned int slave_addr,unsigned int addr, const buffer_t write, unsigned int wlen, const buffer_t read, unsigned int rlen ) {
  n_write = wlen;
  n_read = rlen;
  to_write = write;
  to_read = read;
  sad = slave_addr;
  address = addr;
  state = START;
  //NU32_WriteUART1("here 2 \r\n");
  I2C2CONbits.SEN = 1;                          // initialize the start

  while(state != IDLE && state != ERROR) { ; }  // initialize the sequence
  return state != ERROR;
}

int i2c_write_byte(unsigned int addr, unsigned char byte) {
  return i2c_write_read(NULL,addr,&byte,1,NULL,0);
}

unsigned char test_IMU(){
  //buffer_t output;
  unsigned char output;
  //NU32_WriteUART1("here 1 \r\n");
  i2c_write_read(0x6B, A_G_WHO_AM_I, NULL, 0, &output, 1);
  return output;
  //return output[0];
}
