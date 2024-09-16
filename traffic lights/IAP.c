#include <STC12C5A60S2.H>
#include "intrins.h"
#include <stdio.h>  
#define uchar unsigned char
#define uint unsigned int
#define CMD_IDLE 0     //IAP等待
#define CMD_READ 1     //读flash
#define CMD_PROGRAM  2   //写入
#define CMD_ERASE 3    //擦除
#define IAP_EN 0x83

//使硬件或软件操作停止
void iap_IDLE()
{
	IAP_CONTR=0;  //禁止IAP读写擦除
  IAP_CMD=0;  //等待
  IAP_TRIG=0;  //命令未触发
  IAP_ADDRH=0x80;  //高位数据地址
  IAP_ADDRL=0;     //低位
}
//读取数据
uchar iap_READ(uint addr)
{   
	uchar datas;
  IAP_CONTR=IAP_EN; //控制允许位
  IAP_CMD=CMD_READ;   //写入读命令
  IAP_ADDRL=addr;  //自定义地址
  IAP_ADDRH=addr>>8;
  IAP_TRIG=0x5a;  //命令有效
  IAP_TRIG=0xa5;
  _nop_();
	datas=IAP_DATA;
  iap_IDLE();
  return datas;  //返回读出的数据
}
//写到某个地方
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
//擦掉某个区域
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
