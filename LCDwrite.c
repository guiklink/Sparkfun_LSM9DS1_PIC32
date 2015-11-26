//#define NU32_STANDALONE  // uncomment if program is standalone, not bootloaded
#include "NU32.h"          // config bits, constants, funcs for startup and UART
#include "LCD.h"
#include "SparkFun_LSM9DS1.h"

#define MSG_LEN 20

int main() {
  char msg[MSG_LEN];
  int nreceived = 1;

  NU32_Startup();         // cache on, interrupts on, LED/button init, UART init
  //NU32_WriteUART1("Before setup \r\n");
  LCD_Setup();
  //NU32_WriteUART1("After setup \r\n");
  //NU32_WriteUART1("Before IMU setup \r\n");
  nreceived = i2c_master_setup();
  //NU32_WriteUART1("After IMU setup \r\n");
  
  if(nreceived)
    NU32_WriteUART1("IMU working!\r\n");
  else
    NU32_WriteUART1("IMU NOT working!\r\n");

  return 0;
}

/*
  while (1) {
    NU32_WriteUART1("What do you want to write? ");
    NU32_ReadUART1(msg, MSG_LEN);             // get the response
    LCD_Clear();                              // clear LCD screen
    LCD_Move(0,0);
    LCD_WriteString(msg);                     // write msg at row 0 col 0
    sprintf(msg, "Received %d", nreceived);   // display how many messages received
    ++nreceived;
    LCD_Move(1,3);
    LCD_WriteString(msg);                     // write new msg at row 0 col 2
    NU32_WriteUART1("\r\n");          
  }
*/
