#include <STC12C5A60S2.H>
/**
   * @brief ��ʱ��0���ã�50����@11.0592MHz
   * @param ��
   * @retval ��
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
