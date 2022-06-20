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

// current page
volatile u8 currentPage = 0;

// Current Context
typedef struct Context {
	volatile u8 currentSecond;
	volatile u8 currentMinute;
	volatile u8 currentHour;
	volatile u16 currentYear;
	volatile u16 currentMonth;
	volatile u16 currentDay;
	volatile char currentTimeStr[10];
	volatile char currentDateStr[20];
} Context;

typedef struct SettingsState {
	// current setting position
	volatile u8 currentSettingPosition;
} SettingsState;
	

struct Context homePage = { 40, 58, 23, 2022, 6, 21, {0}, {0} };
struct Context settingsPage;
struct SettingsState settingsState = { 0 };

static  void  InitSoftware(void);   //初始化软件相关的模块
static  void  InitHardware(void);   //初始化硬件相关的模块
static  void  Proc2msTask(void);    //2ms处理任务
static  void  Proc1SecTask(void);   //1s处理任务
static 	void 	HomePageClockPerTick(u16 times);
static 	void 	renderHomePage(void);
static 	void  handleKeys(void);
static 	void 	renderSettingsPage(void);
static 	void  ProcKeyLongDownKey(u8 keyName);
static 	void  ProcKeyDownKey1(void);
static 	void  ProcKeyDownKey2(void);
static 	void  ProcKeyDownKey3(void);
static 	void 	ProcKeyUpKey1(void);
static 	void 	ProcKeyUpKey2(void);
static 	void 	ProcKeyUpKey3(void);
static 	void 	handleSettingsValue(void);
static 	void  handleSettingsPositionAnimation(void);
static  void  handleContextStr(struct Context* t);

static  void  InitSoftware(void){}

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
		HomePageClockPerTick(10);		
		
		handleKeys();
		OLEDRefreshGRAM();
		
		if(currentPage == 1) {
			handleContextStr(&settingsPage);
		}
		
		if(currentPage == 0) {
			handleContextStr(&homePage);
		}
		
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

static void handleContextStr(struct Context* t) {
	sprintf((*t).currentTimeStr, "%02d:%02d:%02d", (*t).currentHour, (*t).currentMinute, (*t).currentSecond);
	sprintf((*t).currentDateStr, "%04d-%02d-%02d", (*t).currentYear, (*t).currentMonth, (*t).currentDay);
}

static void HomePageClockPerTick(u16 times) {
	static u16 totalCount = 0;
	totalCount++;
	if(totalCount <= times) {
		return;
	};
	
	totalCount = 0;
	
	if(homePage.currentSecond < 59) {
		homePage.currentSecond ++;
	} else {
		homePage.currentSecond = 0;
		if(homePage.currentMinute < 59) {
			homePage.currentMinute ++;
		} else {
			homePage.currentMinute = 0;
			if(homePage.currentHour < 23) {
				homePage.currentHour ++;
			} else {
				homePage.currentHour = homePage.currentMinute = homePage.currentSecond= 0;
				if(homePage.currentDay < 30) {
					homePage.currentDay ++;
				} else {
					homePage.currentDay = 1;
					if(homePage.currentMonth < 11) {
						homePage.currentMonth ++;
					} else {
						homePage.currentMonth = 1;
						if (homePage.currentYear < 9999) {
							homePage.currentYear ++;
						} else {
							homePage.currentYear = homePage.currentMonth = homePage.currentDay = homePage.currentHour = homePage.currentMinute= homePage.currentSecond = 0;
						}
					}
				}
			}
		}
	}
}


int main(void)
{ 
  InitSoftware();   //初始化软件相关函数
  InitHardware();   //初始化硬件相关函数
  
  while(1)
  {
		switch(currentPage) {
			case 0: renderHomePage(); break;
			case 1: renderSettingsPage(); break;
		}
		
    Proc2msTask();  //2ms处理任务
    Proc1SecTask(); //1s处理任务   
  }
}

static void renderHomePage(void) {
	OLEDShowStringBySize(24, 13, 16, (char *) homePage.currentDateStr);
	OLEDShowStringBySize(32, 29, 16, (char *) homePage.currentTimeStr);
	OLEDShowStringBySize(40, 52, 12, "Settings>");
}

static void renderSettingsPage(void) {
	char tips[6] = {0};
	OLEDShowStringBySize(0, 0, 12, "< Setting Mode");
	OLEDShowStringBySize(28, 26, 16, (char *) settingsPage.currentDateStr);
	OLEDShowStringBySize(36, 44, 16, (char *) settingsPage.currentTimeStr);
	
	
	switch(settingsState.currentSettingPosition) {
		case 0: sprintf(tips, "%s", "second"); break;
		case 1: sprintf(tips, "%s", "minute"); break;
		case 2: sprintf(tips, "%s", "hour"); break;
		case 3: sprintf(tips, "%s", "day"); break;
		case 4: sprintf(tips, "%s", "month"); break;
		case 5: sprintf(tips, "%s", "year"); break;
		default: sprintf(tips, "%s", "tips");
	}
	
	OLEDShowStringBySize(0, 14, 12, (char*)tips);
}

static void ProcKeyLongDownKey(u8 keyName) {
	// change current screen;
	OLEDClear();
	// TODO: set current value in settings mode to HomePage
	// into settings mode, just copy current homepage state into settings page:
	if(currentPage == 0) {
		// from homepage to settings page
		settingsState.currentSettingPosition = 0;
		memcpy(&settingsPage, &homePage, sizeof(Context));
		currentPage = 1;
	} else if(currentPage == 1) {
		// from settings page to homepage
		memcpy(&homePage, &settingsPage, sizeof(Context));
		currentPage = 0;
	}
}

static void settingsMinusSecond(struct Context* t) {
	if((*t).currentSecond > 0) {
		(*t).currentSecond--;
		return;
	}
	(*t).currentSecond = 59;
}

static void settingsPlushOneSecond(struct Context* t) {
	if((*t).currentSecond < 59) {
		(*t).currentSecond++;
		return;
	}
	(*t).currentSecond = 0;
}

static void settingsPlushOneMinute(struct Context* t) {
	if((*t).currentMinute < 59) {
		(*t).currentMinute++;
		return;
	}
	(*t).currentMinute = 0;
};

static void settingsMinusOneMinute(struct Context* t) {
	if((*t).currentMinute > 0) {
		(*t).currentMinute--;
		return;
	}
	(*t).currentMinute = 59;
}

static void settingsPlushOneHour(struct Context* t) {
	if((*t).currentHour < 23) {
		(*t).currentHour++;
		return;
	}
	(*t).currentHour = 0;
};

static void settingsMinusOneHour(struct Context* t) {
	if((*t).currentHour > 0) {
		(*t).currentHour--;
		return;
	}
	(*t).currentHour = 23;
}

static void settingsPlushOneDay(struct Context* t) {
	if((*t).currentDay < 30) {
		(*t).currentDay++;
		return;
	}
	(*t).currentDay = 1;
};

static void settingsMinusOneDay(struct Context* t) {
	if((*t).currentDay > 0) {
		(*t).currentDay--;
		return;
	}
	(*t).currentDay = 30;
}

static void settingsPlushOneMonth(struct Context* t) {
	if((*t).currentMonth < 11) {
		(*t).currentMonth++;
		return;
	}
	(*t).currentMonth = 1;
};

static void settingsMinusOneMonth(struct Context* t) {
	if((*t).currentMonth > 0) {
		(*t).currentMonth--;
		return;
	}
	(*t).currentMonth = 12;
}

static void settingsPlushOneYear(struct Context* t) {
	if((*t).currentYear < 9998) {
		(*t).currentYear++;
		return;
	}
	(*t).currentYear = 0;
};

static void settingsMinusOneYear(struct Context* t) {
	if((*t).currentYear > 0) {
		(*t).currentYear--;
		return;
	}
	(*t).currentYear = 9998;
}



	
static void ProcKeyUpKey1(void) {
	OLEDClear();
	if(settingsState.currentSettingPosition < 5) {
		settingsState.currentSettingPosition++;
		return;
	}
	settingsState.currentSettingPosition = 0;
}
static void ProcKeyUpKey2(void) {
	switch(settingsState.currentSettingPosition) {
		case 0: settingsMinusSecond(&settingsPage); break;
		case 1: settingsMinusOneMinute(&settingsPage); break;
		case 2: settingsMinusOneHour(&settingsPage); break;
		case 3: settingsMinusOneDay(&settingsPage); break;
		case 4: settingsMinusOneMonth(&settingsPage); break;
		case 5: settingsMinusOneYear(&settingsPage); break;
	}
}
static void ProcKeyUpKey3(void) {
	switch(settingsState.currentSettingPosition) {
		case 0: settingsPlushOneSecond(&settingsPage); break;
		case 1: settingsPlushOneMinute(&settingsPage); break;
		case 2: settingsPlushOneHour(&settingsPage); break;
		case 3: settingsPlushOneDay(&settingsPage); break;
		case 4: settingsPlushOneMonth(&settingsPage); break;
		case 5: settingsPlushOneYear(&settingsPage); break;
	}
} 
	
static void ProcKeyDownKey1(void) {}
static void ProcKeyDownKey2(void) {}
static void ProcKeyDownKey3(void) {}