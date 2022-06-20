/*********************************************************************************************************
* ģ�����ƣ�Main.c
* ժ    Ҫ�����ļ���������Ӳ����ʼ��������main����
* ��ǰ�汾��1.0.0
* ��    �ߣ�SZLY(COPYRIGHT 2018 - 2020 SZLY. All rights reserved.)
* ������ڣ�2020��01��01��
* ��    �ݣ�
* ע    �⣺ע�⹴ѡOptions for Target 'Target1'->Code Generation->Use MicroLIB������printf�޷�ʹ��                                                                  
**********************************************************************************************************
* ȡ���汾��
* ��    �ߣ�
* ������ڣ�
* �޸����ݣ�
* �޸��ļ���
*********************************************************************************************************/

/*********************************************************************************************************
*                                              ����ͷ�ļ�
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
struct Context {
	volatile u8 currentSecond;
	volatile u8 currentMinute;
	volatile u8 currentHour;
	volatile u16 currentYear;
	volatile u16 currentMonth;
	volatile u16 currentDay;
	volatile char currentTimeStr[10];
	volatile char currentDateStr[20];
};

struct Context homePage = { 40, 58, 23, 2022, 6, 21, {0}, {0} };

static  void  InitSoftware(void);   //��ʼ�������ص�ģ��
static  void  InitHardware(void);   //��ʼ��Ӳ����ص�ģ��
static  void  Proc2msTask(void);    //2ms��������
static  void  Proc1SecTask(void);   //1s��������
static 	void 	handleCurrentTimeStr(struct Context* t);
static 	void  handleCurrentDateStr(struct Context* t);
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


static  void  InitSoftware(void){}

static  void  InitHardware(void)
{  
  SystemInit();       //ϵͳ��ʼ��
  InitRCC();          //��ʼ��RCCģ��
  InitNVIC();         //��ʼ��NVICģ��
  InitUART1(115200);  //��ʼ��UARTģ��
  InitTimer();        //��ʼ��Timerģ��
  InitLED();          //��ʼ��LEDģ��
  InitSysTick();      //��ʼ��SysTickģ��
	InitKeyOne();
	InitProcKeyOne();
  InitOLED();         //��ʼ��OLEDģ��
}

/*
* 2ms task
*/
static  void  Proc2msTask(void)
{  
  if(Get2msFlag())  //�ж�2ms��־״̬
  { 
		HomePageClockPerTick(0);		
		handleKeys();
		OLEDRefreshGRAM();
    //LEDFlicker(250);//������˸����
    Clr2msFlag();   //���2ms��־
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
  if(Get1SecFlag()) //�ж�1s��־״̬
  {
		
    Clr1SecFlag();  //���1s��־
  }    
}

static void handleCurrentTimeStr(struct Context* t) {
	// convert integer into string
	sprintf((*t).currentTimeStr, "%02d-%02d-%02d", (*t).currentHour, (*t).currentMinute, (*t).currentSecond);
}

static void handleCurrentDateStr(struct Context* t) {
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
					homePage.currentDay = 0;
					if(homePage.currentMonth < 11) {
						homePage.currentMonth ++;
					} else {
						homePage.currentMonth = 0;
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
	handleCurrentTimeStr(&homePage);
	handleCurrentDateStr(&homePage);
}


int main(void)
{ 
  InitSoftware();   //��ʼ�������غ���
  InitHardware();   //��ʼ��Ӳ����غ���
  
  printf("Init System has been finished.\r\n" );  //��ӡϵͳ״̬
  
  while(1)
  {
		switch(currentPage) {
			case 0: renderHomePage(); break;
			case 1: renderSettingsPage(); break;
		}
		
    Proc2msTask();  //2ms��������
    Proc1SecTask(); //1s��������   
  }
}

static void renderHomePage(void) {
	OLEDShowStringBySize(24, 13, 16, (char *) homePage.currentDateStr);
	OLEDShowStringBySize(32, 29, 16, (char *) homePage.currentTimeStr);
	OLEDShowStringBySize(40, 52, 12, "Settings>");
}

static void renderSettingsPage(void) {
	OLEDShowStringBySize(0, 0, 12, "< Setting Mode");
	OLEDShowStringBySize(24, 18, 16, "2018-01-01");
	OLEDShowStringBySize(32, 36, 16, (char *) "00-22w2-321");
}

static void ProcKeyLongDownKey(u8 keyName) {
	OLEDClear();
	// TODO: set current value in settings mode to HomePage
	currentPage = currentPage == 0 ? 1 : 0;
}

static void ProcKeyUpKey1(void) {

}
static void ProcKeyUpKey2(void) {

}
static void ProcKeyUpKey3(void) {

}
static void ProcKeyDownKey1(void) {

}
static void ProcKeyDownKey2(void) {

}
static void ProcKeyDownKey3(void) {

}