/**
   * @brief ����ƽ����
   * @param
   * @retval
   */
#include <STC12C5A60S2.H>
#include "intrins.h"
#define uchar unsigned char
#define uint  unsigned int
uint dis_voltage;//�������ʾ��ѹ
uchar code num_disp_L[]={0xc0,0xf9,0xa4,0xb0,0x99,
					       0x92,0x82,0xf8,0x80,0x90};//��С���㣬����0x7F��С����
uchar num[4]={0,0,0,0};//�洢����ܶ���ʾ����
uchar code wei[4]={0xf7,0xfb,0xfd,0xfe};//λѡ
void Delayms(int ms);
void ADC_init();//AD��ʼ������
float ADC_Value(uchar Channel);//����ƽ����������10��
uint ADC_Get(uchar Channel);//��һ��AD���
void dis_choose();//����ܶ�ѡ����
void seg_dispaly();//�������ʾ
void main()
{
	ADC_init();
	while(1)
	{
		dis_voltage=ADC_Value(0)*1000;//��int������λ��
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
	P1ASF=0xFF;//P1��ȫ����AD����
	AUXR1=0x00;//ADRJ=0,��8λ��ADC_RES,��2λADC_RESL��2λ
	ADC_CONTR=0x80;//��ADC��Դ
	_nop_(); _nop_(); _nop_(); _nop_();
	//�����ڲ�AD���ⲿ��·������ʱ�ӣ�������������ADC_CONTR��Ҫ���ĸ���ʱ����
}

float ADC_Value(uchar Channel)
{
	float AD_Value;
	uchar i;
	for(i=0;i<10;i++) //����10��ȡƽ��ֵ
	AD_Value+=ADC_Get(Channel); 
	AD_Value/=10;
	AD_Value=(AD_Value*5.11)/1024.0;//ADRJ=0��ȡ10λת�����ADC_RES+ADC_RESL=1024*Vin/Vcc
	return AD_Value;
}

uint ADC_Get(uchar Channel)
{
	ADC_CONTR=0x88|Channel;//��ADC��Դ,����ADC,ѡ��P10��ģ������ͨ��
	_nop_(); _nop_(); _nop_(); _nop_();
	while(!(ADC_CONTR&0x10));    //�ȴ�ת�����
	ADC_CONTR&=0xe7;      //�ر�ADת����ADC_FLAG��0
	return(ADC_RES*4+ADC_RESL);   //����ADת����ɵ�10λ����
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
	{//����ܶ�̬��ʾ,���λ��С����
		if(i==3)//���λ��С����
			P0=num_disp_L[num[i]]&0x7F;
		else
			P0=num_disp_L[num[i]];
		P2=wei[i];
		Delayms(2);
	}
}