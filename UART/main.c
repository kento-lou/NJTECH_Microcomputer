//帧头为0XFA,0XAF,结束符为0xA0,0X0A
#include <STC12C5A60S2.H>
#include <stdio.h>
#include <string.h>
#define uchar unsigned char
#define uint unsigned int  
uchar rec[50];//发送缓冲区
uchar sen[50];//接收缓冲区
uchar rec_flag;//rec_flag=0时表示还未对起始帧进行判断删改
uchar end;//end=1表示接收完成 
uint add_check;//累加和检查变量
uchar rec_length;
uchar *send_Pt;
uchar send_length;
void UART_Send_Int(uchar *pt,uchar len);//发送函数
void judge();//判断接受数据函数
void main()
{
	TMOD=0x20;
	TH1=232;TL1=232;//波特率1200
	SCON=0x50;
	PCON=0x00;
	TR1=1;EA=1;ES=1;//打开中断
	P1=0xff;
	while(1)
	{
		if(end)
		{//接收缓冲区配置完成
			judge();
			end=0;
		}
	}
}
void uart() interrupt 4
{
	static uchar count;//存储接受数据的位
	if(RI==1)//接收
	{
		RI=0;
		rec[count++]=SBUF;
		if((rec[count-1]==0xAF)&&(rec_flag==0))//起始帧不是0xFA，0xAF
		{
			if(rec[count-2]==0xFA)
			{
				memset(rec,0,sizeof rec);//将接收缓冲区全部清0
				rec[0]=0xFA;
				rec[1]=0xAF;
				count=2;
				rec_flag=1;
			}
			else
			{
				memset(rec,0,sizeof rec);//将接收缓冲区全部清0
				count=0;
			}
		}
		else if(count>50) 	count=0;//防止出现都是无效数据
		if((rec[count-1]==0x0A)&&(rec[count-2])==0xA0)）
		{//结束符判断
			end=1;
			rec_length=count;
			count=0;rec_flag=0;
		}
	}
	if(TI==1)//发送
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
	add_check=0;//检查变量清除上一次的数据
	for(i=0;i<=(rec_length-4);i++)
		add_check+=rec[i];
	if((uchar)(add_check&0x00ff)!=rec[rec_length-3])
	{//检查校验是否正确
			strcpy(sen,"jiaoyan wrong\r\n");
			UART_Send_Int(sen,20);
	}
	else if((rec_length-6)!=rec[2])
	{//检查长度是否正确
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
