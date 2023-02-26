#include "system.h"
#include "SysTick.h"
#include "led.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "key.h"
#include "time.h"


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
#define Interrupt_TASK_PRIO		4
//�����ջ��С	
#define Interrupt_STK_SIZE 		512  
//������
TaskHandle_t InterruptTask_Handler;
//������
void Interrupt_task(void *pvParameters);


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
	TIM3_Init(10000-1,7200-1);//��ʱ1S
	TIM4_Init(10000-1,7200-1);
	printf("FreeRTOS�жϹ���ʵ��\r\n");
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
	
	//����Interrupt����
    xTaskCreate((TaskFunction_t )Interrupt_task,     
                (const char*    )"Interrupt_task",   
                (uint16_t       )Interrupt_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )Interrupt_TASK_PRIO,
                (TaskHandle_t*  )&InterruptTask_Handler);
	
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

//Interrupt������
void Interrupt_task(void *pvParameters)
{
    static u32 total_num=0;
	
	while(1)
    {
		total_num+=1; 
		if(total_num==5)
		{
			printf("�ر��ж�.............\r\n");
			portDISABLE_INTERRUPTS(); //�ر��ж�
			delay_xms(5000); //��ʱ5s��ʹ�ò�Ӱ��������ȵ���ʱ
			printf("���ж�.............\r\n"); //���ж�
			portENABLE_INTERRUPTS();
		}
		LED2=!LED2;
		vTaskDelay(1000);
    }
}
