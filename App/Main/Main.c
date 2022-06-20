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

static  void  InitSoftware(void);   //��ʼ�������ص�ģ��
static  void  InitHardware(void);   //��ʼ��Ӳ����ص�ģ��
static  void  Proc2msTask(void);    //2ms��������
static  void  Proc1SecTask(void);   //1s��������
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
		clockPerTick(0);		
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
  InitSoftware();   //��ʼ�������غ���
  InitHardware();   //��ʼ��Ӳ����غ���
  
  printf("Init System has been finished.\r\n" );  //��ӡϵͳ״̬
  
  while(1)
  {
		renderHomePage();
		//renderSettingsPage();
    Proc2msTask();  //2ms��������
    Proc1SecTask(); //1s��������   
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
