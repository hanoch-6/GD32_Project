#include "system.h"
#include "SysTick.h"
#include "led.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "key.h"


//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С	
#define START_STK_SIZE 		128  
//������
TaskHandle_t StartTask_Handler;
//������
void start_task(void *pvParameters);

//�������ȼ�
#define LED1_TASK_PRIO		2
//�����ջ��С	
#define LED1_STK_SIZE 		50  
//������
TaskHandle_t LED1Task_Handler;
//������
void led1_task(void *pvParameters);

//�������ȼ�
#define Take_TASK_PRIO		3
//�����ջ��С	
#define Take_STK_SIZE 		512  
//������
TaskHandle_t TakeTask_Handler;
//������
void Take_task(void *pvParameters);

//�������ȼ�
#define Give_TASK_PRIO		4
//�����ջ��С	
#define Give_STK_SIZE 		512  
//������
TaskHandle_t GiveTask_Handler;
//������
void Give_task(void *pvParameters);



/*******************************************************************************
* �� �� ��         : main
* ��������		   : ������
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
int main()
{
	SysTick_Init(72);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
	LED_Init();
	KEY_Init();
	USART1_Init(115200);
	printf("FreeRTOS����֪ͨ��������ź���ʵ��\r\n");
	printf("��λĬ��ֵΪ0��������KEY1���복λ������KEY2�ͷų�λ��\n\n");
	//������ʼ����
    xTaskCreate((TaskFunction_t )start_task,            //������
                (const char*    )"start_task",          //��������
                (uint16_t       )START_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);   //������              
    vTaskStartScheduler();          //�����������
}

//��ʼ����������
void start_task(void *pvParameters)
{
	taskENTER_CRITICAL();           //�����ٽ���
							 
    //����LED1����
    xTaskCreate((TaskFunction_t )led1_task,     
                (const char*    )"led1_task",   
                (uint16_t       )LED1_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )LED1_TASK_PRIO,
                (TaskHandle_t*  )&LED1Task_Handler); 
	
	//����Take����
    xTaskCreate((TaskFunction_t )Take_task,     
                (const char*    )"Take_task",   
                (uint16_t       )Take_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )Take_TASK_PRIO,
                (TaskHandle_t*  )&TakeTask_Handler);
				
	//����Give����
    xTaskCreate((TaskFunction_t )Give_task,     
                (const char*    )"Give_task",   
                (uint16_t       )Give_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )Give_TASK_PRIO,
                (TaskHandle_t*  )&GiveTask_Handler);
	
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
} 

//LED1������
void led1_task(void *pvParameters)
{
    while(1)
    {
        LED1=0;
        vTaskDelay(200);
        LED1=1;
        vTaskDelay(800);
    }
}

//Give������
void Give_task(void *pvParameters)
{	
	BaseType_t xReturn = pdPASS;
	u8 key=0;
	
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY2_PRESS)
		{
			xTaskNotifyGive(TakeTask_Handler);//��������֪ͨ
			if ( pdPASS == xReturn ) 
				printf( "KEY2�����£��ͷ�1��ͣ��λ��\n" );
		}
		vTaskDelay(20);
	}
}

//Take������
void Take_task(void *pvParameters)
{
	uint32_t take_num = pdTRUE;
	u8 key=0;
	
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY1_PRESS)
		{
			//��ȡ����֪ͨ ,û��ȡ���򲻵ȴ�
			take_num=ulTaskNotifyTake(pdFALSE,0);
			if(take_num > 0)
				printf( "KEY1�����£��ɹ����뵽ͣ��λ����ǰ��λΪ %d \n", take_num - 1);
			else
				printf( "KEY1�����£���λ�Ѿ�û���ˣ��밴KEY2�ͷų�λ\n" );  
		}
		vTaskDelay(20);
	}
}
