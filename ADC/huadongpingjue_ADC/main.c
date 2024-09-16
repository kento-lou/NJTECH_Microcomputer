//滑动平均法
#include <STC12C5A60S2.H>
#include "intrins.h"
#define uchar unsigned char
#define uint  unsigned int
#define N 10
float voltage;//AD输入电压
uint dis_voltage;//数码管显示电压
uchar code num_disp_L[]={0xc0,0xf9,0xa4,0xb0,0x99,
												 0x92,0x82,0xf8,0x80,0x90};//无小数点，与上0x7F加小数点
uchar num[4]={0,0,0,0};
uchar code wei[4]={0xf7,0xfb,0xfd,0xfe};//位选
uchar rec_count=0;//采样次数
void Delayms(int ms);
void ADC_init();
float ADC_Value(uchar Channel);//滑动平均法，采样10次
uint ADC_Get(uchar Channel);
void dis_choose();
void main()
{
	uchar i;
	ADC_init();
	while(1)
	{
			dis_voltage=ADC_Value(0)*1000;
			dis_choose();
			for(i=0;i<4;i++)
			{//数码管动态显示,最高位有小数点
				if(i==3)//最高位加小数点
					P0=num_disp_L[num[i]]&0x7F;
				else
					P0=num_disp_L[num[i]];
				P2=wei[i];
				Delayms(2);
			}	
	}
}
void Delayms(int ms)		//@11.0592MHz
{
	unsigned char data i, j;
	while(ms--)
	{
		i = 11;
		j = 190;
		do
		{
			while (--j);
		} while (--i);
	}
}
void ADC_init()
{
	ADC_RES=0;
	ADC_RESL=0;
	P1ASF=0x01;//P10作模拟输入
	AUXR1=0x00;//ADRJ=0,高8位存ADC_RES,低2位ADC_RESL低2位
	ADC_CONTR=0x80;//打开ADC电源
	Delayms(2);
}
//前十次的每采样一次就相加并取平均值，之后每次先减去前一次的平均值再相加取平均值
float ADC_Value(uchar Channel)
{
	float AD_Value;
	uint voltage_buf[N];//缓冲区
	if(rec_count<N)
	{
		voltage_buf[rec_count]=ADC_Get(Channel); 
		AD_Value+=(voltage_buf[rec_count]*5.11)/1024.0;
		//ADRJ=0，取10位转换结果ADC_RES+ADC_RESL=1024*Vin/Vcc
		rec_count++;
		return AD_Value/(rec_count*1.0);
	}
	else
	{
		AD_Value-=AD_Value/(rec_count*1.0);
		AD_Value+=(ADC_Get(Channel)*5.11)/1024.0;
		return AD_Value/(rec_count*1.0);
	}
}



uint ADC_Get(uchar Channel)
{
	ADC_CONTR=0x88|Channel;//打开ADC电源,启动ADC,选择P10作模拟输入通道
	_nop_(); _nop_(); _nop_(); _nop_();
	while(!(ADC_CONTR&0x10));    //等待转换完成
	ADC_CONTR&=0xe7;      //关闭AD转换，ADC_FLAG清0
	return(ADC_RES*4+ADC_RESL);   //返回AD转换完成的10位数据
}

void dis_choose()
{
	num[0]=dis_voltage%10;
  num[1]=dis_voltage/10%10;
	num[2]=dis_voltage/100%10;
	num[3]=dis_voltage/1000%10;
}