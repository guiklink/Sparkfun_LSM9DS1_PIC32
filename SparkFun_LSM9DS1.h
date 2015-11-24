/* 
 * File:   i2c_master_int.h
 * Author: klink
 *
 * Created on April 12, 2015, 4:41 PM
 */


#ifndef SPARKFUNLSM9DS1__H__
#define SPARKFUNLSM9DS1__H__

#define SAD_AG_0 0x6A
#define SAD_AG_1 0x6B
#define SAD_M_0 0x1C
#define SAD_M_1 0x1E

#define A_G_WHO_AM_I 0x0F

void i2c_master_setup(void);              // set up I2C 1 as a master, at 100 kHz

void i2c_master_start(void);              // send a START signal
void i2c_master_restart(void);            // send a RESTART signal
void i2c_master_send(unsigned char byte); // send a byte (either an address or data)
unsigned char i2c_master_recv(void);      // receive a byte of data
void i2c_master_ack(int val);             // send an ACK (0) or NACK (1)
void i2c_master_stop(void);               // send a stop
unsigned char test_IMU();



#endif