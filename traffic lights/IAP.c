#include <STC12C5A60S2.H>
#include "intrins.h"
#include <stdio.h>  
#define uchar unsigned char
#define uint unsigned int
#define CMD_IDLE 0     //IAP�ȴ�
#define CMD_READ 1     //��flash
#define CMD_PROGRAM  2   //д��
#define CMD_ERASE 3    //����
#define IAP_EN 0x83

//ʹӲ�����������ֹͣ
void iap_IDLE()
{
	IAP_CONTR=0;  //��ֹIAP��д����
  IAP_CMD=0;  //�ȴ�
  IAP_TRIG=0;  //����δ����
  IAP_ADDRH=0x80;  //��λ���ݵ�ַ
  IAP_ADDRL=0;     //��λ
}
//��ȡ����
uchar iap_READ(uint addr)
{   
	uchar datas;
  IAP_CONTR=IAP_EN; //��������λ
  IAP_CMD=CMD_READ;   //д�������
  IAP_ADDRL=addr;  //�Զ����ַ
  IAP_ADDRH=addr>>8;
  IAP_TRIG=0x5a;  //������Ч
  IAP_TRIG=0xa5;
  _nop_();
	datas=IAP_DATA;
  iap_IDLE();
  return datas;  //���ض���������
}
//д��ĳ���ط�
void iap_PROGRAM(uint addr,uchar datas)
{
	IAP_CONTR=IAP_EN;
	IAP_CMD=CMD_PROGRAM;
	IAP_ADDRL=addr;
	IAP_ADDRH=addr>>8;
	IAP_DATA=datas;
	IAP_TRIG=0x5a;
	IAP_TRIG=0xa5;
	_nop_();
	iap_IDLE();
}
//����ĳ������
void iap_ERASE(uint addr)
{
	IAP_CONTR=IAP_EN;
	IAP_CMD=CMD_ERASE;
  IAP_ADDRL=addr;
  IAP_ADDRH=addr>>8;
  IAP_TRIG=0x5a;
  IAP_TRIG=0xa5;
  _nop_();
  iap_IDLE();
}    
