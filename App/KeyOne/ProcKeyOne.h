#ifndef _PROC_KEY_ONE_H_
#define _PROC_KEY_ONE_H_

#include "DataType.h"

void  InitProcKeyOne(void);  

void  ProcKeyDownKey1(void); 
void  ProcKeyUpKey1(void);   
void  ProcKeyDownKey2(void);  
void  ProcKeyUpKey2(void);   
void  ProcKeyDownKey3(void);  
void  ProcKeyUpKey3(void);   

// handler function for longing press a key.
void 	ProcKeyLongDownKey(u8 keyName);

#endif
