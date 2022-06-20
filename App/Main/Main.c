/*********************************************************************************************************
* 模块名称：Main.c
* 摘    要：主文件，包含软硬件初始化函数和main函数
* 当前版本：1.0.0
* 作    者：SZLY(COPYRIGHT 2018 - 2020 SZLY. All rights reserved.)
* 完成日期：2020年01月01日
* 内    容：
* 注    意：注意勾选Options for Target 'Target1'->Code Generation->Use MicroLIB，否则printf无法使用                                                                  
**********************************************************************************************************
* 取代版本：
* 作    者：
* 完成日期：
* 修改内容：
* 修改文件：
*********************************************************************************************************/

/*********************************************************************************************************
*                                              包含头文件
*********************************************************************************************************/
#include "Main.h"
#include "stm32f10x_conf.h"
#include "DataType.h"
#include "NVIC.h"
#include "SysTick.h"
#include "RCC.h"
#include "Timer.h"
#include "UART1.h"
#include "LED.h"
#include "OLED.h"
#include "string.h"
#include "stdlib.h"
#include "KeyOne.h"
#include "ProcKeyOne.h"

// current seconds
volatile u8 currentSecond = 40;
// current Mimutes
volatile u8 currentMimutes = 58;
// current hours
volatile u8 currentHours = 23;

// current years
volatile u16 currentYear = 2022;
// current months 
volatile u16 currentMonth = 6;
// current day
volatile u16 currentDay = 21;

// currentTimeStr
volatile char currentTimeStr[10] = {0};

// currentDateStr
volatile char currentDateStr[20] = {0};

static  void  InitSoftware(void);   //初始化软件相关的模块
static  void  InitHardware(void);   //初始化硬件相关的模块
static  void  Proc2msTask(void);    //2ms处理任务
static  void  Proc1SecTask(void);   //1s处理任务
static 	void 	handleCurrentTimeStr(void);
static 	void 	clockPerTick(u16 times);
static 	void 	renderHomePage(void);
static 	void  handleKeys(void);
static 	void 	renderSettingsPage(void);
static  void  InitSoftware(void)
{

}

static  void  InitHardware(void)
{  
  SystemInit();       //系统初始化
  InitRCC();          //初始化RCC模块
  InitNVIC();         //初始化NVIC模块
  InitUART1(115200);  //初始化UART模块
  InitTimer();        //初始化Timer模块
  InitLED();          //初始化LED模块
  InitSysTick();      //初始化SysTick模块
	InitKeyOne();
	InitProcKeyOne();
  InitOLED();         //初始化OLED模块
}

/*
* 2ms task
*/
static  void  Proc2msTask(void)
{  
  if(Get2msFlag())  //判断2ms标志状态
  { 
		clockPerTick(0);		
		handleKeys();
		OLEDRefreshGRAM();
    //LEDFlicker(250);//调用闪烁函数
    Clr2msFlag();   //清除2ms标志
  }
}

static void handleKeys(void) {
	static u8 t = 0;
	if(t < 1) {
		t++;
		return;
	}
	
	t = 0;
	ScanKeyOne(KEY_NAME_KEY1, ProcKeyUpKey1, ProcKeyDownKey1, ProcKeyLongDownKey);
	ScanKeyOne(KEY_NAME_KEY2, ProcKeyUpKey2, ProcKeyDownKey2, ProcKeyLongDownKey);
	ScanKeyOne(KEY_NAME_KEY3, ProcKeyUpKey3, ProcKeyDownKey3, ProcKeyLongDownKey);
}

/*
* 1s task
*/
static  void  Proc1SecTask(void)
{ 
  if(Get1SecFlag()) //判断1s标志状态
  {
		
    Clr1SecFlag();  //清除1s标志
  }    
}

static void handleCurrentTimeStr(void) {
	// convert integer into string
	sprintf(currentTimeStr, "%02d-%02d-%02d", currentHours, currentMimutes, currentSecond);
}

static void handleCurrentDateStr(void) {
	sprintf(currentDateStr, "%04d-%02d-%02d", currentYear, currentMonth, currentDay);
}

static void clockPerTick(u16 times) {
	static u16 totalCount = 0;
	totalCount++;
	if(totalCount <= times) {
		return;
	};
	
	totalCount = 0;
	
	if(currentSecond < 59) {
		currentSecond ++;
	} else {
		currentSecond = 0;
		if(currentMimutes < 59) {
			currentMimutes ++;
		} else {
			currentMimutes = 0;
			if(currentHours < 23) {
				currentHours ++;
			} else {
				currentSecond = currentMimutes = currentHours = 0;
				if(currentDay < 30) {
					currentDay ++;
				} else {
					currentDay = 0;
					if(currentMonth < 11) {
						currentMonth ++;
					} else {
						currentMonth = 0;
						if (currentYear < 9999) {
							currentYear ++;
						} else {
							currentYear = currentMonth = currentDay = currentHours = currentMimutes = currentSecond = 0;
						}
					}
				}
			}
		}
	}
	handleCurrentTimeStr();
	handleCurrentDateStr();
}


int main(void)
{ 
  InitSoftware();   //初始化软件相关函数
  InitHardware();   //初始化硬件相关函数
  
  printf("Init System has been finished.\r\n" );  //打印系统状态
  
  while(1)
  {
		renderHomePage();
		//renderSettingsPage();
    Proc2msTask();  //2ms处理任务
    Proc1SecTask(); //1s处理任务   
  }
}

static void renderHomePage(void) {
	OLEDShowStringBySize(24, 13, 16, (char *) currentDateStr);
	OLEDShowStringBySize(32, 29, 16, (char *)currentTimeStr);
	OLEDShowStringBySize(40, 52, 12, "Settings>");
	// TODO: long press go into settings mode.
}

static void renderSettingsPage(void) {
	OLEDShowStringBySize(0, 0, 12, "< Setting Mode");
	OLEDShowStringBySize(24, 18, 16, "2018-01-01");
	OLEDShowStringBySize(32, 36, 16, (char *)currentTimeStr);
	// TODO: long press return and save settings.
}
