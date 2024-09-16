/*
方案说明：
	1、LED部分：		LED1-3做第一组绿、黄、红灯，LED5-7做第二组绿、黄、红灯
	2、数码管部分：	数码管高二位对应一组，低二位对应第二组
									第二组红灯时长等于第一组绿黄时长，2组黄灯时长相等，第二组绿灯时长等于第一组红灯时长减第二组黄灯时长，灯计时用TIM0实现
	3、按键部分：		S4控制count和set模式，上电后默认在count模式，set模式下倒计时暂停，灯保持现状
									在Set模式下按下S1第一组时间加1S，按下S2第一组时间减1，第二组的时间根据对应关系自动计算,自动保存。在count模式下两个按键无效
*/
/*----------include--------------*/
#include <STC12C5A60S2.H>
#include "TIM_set.h"//定时器设置文件
#include "Delayms.h"//延时文件
#include "IAP.h"//I2C文件
/*----------define---------------*/
#define green_time_iap_ADDRESS 0x0000 //绿灯时间eeprom地址  
#define red_time_iap_ADDRESS 0x0200 //绿灯时间eeprom地址    
/*----------variable---------------*/
unsigned char green_time1;//第一组绿灯时间
unsigned char yellow_time1=5;//第一组黄灯时间
unsigned char red_time1;//第一组红灯时间
unsigned char green_time2;//第二组绿灯时间
unsigned char yellow_time2;//第二组黄灯时间
unsigned char red_time2;//第二组红灯时间
unsigned char count_t0=0;//记录进入t0中断次数
unsigned char time_1;//第一组灯时间
unsigned char time_2;//第二组灯时间
unsigned char judge_flag_1=0;//0表示第一组进入绿灯，1表示进入黄灯，2表示进入红灯
unsigned char judge_flag_2=2;//0表示第二组进入绿灯，1表示进入黄灯，2表示进入红灯
unsigned int seg_time;//数码管显示
unsigned char seg_num[4]={0,0,0,0};//数码管显示
unsigned char code wei_num[4]={0xf7,0xfb,0xfd,0xfe};//位选码
unsigned char code duan_num[10]={0xc0,0xf9,0xa4,0xb0,0x99,
								0x92,0x82,0xf8,0x80,0x90};//段选码
unsigned char state_flag=0;//状态标志位，0表示count状态，1表示set模式

/*----------function--------------*/
void Timer0_Init(void);
void time_compute();//倒计时计算函数
void led_judge_1();//处理第一组灯的倒计时和状态转换函数
void led_judge_2();//处理第二组灯的倒计时和状态转换函数
void seg_translation();//译码函数
void seg();//数码管动态扫描函数
unsigned char key_scan();//按键扫描函数
void key_compute_save();//时间加减函数
void save();//时间写入EEPROM函数		
void read();	
void time2_compuate();													
/*---------------------------------*/
void main()
{
	/*iap_ERASE(green_time_iap_ADDRESS);//擦除扇区
	iap_PROGRAM(green_time_iap_ADDRESS,25);//写东西
	iap_ERASE(red_time_iap_ADDRESS);//擦除扇区
	iap_PROGRAM(red_time_iap_ADDRESS,30);//写东西*/
	green_time1=iap_READ(green_time_iap_ADDRESS);
	red_time1=iap_READ(red_time_iap_ADDRESS);
	judge_flag_1=0;
	time_compute();
	time_1=green_time1;
	time_2=red_time2;
	EA=1;
	EX0=1;
	IT0=1;
	Timer0_Init();
	P1=0xBE;
	while(1)
	{
		if(!state_flag)
		{//count模式
			time_compute();
			if(!time_1) led_judge_1();
			if(!time_2) led_judge_2();
			seg();
		}
		else
		{//set模式
			key_compute_save();
			seg();
		}
	}
}
void int0() interrupt 0
{
	Delayms(10);
	if(++state_flag==2)	state_flag=0;
	switch(state_flag)
	{
		case 0:
		{
			TR0=1;//count模式下重新开始计时
		}break;
		case 1:
		{
			TR0=0;//set模式下暂停计时
		}break;
	}
}
void Timer0_Isr(void) interrupt 1//此定时器同时计时两组灯
{
	TL0 = 0x00;				
	TH0 = 0x4C;
	if(++count_t0==20)
	{//1S
		count_t0=0;
		if(time_1) time_1--;
		if(time_2) time_2--;
	}
	if((judge_flag_1==1)&&((count_t0%2)==0)) P1=P1^0x02;
	if((judge_flag_2==1)&&((count_t0%2)==0)) P1=P1^0x20;
}

void time_compute()
{
	yellow_time2=yellow_time1;
	green_time2=red_time1-yellow_time2;
	red_time2=green_time1+yellow_time1;
}
void led_judge_1()
{
	unsigned char temp=0;
		if(++judge_flag_1==3)
			judge_flag_1=0;
	switch(judge_flag_1)
	{
		case 0:
		{
			temp=P1&0xF0;//xxxx 0000
			time_1=green_time1;
			P1=temp|0x0E;//xxxx 1110
		}break;
		case 1:
		{
			temp=P1&0xF0;//xxxx 0000
			time_1=yellow_time1;
			P1=temp|0x0D;//xxxx 1101
		}break;
		case 2:
		{
			temp=P1&0xF0;//xxxx 0000
			time_1=red_time1;
			P1=temp|0x0B;//xxxx 1011
		}break;
	}
}
void led_judge_2()
{
	
	unsigned char temp=0;
		if(++judge_flag_2==3)
			judge_flag_2=0;
	switch(judge_flag_2)
	{
		case 0:
		{
			temp=P1&0x0F;//0000 xxxx 
			time_2=green_time2;
			P1=temp|0xE0;//1110 xxxx
		}break;
		case 1:
		{
			temp=P1&0x0F;//0000 xxxx
			time_2=yellow_time2;
			P1=temp|0xD0;//1101 xxxx
		}break;
		case 2:
		{
			temp=P1&0x0F;//0000 xxxx 
			time_2=red_time2;
			P1=temp|0xB0;//1011 xxxx 
		}break;
	}

}
void seg_translation()
{
	seg_num[0]=seg_time%10;
	seg_num[1]=seg_time/10%10;
	seg_num[2]=seg_time/100%10;
	seg_num[3]=seg_time/1000%10;
}
void seg()
{
	unsigned char i;
	seg_time=time_1*100+time_2;
	seg_translation();
	for(i=0;i<4;i++)
	{
		P0=duan_num[seg_num[i]];
		P2=wei_num[i];
		Delayms(2);
	}
}
unsigned char key_scan()
{
	static kp=0;
	if(P3!=0xFF)
	{
		Delayms(10);
		if((P3!=0xFF)&&(kp==0))
		{
			kp=1;
			if(P3==0xDF) return 1;
			if(P3==0xEF) return 2;
		}
	}
	else kp=0;
	return 0;
}
void key_compute_save()
{
	unsigned char key_val;
	key_val=key_scan();
	switch(key_val)
	{
		case 1:
		{
			if(time_1++<99);
			save();
			read();
			time_compute();
			time2_compuate();	
		}break;
		case 2:
		{
			if(time_1-->5);
			save();
			read();
			time_compute();
			time2_compuate();	
		}break;
	}
}
void save()
{
	switch(judge_flag_1)
	{
		case 0:
		{
			iap_ERASE(green_time_iap_ADDRESS);//擦除扇区
			iap_PROGRAM(green_time_iap_ADDRESS,time_1);//写东西
		}break;
		case 2:
		{
			iap_ERASE(red_time_iap_ADDRESS);//擦除扇区
			iap_PROGRAM(red_time_iap_ADDRESS,time_1);//写东西
		}break;
	}
}
void read()
{
	green_time1=iap_READ(green_time_iap_ADDRESS);
	red_time1=iap_READ(red_time_iap_ADDRESS);
}
void time2_compuate()
{
	switch(judge_flag_2)
	{
		case 0:
		{
			time_2=green_time2;
		}break;
		case 2:
		{
			time_2=red_time2;
		}break;
	}
}
