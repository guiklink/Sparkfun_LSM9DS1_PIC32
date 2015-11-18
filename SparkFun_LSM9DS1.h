/* 
 * File:   i2c_master_int.h
 * Author: klink
 *
 * Created on April 12, 2015, 4:41 PM
 */


#ifndef SPARKFUNLSM9DS1__H__
#define SPARKFUNLSM9DS1__H__

#define A_G_WHO_AM_I 0x0F

// buffer pointer type.  The buffer is shared by an ISR and mainline code.
// the pointer to the buffer is also shared by an ISR and mainline code.
// Hence the double volatile qualification
typedef volatile unsigned char * volatile buffer_t;
unsigned char test_IMU();

void i2c_master_setup(); //sets up i2c2 as a master using an interrupt

// initiate an i2c write read operation at the given address. You can optionally only read or only write by passing zero lengths for the reading or writing
// this will not return until the transaction is complete.  returns false on error
int i2c_write_read(unsigned int slave_addr,unsigned int addr, const buffer_t write, unsigned int wlen, const buffer_t read, unsigned int rlen );



#endif