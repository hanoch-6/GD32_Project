#include "system.h"
#include "SysTick.h"
#include "led.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"


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
#define LED2_TASK_PRIO		3
//�����ջ��С	
#define LED2_STK_SIZE 		50  
//������
TaskHandle_t LED2Task_Handler;
//������
void led2_task(void *pvParameters);


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
	USART1_Init(115200);
	
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
				
	//����LED2����
    xTaskCreate((TaskFunction_t )led2_task,     
                (const char*    )"led2_task",   
                (uint16_t       )LED2_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )LED2_TASK_PRIO,
                (TaskHandle_t*  )&LED2Task_Handler); 
				
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

//LED2������
void led2_task(void *pvParameters)
{
    while(1)
    {
        LED2=0;
        vTaskDelay(800);
        LED2=1;
        vTaskDelay(200);
    }
}
