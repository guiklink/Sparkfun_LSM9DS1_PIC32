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
  i2c_master_setup();
  //NU32_WriteUART1("After IMU setup \r\n");
  
  unsigned char answer = 2;
  unsigned int waitCounter = 0;
  //sprintf(msg, "  %d", waitCounter);
  //NU32_WriteUART1(msg);

  

  //NU32_WriteUART1("\nAbout to ping IMU \r\n");
  answer = test_IMU();

  for(waitCounter = 0;
      waitCounter<100000;waitCounter++){;}

  sprintf(msg, "Received %d", answer);
  NU32_WriteUART1(msg);

  /*while (1) {
    NU32_WriteUART1("About to ping IMU \r\n");
    answer = test_IMU();
    sprintf(msg, "Received %d", answer);
    NU32_WriteUART1(msg);
    
  }*/
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
