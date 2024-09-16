#include <STC12C5A60S2.H>
#include "intrins.h"
/**
   * @brief 11.0592MHz晶振延时函数
   * @param	延时长度
   * @retval 无
   */
void Delayms(unsigned int ms)		
{
	unsigned char data i, j;
	while(ms--)
	{
		_nop_();
		i = 11;
		j = 190;
		do
		{
			while (--j);
		} while (--i);
	}
}
