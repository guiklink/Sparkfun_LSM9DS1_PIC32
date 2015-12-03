/* 
 * File:   i2c_master_int.h
 * Author: klink
 *
 * Created on April 12, 2015, 4:41 PM
 */


#ifndef SPARKFUNLSM9DS1__H__
#define SPARKFUNLSM9DS1__H__

//////////////////////////////////////
// Pre-defined scales for gyroscope //
//////////////////////////////////////
#define G_SCALE_1 245 / 32768.0
#define G_SCALE_2 500.0 / 32768.0
#define G_SCALE_3 2000.0 / 32768.0


///////////////////////////////////////////
// Slave addresses for SDO set as 0 or 1 //
///////////////////////////////////////////
#define SAD_AG_0 0x6A
#define SAD_AG_1 0x6B
#define SAD_M_0 0x1C
#define SAD_M_1 0x1E



int i2c_master_setup(void);              	// set up I2C 1 as a master, at 400 kHz
void i2c_master_start(void);              	// send a START signal
void i2c_master_restart(void);            	// send a RESTART signal
void i2c_master_send(unsigned char byte); 	// send a byte (either an address or data)
unsigned char i2c_master_recv(void);      	// receive a byte of data
void i2c_master_ack(int val);             	// send an ACK (0) or NACK (1)
void i2c_master_stop(void);               	// send a stop
unsigned char test_A_G();					// ping WHO_AM_I register from accel and gyro
unsigned char test_M();						// ping WHO_AM_I register from magnetometer
void get_gyro(int *output);					// output[0] = x | output[1] = y | output[2] = z
void get_accel(int *output);				// output[0] = x | output[1] = y | output[2] = z
void get_mag(int *output);					// output[0] = x | output[1] = y | output[2] = z
int get_x_A();


#endif