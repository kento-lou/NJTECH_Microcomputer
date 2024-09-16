#ifndef __IAP_H__
#define __IAP_H__
#define uchar unsigned char
#define uint unsigned int
void iap_IDLE();
uchar iap_READ(uint addr);
void iap_PROGRAM(uint addr,uchar datas);
void iap_ERASE(uint addr);
#endif