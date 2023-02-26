#include "system.h"
#include "SysTick.h"
#include "led.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "key.h"
#include "semphr.h"


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
#define LowPriority_TASK_PRIO		3
//�����ջ��С	
#define LowPriority_STK_SIZE 		512  
//������
TaskHandle_t LowPriority_Task_Handle;
//������
void LowPriority_Task(void *pvParameters);

//�������ȼ�
#define MidPriority_TASK_PRIO		4
//�����ջ��С	
#define MidPriority_STK_SIZE 		512  
//������
TaskHandle_t MidPriority_Task_Handle;
//������
void MidPriority_Task(void *pvParameters);

//�������ȼ�
#define HighPriority_TASK_PRIO		5
//�����ջ��С	
#define HighPriority_STK_SIZE 		512  
//������
TaskHandle_t HighPriority_Task_Handle;
//������
void HighPriority_Task(void *pvParameters);


SemaphoreHandle_t BinarySem_Handle =NULL;


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
	printf("FreeRTOS���ȼ���תʵ��\r\n");
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
    BaseType_t xReturn = pdPASS;
	taskENTER_CRITICAL();           //�����ٽ���
     
	/* ���� BinarySem */
	BinarySem_Handle = xSemaphoreCreateBinary();
	xReturn = xSemaphoreGive( BinarySem_Handle );//������ֵ�ź���	
	
    //����LED1����
    xTaskCreate((TaskFunction_t )led1_task,     
                (const char*    )"led1_task",   
                (uint16_t       )LED1_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )LED1_TASK_PRIO,
                (TaskHandle_t*  )&LED1Task_Handler); 
				
	//���������ȼ�����
    xTaskCreate((TaskFunction_t )LowPriority_Task,     
                (const char*    )"LowPriority_Task",   
                (uint16_t       )LowPriority_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )LowPriority_TASK_PRIO,
                (TaskHandle_t*  )&LowPriority_Task_Handle);

	//���������ȼ�����
    xTaskCreate((TaskFunction_t )MidPriority_Task,     
                (const char*    )"MidPriority_Task",   
                (uint16_t       )MidPriority_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )MidPriority_TASK_PRIO,
                (TaskHandle_t*  )&MidPriority_Task_Handle);
				
	//���������ȼ�����
    xTaskCreate((TaskFunction_t )HighPriority_Task,     
                (const char*    )"HighPriority_Task",   
                (uint16_t       )HighPriority_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )HighPriority_TASK_PRIO,
                (TaskHandle_t*  )&HighPriority_Task_Handle);
				
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

//�����ȼ�������
void LowPriority_Task(void *pvParameters)
{
    BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
	static uint32_t i=0;
	
	while(1)
    {
        printf("LowPriority_Task ��ȡ�ź���\n");
		//��ȡ��ֵ�ź��� xSemaphore,û��ȡ����һֱ�ȴ�
		xReturn = xSemaphoreTake(BinarySem_Handle,/* ��ֵ�ź������ */
							  portMAX_DELAY); /* �ȴ�ʱ�� */
		if( xReturn == pdTRUE )
			printf("LowPriority_Task Running\n\n");

		for(i=0;i<2000000;i++)//ģ������ȼ�����ռ���ź���
		{
			taskYIELD();//�����������
		}

		printf("LowPriority_Task �ͷ��ź���!\r\n");
		xReturn = xSemaphoreGive( BinarySem_Handle );//������ֵ�ź���
		LED2=!LED2;
		vTaskDelay(500);
    }
}

//�����ȼ�������
void MidPriority_Task(void *pvParameters)
{
	while(1)
    {
		printf("MidPriority_Task Running\n");
		vTaskDelay(500);
    }
}

//�����ȼ�������
void HighPriority_Task(void *pvParameters)
{
	BaseType_t xReturn = pdTRUE;
	
	while(1)
    {
		printf("HighPriority_Task ��ȡ�ź���\n");
		//��ȡ��ֵ�ź��� xSemaphore,û��ȡ����һֱ�ȴ�
		xReturn = xSemaphoreTake(BinarySem_Handle,/* ��ֵ�ź������ */
                              portMAX_DELAY); /* �ȴ�ʱ�� */
		if(pdTRUE == xReturn)
			printf("HighPriority_Task Running\n");
		LED2=!LED2;
		xReturn = xSemaphoreGive( BinarySem_Handle );//������ֵ�ź���
		vTaskDelay(500);
    }
}
