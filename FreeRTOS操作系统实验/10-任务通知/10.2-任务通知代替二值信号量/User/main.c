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
#define Receive1_TASK_PRIO		3
//�����ջ��С	
#define Receive1_STK_SIZE 		512  
//������
TaskHandle_t Receive1Task_Handler;
//������
void Receive1_task(void *pvParameters);

//�������ȼ�
#define Receive2_TASK_PRIO		4
//�����ջ��С	
#define Receive2_STK_SIZE 		512  
//������
TaskHandle_t Receive2Task_Handler;
//������
void Receive2_task(void *pvParameters);

//�������ȼ�
#define Send_TASK_PRIO		5
//�����ջ��С	
#define Send_STK_SIZE 		512  
//������
TaskHandle_t SendTask_Handler;
//������
void Send_task(void *pvParameters);



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
	printf("FreeRTOS����֪ͨ�����ֵ�ź���ʵ��\r\n");
	printf("����KEY_UP����KEY1����������������ͬ��\n");
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
	
	//����Receive1����
    xTaskCreate((TaskFunction_t )Receive1_task,     
                (const char*    )"Receive1_task",   
                (uint16_t       )Receive1_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )Receive1_TASK_PRIO,
                (TaskHandle_t*  )&Receive1Task_Handler);
				
	//����Receive2����
    xTaskCreate((TaskFunction_t )Receive2_task,     
                (const char*    )"Receive2_task",   
                (uint16_t       )Receive2_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )Receive2_TASK_PRIO,
                (TaskHandle_t*  )&Receive2Task_Handler);
	
	//����Send����
    xTaskCreate((TaskFunction_t )Send_task,     
                (const char*    )"Send_task",   
                (uint16_t       )Send_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )Send_TASK_PRIO,
                (TaskHandle_t*  )&SendTask_Handler);
			
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

//Receive1������
void Receive1_task(void *pvParameters)
{
	while(1)
    {
		//��ȡ����֪ͨ ,û��ȡ����һֱ�ȴ�
		ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
		printf("Receive1_Task ����֪ͨ��ȡ�ɹ�!\n\n");
	}
}

//Receive2������
void Receive2_task(void *pvParameters)
{	
	while(1)
    {
		//��ȡ����֪ͨ ,û��ȡ����һֱ�ȴ�
		ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
		printf("Receive2_Task ����֪ͨ��ȡ�ɹ�!\n\n");
	}
}

//Send������
void Send_task(void *pvParameters)
{
	BaseType_t xReturn = pdPASS;
	u8 key=0;
	
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY_UP_PRESS)
		{
			xReturn = xTaskNotifyGive(Receive1Task_Handler);
			if( xReturn == pdTRUE )
				printf("Receive1_Task_Handle ����֪ͨ���ͳɹ�!\r\n");
		}
		else if(key==KEY1_PRESS)
		{
			xReturn = xTaskNotifyGive(Receive2Task_Handler);
			if( xReturn == pdTRUE )
				printf("Receive2_Task_Handle ����֪ͨ���ͳɹ�!\r\n");
		}
		vTaskDelay(20);
	}
}
