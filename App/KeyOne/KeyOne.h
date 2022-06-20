#ifndef _KEY_ONE_H_
#define _KEY_ONE_H_

#include "DataType.h"

#define  KEY_DOWN_LEVEL_KEY1    0x00   
#define  KEY_DOWN_LEVEL_KEY2    0x00     
#define  KEY_DOWN_LEVEL_KEY3    0x00    

typedef enum
{
  KEY_NAME_KEY1 = 0, 
  KEY_NAME_KEY2,      
  KEY_NAME_KEY3,      
  KEY_NAME_MAX
}EnumKeyOneName;

void  InitKeyOne(void);                                                        
void  ScanKeyOne(u8 keyName, void(*OnKeyOneUp)(void), void(*OnKeyOneDown)(void), void(*OnKeyOneLongDown)(u8 keyName));

#endif
