#include "ProcKeyOne.h"
#include "UART1.h"

void InitProcKeyOne(void)
{
  
}

/*
* Handler function for long press
*/
void ProcKeyLongDownKey(u8 keyName) {
	printf("got a long press, for key: %d \r\n", keyName);
}

void  ProcKeyDownKey1(void)
{  
  printf("KEY1 PUSH DOWN\r\n"); 
}

void  ProcKeyUpKey1(void)
{
  printf("KEY1 RELEASE\r\n");   
}

void  ProcKeyDownKey2(void)
{
  printf("KEY2 PUSH DOWN\r\n");  
}

void  ProcKeyUpKey2(void)
{
  printf("KEY2 RELEASE\r\n");    
}

void  ProcKeyDownKey3(void)
{
  printf("KEY3 PUSH DOWN\r\n");   
}

void  ProcKeyUpKey3(void)
{  
  printf("KEY3 RELEASE\r\n");     
}
