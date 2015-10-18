#ifndef ENCODER__H__
#define ENCODER__H__

#define WHO_AM_I 0X0F

// Gyroscope Output Registers
#define OUTX_L_G 0x22
#define OUTX_H_G 0x23
#define OUTY_L_G 0x24
#define OUTY_H_G 0x25
#define OUTZ_L_G 0x26
#define OUTZ_H_G 0x27

// Accelerometer Output Registers
#define OUTX_L_XL 0x28
#define OUTX_H_XL 0x29
#define OUTY_L_XL 0x2A
#define OUTY_H_XL 0x2B
#define OUTZ_L_XL 0x2C
#define OUTZ_H_XL 0x2D


void imu_init();          // initialize the encoder module

int encoder_ticks();          // read the encoder, in ticks

int encoder_angle();          // read the encoder angle in 1/10 degrees

void encoder_reset();         // reset the encoder position

#endif
