#include "system.h"
#include "SysTick.h"
#include "led.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "key.h"
#include "time.h"
#include "string.h"


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


//�������ȼ�
#define CPU_TASK_PRIO		4
//�����ջ��С	
#define CPU_STK_SIZE 		512  
//������
TaskHandle_t CPUTask_Handler;
//������
void CPU_task(void *pvParameters);


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
	TIM6_Init(100-1,72-1);//��ʱ0.1ms
	printf("FreeRTOS CPU������ͳ��\r\n");
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
	
	//����CPU����
    xTaskCreate((TaskFunction_t )CPU_task,     
                (const char*    )"CPU_task",   
                (uint16_t       )CPU_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )CPU_TASK_PRIO,
                (TaskHandle_t*  )&CPUTask_Handler);
	
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
		printf("LED1_Task Running,LED2_ON\r\n");
        LED1=1;
        vTaskDelay(800);
		printf("LED1_Task Running,LED2_OFF\r\n");
    }
}

//LED2������
void led2_task(void *pvParameters)
{
    while(1)
    {
        LED2=0;
        vTaskDelay(800);
		printf("LED2_Task Running,LED2_ON\r\n");
        LED2=1;
        vTaskDelay(200);
		printf("LED2_Task Running,LED2_OFF\r\n");
    }
}

//CPU������
void CPU_task(void *pvParameters)
{
    uint8_t CPU_RunInfo[400];//������������ʱ����Ϣ
	
	while(1)
    {
		memset(CPU_RunInfo,0,400);//��Ϣ����������
		vTaskList((char *)&CPU_RunInfo);  //��ȡ��������ʱ����Ϣ
		
		printf("---------------------------------------------\r\n");
		printf("������      ����״̬ ���ȼ�   ʣ��ջ �������\r\n");
		printf("%s", CPU_RunInfo);
		printf("---------------------------------------------\r\n");

		memset(CPU_RunInfo,0,400);				//��Ϣ����������

		vTaskGetRunTimeStats((char *)&CPU_RunInfo);

		printf("������       ���м���         ������\r\n");
		printf("%s", CPU_RunInfo);
		printf("---------------------------------------------\r\n\n");
		vTaskDelay(1000);   /* ��ʱ1000��tick */
    }
}
