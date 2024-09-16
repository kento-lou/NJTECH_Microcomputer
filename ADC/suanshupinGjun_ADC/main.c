/**
   * @brief 算术平均法
   * @param
   * @retval
   */
#include <STC12C5A60S2.H>
#include "intrins.h"
#define uchar unsigned char
#define uint  unsigned int
uint dis_voltage;//数码管显示电压
uchar code num_disp_L[]={0xc0,0xf9,0xa4,0xb0,0x99,
					       0x92,0x82,0xf8,0x80,0x90};//无小数点，与上0x7F加小数点
uchar num[4]={0,0,0,0};//存储数码管段显示编码
uchar code wei[4]={0xf7,0xfb,0xfd,0xfe};//位选
void Delayms(int ms);
void ADC_init();//AD初始化函数
float ADC_Value(uchar Channel);//算术平均法，采样10次
uint ADC_Get(uchar Channel);//读一次AD结果
void dis_choose();//数码管段选函数
void seg_dispaly();//数码管显示
void main()
{
	ADC_init();
	while(1)
	{
		dis_voltage=ADC_Value(0)*1000;//将int化成四位数
		dis_choose();
		seg_display();
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
	P1ASF=0xFF;//P1口全部打开AD功能
	AUXR1=0x00;//ADRJ=0,高8位存ADC_RES,低2位ADC_RESL低2位
	ADC_CONTR=0x80;//打开ADC电源
	_nop_(); _nop_(); _nop_(); _nop_();
	//由于内部AD和外部电路是两套时钟，所以在设置了ADC_CONTR后要加四个延时操作
}

float ADC_Value(uchar Channel)
{
	float AD_Value;
	uchar i;
	for(i=0;i<10;i++) //采样10次取平均值
	AD_Value+=ADC_Get(Channel); 
	AD_Value/=10;
	AD_Value=(AD_Value*5.11)/1024.0;//ADRJ=0，取10位转换结果ADC_RES+ADC_RESL=1024*Vin/Vcc
	return AD_Value;
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
void seg_display()
{
	uchar i;
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