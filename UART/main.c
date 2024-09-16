//֡ͷΪ0XFA,0XAF,������Ϊ0xA0,0X0A
#include <STC12C5A60S2.H>
#include <stdio.h>
#include <string.h>
#define uchar unsigned char
#define uint unsigned int  
uchar rec[50];//���ͻ�����
uchar sen[50];//���ջ�����
uchar rec_flag;//rec_flag=0ʱ��ʾ��δ����ʼ֡�����ж�ɾ��
uchar end;//end=1��ʾ������� 
uint add_check;//�ۼӺͼ�����
uchar rec_length;
uchar *send_Pt;
uchar send_length;
void UART_Send_Int(uchar *pt,uchar len);//���ͺ���
void judge();//�жϽ������ݺ���
void main()
{
	TMOD=0x20;
	TH1=232;TL1=232;//������1200
	SCON=0x50;
	PCON=0x00;
	TR1=1;EA=1;ES=1;//���ж�
	P1=0xff;
	while(1)
	{
		if(end)
		{//���ջ������������
			judge();
			end=0;
		}
	}
}
void uart() interrupt 4
{
	static uchar count;//�洢�������ݵ�λ
	if(RI==1)//����
	{
		RI=0;
		rec[count++]=SBUF;
		if((rec[count-1]==0xAF)&&(rec_flag==0))//��ʼ֡����0xFA��0xAF
		{
			if(rec[count-2]==0xFA)
			{
				memset(rec,0,sizeof rec);//�����ջ�����ȫ����0
				rec[0]=0xFA;
				rec[1]=0xAF;
				count=2;
				rec_flag=1;
			}
			else
			{
				memset(rec,0,sizeof rec);//�����ջ�����ȫ����0
				count=0;
			}
		}
		else if(count>50) 	count=0;//��ֹ���ֶ�����Ч����
		if((rec[count-1]==0x0A)&&(rec[count-2])==0xA0)��
		{//�������ж�
			end=1;
			rec_length=count;
			count=0;rec_flag=0;
		}
	}
	if(TI==1)//����
	{
		TI=0;
		if(send_length>0) 
		{    
			SBUF=*send_Pt++;
			send_length--;
		}
	}
}
void UART_Send_Int(uchar *pt,uchar len)
{
	send_Pt=pt;
	send_length=len;
	send_length--;
	SBUF=*send_Pt++;	
}
void judge()
{
	uchar i;
	add_check=0;//�����������һ�ε�����
	for(i=0;i<=(rec_length-4);i++)
		add_check+=rec[i];
	if((uchar)(add_check&0x00ff)!=rec[rec_length-3])
	{//���У���Ƿ���ȷ
			strcpy(sen,"jiaoyan wrong\r\n");
			UART_Send_Int(sen,20);
	}
	else if((rec_length-6)!=rec[2])
	{//��鳤���Ƿ���ȷ
		strcpy(sen,"changdu wrong\r\n");
		UART_Send_Int(sen,20);
	}
	else 
	{
		P1=~rec[3];
		strcpy(sen,"right!!      \r\n");
		UART_Send_Int(sen,20);
		end=0;
	}
}
