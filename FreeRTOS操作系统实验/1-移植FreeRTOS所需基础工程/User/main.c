#include "system.h"
#include "SysTick.h"
#include "led.h"
#include "usart.h"


/*******************************************************************************
* �� �� ��         : main
* ��������		   : ������
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
int main()
{
	u8 i=0; 
	u16 data=1234;
	float fdata=12.34;
	char str[]="Hello World!";	
	SysTick_Init(72);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //�ж����ȼ����� ��2��
	LED_Init();
	USART1_Init(115200);
	
	while(1)
	{
		i++;
		if(i%50==0)
		{
			LED1=!LED1;
			
			printf("���������data=%d\r\n",data);
			printf("�����������fdata=%0.2f\r\n",fdata);
			printf("���ʮ��������data=%X\r\n",data);
			printf("����˽�����data=%o\r\n",data);
			printf("����ַ���str=%s\r\n",str);
			
		}
		delay_ms(10);
	}
}
