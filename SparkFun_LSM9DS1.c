#include "NU32.h"               // constants, funcs for startup and UART
#include "SparkFun_LSM9DS1.h"
#include "LSM9DS1_Registers.h"

// I2C Master utilities, 100 kHz, using polling rather than interrupts
// The functions must be callled in the correct order as per the I2C protocol
// Master will use I2C1 SDA1 (D9) and SCL1 (D10)
// Connect these through resistors to Vcc (3.3 V). 2.4k resistors recommended, 
// but something close will do.
// Connect SDA1 to the SDA pin on the slave and SCL1 to the SCL pin on a slave

int i2c_master_setup(void) {
  I2C2BRG = 90;                     // I2CBRG = [1/(2*Fsck) - PGD]*Pblck - 2 
                                    // Fsck is the freq (100 kHz here), PGD = 104 ns
  I2C2CONbits.ON = 1;               // turn on the I2C1 module

  int status = 0, wait;             // Used to check is A, G and M is working
  char status_A_G, status_M;
  status_A_G = test_A_G();
  for(wait = 0; wait < 1000000; wait++){;}
  status_M = test_M();
  for(wait = 0; wait < 1000000; wait++){;}
  if(status_A_G == WHO_AM_I_AG_RSP && status_M == WHO_AM_I_M_RSP)
    status = 1;
  return status;
}

// Start a transmission on the I2C bus
void i2c_master_start(void) {
    I2C2CONbits.SEN = 1;            // send the start bit
    while(I2C2CONbits.SEN) { ; }    // wait for the start bit to be sent
}

void i2c_master_restart(void) {     
    I2C2CONbits.RSEN = 1;           // send a restart 
    while(I2C2CONbits.RSEN) { ; }   // wait for the restart to clear
}

void i2c_master_send(unsigned char byte) { // send a byte to slave
  I2C2TRN = byte;                   // if an address, bit 0 = 0 for write, 1 for read
  while(I2C2STATbits.TRSTAT) { ; }  // wait for the transmission to finish
  if(I2C2STATbits.ACKSTAT) {        // if this is high, slave has not acknowledged
    NU32_WriteUART1("I2C2 Master: failed to receive ACK\r\n");
  }
}

unsigned char i2c_master_recv(void) { // receive a byte from the slave
    I2C2CONbits.RCEN = 1;             // start receiving data
    while(!I2C2STATbits.RBF) { ; }    // wait to receive the data
    return I2C2RCV;                   // read and return the data
}

void i2c_master_ack(int val) {        // sends ACK = 0 (slave should send another byte)
                                      // or NACK = 1 (no more bytes requested from slave)
    I2C2CONbits.ACKDT = val;          // store ACK/NACK in ACKDT
    I2C2CONbits.ACKEN = 1;            // send ACKDT
    while(I2C2CONbits.ACKEN) { ; }    // wait for ACK/NACK to be sent
}

void i2c_master_stop(void) {          // send a STOP:
  I2C2CONbits.PEN = 1;                // comm is complete and master relinquishes bus
  while(I2C2CONbits.PEN) { ; }        // wait for STOP to complete
}

void start_comm(unsigned char sad, unsigned char ad){
  i2c_master_start();
  i2c_master_send(sad << 1);
  i2c_master_send(ad);
}

int read_register(unsigned char sad, unsigned char ad, unsigned char * buffer, int n_bytes_read){
  int read_index = 0;

  start_comm(sad, ad);
  i2c_master_restart();
  i2c_master_send((sad << 1) | 1) ;

  while(read_index < n_bytes_read){
    buffer[read_index] = i2c_master_recv();
    read_index++;

    if(read_index == n_bytes_read)
      i2c_master_ack(1);
    else
      i2c_master_ack(0);
  }
  i2c_master_stop();

  return 1;
}

int write_register(unsigned char sad, unsigned char ad, unsigned char * buffer, int n_bytes_write){
  int write_index = 0;

  start_comm(sad, ad);

  // Send data
  while(write_index < n_bytes_write){
    i2c_master_send(buffer[write_index]);
    write_index++;
  }
  i2c_master_stop();

  return 1;
}

unsigned char test_A_G(){
  unsigned char buffer[1];
  read_register(SAD_AG_1, WHO_AM_I_XG, buffer, 1);
  return buffer[0];
}

unsigned char test_M(){
  unsigned char buffer[1];
  read_register(SAD_M_1, WHO_AM_I_M, buffer, 1);
  return buffer[0];
}

void config_gyro_accel_default(){      // Turn on the gyro and accel with default pre-selected configurations
  char buffer[1];
  int wait;
  buffer[0] = 0b11000001;        // See table 46 and 47 from the datasheet (Set Output Data Rate / Scale / DPS)
  write_register(SAD_AG_1, CTRL_REG1_G, buffer, 1);
  for(wait = 0; wait < 1000000; wait++){;}  // Wait before doing another operation

  buffer[0] = 0x00;              // See item 7.13 from the datasheet
  write_register(SAD_AG_1, CTRL_REG2_G, buffer, 1);
  for(wait = 0; wait < 1000000; wait++){;}  // Wait before doing another operation

  buffer[0] = 0x00;              // See table 51 from the datasheet (in this case low power is disable and ho HP filter is used)
  write_register(SAD_AG_1, CTRL_REG3_G, buffer, 1);
  for(wait = 0; wait < 1000000; wait++){;}  // Wait before doing another operation

  buffer[0] = 0b00111000;        // See table 62 from the datasheet (Enable all the axis and disable the interruption request)
  write_register(SAD_AG_1, CTRL_REG4, buffer, 1);
  for(wait = 0; wait < 1000000; wait++){;}  // Wait before doing another operation

  buffer[0] = 0x00;             // See table 54 from the datasheet (Define axis-orientation)
  write_register(SAD_AG_1, ORIENT_CFG_G, buffer, 1);
  for(wait = 0; wait < 1000000; wait++){;}  // Wait before doing another operation

  buffer[0] = 0b00111000;             // See table 64 from the datasheet (Define data decimation and enable axis)
  write_register(SAD_AG_1, CTRL_REG5_XL, buffer, 1);
  for(wait = 0; wait < 1000000; wait++){;}  // Wait before doing another operation

  buffer[0] = 0b11010000;             // See table 67 from the datasheet (Define power mode, scale, bandwidht and anti-aliasing)
  write_register(SAD_AG_1, CTRL_REG6_XL, buffer, 1);
  for(wait = 0; wait < 1000000; wait++){;}  // Wait before doing another operation
}

void config_mag(){
  char buffer[1];

  buffer[0] = 0b10100001;
  write_register(SAD_AG_1, CTRL_REG1_G, buffer, 1);
}

void get_sensor_data(char sad, char ad, int *output){       // output[0] = x | output[1] = y | output[2] = z
  unsigned char buffer[6];
  int timer;

  read_register(sad, ad, buffer, 6);

  output[0] = (buffer[1] << 8) | buffer[0];
  output[1] = (buffer[3] << 8) | buffer[2];
  output[2] = (buffer[5] << 8) | buffer[4];
  //for(timer = 0; timer < 1000000; timer++){;};
  //return (buffer[1] << 8) | buffer[0];
}

void get_gyro(int *output){
  get_sensor_data(SAD_AG_1, OUT_X_L_G, output);
}

void get_accel(int *output){
  get_sensor_data(SAD_AG_1, OUT_X_L_XL, output);
}

void get_mag(int *output){
  get_sensor_data(SAD_M_1, OUT_X_L_M, output);
}

int get_x_A(){
  unsigned char buffer[1];
  char log[20];
  int answer, timer;
  read_register(SAD_AG_1, OUT_X_L_G, buffer, 1);
  for(timer = 0; timer < 1000000; timer++){;};

  NU32_WriteUART1("\r\n");
  sprintf(log, "Accel X1: %d", buffer[0]);
  NU32_WriteUART1(log);
  

  read_register(SAD_AG_1, OUT_X_H_G, buffer, 1);
  for(timer = 0; timer < 1000000; timer++){;};

  //answer = (buffer[1] << 8) | buffer[0];
  NU32_WriteUART1("\r\n");
  sprintf(log, "Accel X2: %d", buffer[0]);
  NU32_WriteUART1(log);

  return buffer[0];
  //return (buffer[1] << 8) | buffer[0];
}
