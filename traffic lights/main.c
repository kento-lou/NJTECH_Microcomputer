/*
����˵����
	1��LED���֣�		LED1-3����һ���̡��ơ���ƣ�LED5-7���ڶ����̡��ơ����
	2������ܲ��֣�	����ܸ߶�λ��Ӧһ�飬�Ͷ�λ��Ӧ�ڶ���
									�ڶ�����ʱ�����ڵ�һ���̻�ʱ����2��Ƶ�ʱ����ȣ��ڶ����̵�ʱ�����ڵ�һ����ʱ�����ڶ���Ƶ�ʱ�����Ƽ�ʱ��TIM0ʵ��
	3���������֣�		S4����count��setģʽ���ϵ��Ĭ����countģʽ��setģʽ�µ���ʱ��ͣ���Ʊ�����״
									��Setģʽ�°���S1��һ��ʱ���1S������S2��һ��ʱ���1���ڶ����ʱ����ݶ�Ӧ��ϵ�Զ�����,�Զ����档��countģʽ������������Ч
*/
/*----------include--------------*/
#include <STC12C5A60S2.H>
#include "TIM_set.h"//��ʱ�������ļ�
#include "Delayms.h"//��ʱ�ļ�
#include "IAP.h"//I2C�ļ�
/*----------define---------------*/
#define green_time_iap_ADDRESS 0x0000 //�̵�ʱ��eeprom��ַ  
#define red_time_iap_ADDRESS 0x0200 //�̵�ʱ��eeprom��ַ    
/*----------variable---------------*/
unsigned char green_time1;//��һ���̵�ʱ��
unsigned char yellow_time1=5;//��һ��Ƶ�ʱ��
unsigned char red_time1;//��һ����ʱ��
unsigned char green_time2;//�ڶ����̵�ʱ��
unsigned char yellow_time2;//�ڶ���Ƶ�ʱ��
unsigned char red_time2;//�ڶ�����ʱ��
unsigned char count_t0=0;//��¼����t0�жϴ���
unsigned char time_1;//��һ���ʱ��
unsigned char time_2;//�ڶ����ʱ��
unsigned char judge_flag_1=0;//0��ʾ��һ������̵ƣ�1��ʾ����Ƶƣ�2��ʾ������
unsigned char judge_flag_2=2;//0��ʾ�ڶ�������̵ƣ�1��ʾ����Ƶƣ�2��ʾ������
unsigned int seg_time;//�������ʾ
unsigned char seg_num[4]={0,0,0,0};//�������ʾ
unsigned char code wei_num[4]={0xf7,0xfb,0xfd,0xfe};//λѡ��
unsigned char code duan_num[10]={0xc0,0xf9,0xa4,0xb0,0x99,
								0x92,0x82,0xf8,0x80,0x90};//��ѡ��
unsigned char state_flag=0;//״̬��־λ��0��ʾcount״̬��1��ʾsetģʽ

/*----------function--------------*/
void Timer0_Init(void);
void time_compute();//����ʱ���㺯��
void led_judge_1();//�����һ��Ƶĵ���ʱ��״̬ת������
void led_judge_2();//����ڶ���Ƶĵ���ʱ��״̬ת������
void seg_translation();//���뺯��
void seg();//����ܶ�̬ɨ�躯��
unsigned char key_scan();//����ɨ�躯��
void key_compute_save();//ʱ��Ӽ�����
void save();//ʱ��д��EEPROM����		
void read();	
void time2_compuate();													
/*---------------------------------*/
void main()
{
	/*iap_ERASE(green_time_iap_ADDRESS);//��������
	iap_PROGRAM(green_time_iap_ADDRESS,25);//д����
	iap_ERASE(red_time_iap_ADDRESS);//��������
	iap_PROGRAM(red_time_iap_ADDRESS,30);//д����*/
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
		{//countģʽ
			time_compute();
			if(!time_1) led_judge_1();
			if(!time_2) led_judge_2();
			seg();
		}
		else
		{//setģʽ
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
			TR0=1;//countģʽ�����¿�ʼ��ʱ
		}break;
		case 1:
		{
			TR0=0;//setģʽ����ͣ��ʱ
		}break;
	}
}
void Timer0_Isr(void) interrupt 1//�˶�ʱ��ͬʱ��ʱ�����
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
			iap_ERASE(green_time_iap_ADDRESS);//��������
			iap_PROGRAM(green_time_iap_ADDRESS,time_1);//д����
		}break;
		case 2:
		{
			iap_ERASE(red_time_iap_ADDRESS);//��������
			iap_PROGRAM(red_time_iap_ADDRESS,time_1);//д����
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
