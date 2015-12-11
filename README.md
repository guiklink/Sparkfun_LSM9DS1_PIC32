# Sparkfun LSM9DS1 (IMU)

This library is a prototype and was created to be used with the [NU32](http://hades.mech.northwestern.edu/index.php/NU32) developed at Northwestern University. However, it need a minimum amount of modifications in order to be used with a standard PIC32. 
The LDM9DS1 is a IMU with 9 degrees of freedom, 3-axis gyroscope, 3-axis accelerometer and 3-axis magnetometer, that can communicate through both SPI and I2C protocols. This library uses [I2C](https://learn.sparkfun.com/tutorials/i2c?_ga=1.155492031.547596018.1444425156) (I2C2 more specifically), it uses less pins and it is the standard breakout board protocol (no soldering needed). 
The LDM9DS1 contains two independent slaves in the same breakboard, one for the gyroscope/accelerometer and the other for the magnetometer, that can be independently addressed as showed in tables 19 and 20 of the [datasheet](https://cdn.sparkfun.com/assets/learn_tutorials/3/7/3/LSM9DS1_Datasheet.pdf). The chip by defaul comes with SDO and SDI set to hight. 
The datasheet for the breakout can be found [here](https://www.sparkfun.com/products/13284), such as the electrical specifications and hookup guide. 
