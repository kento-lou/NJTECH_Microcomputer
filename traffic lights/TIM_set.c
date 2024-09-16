#include <STC12C5A60S2.H>
/**
   * @brief 定时器0设置，50毫秒@11.0592MHz
   * @param 无
   * @retval 无
   */
void Timer0_Init(void)	
{
	TMOD &= 0xF0;			
	TMOD |= 0x01;			
	TL0 = 0x00;				
	TH0 = 0x4C;				
	TF0 = 0;				
	TR0 = 1;				
	ET0=1;
}
