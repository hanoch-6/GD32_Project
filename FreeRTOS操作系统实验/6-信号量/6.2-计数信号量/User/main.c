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
#define RECEIVE_TASK_PRIO		3
//�����ջ��С	
#define RECEIVE_STK_SIZE 		50  
//������
TaskHandle_t ReceiveTask_Handler;
//������
void receive_task(void *pvParameters);

//�������ȼ�
#define SEND_TASK_PRIO		4
//�����ջ��С	
#define SEND_STK_SIZE 		50  
//������
TaskHandle_t SendTask_Handler;
//������
void send_task(void *pvParameters);


SemaphoreHandle_t CountSem_Handle =NULL;


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
	printf("FreeRTOS�����ź���ʵ��\r\n");
	printf("��λĬ��ֵΪ5��������KEY_UP���복λ������KEY1�ͷų�λ\r\n");
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
     
	/* ���� CountSem */
	CountSem_Handle = xSemaphoreCreateCounting(5,5); 
	
    //����LED1����
    xTaskCreate((TaskFunction_t )led1_task,     
                (const char*    )"led1_task",   
                (uint16_t       )LED1_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )LED1_TASK_PRIO,
                (TaskHandle_t*  )&LED1Task_Handler); 
				
	//������������
    xTaskCreate((TaskFunction_t )receive_task,     
                (const char*    )"receive_task",   
                (uint16_t       )RECEIVE_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )RECEIVE_TASK_PRIO,
                (TaskHandle_t*  )&ReceiveTask_Handler);

	//������������
    xTaskCreate((TaskFunction_t )send_task,     
                (const char*    )"send_task",   
                (uint16_t       )SEND_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )SEND_TASK_PRIO,
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

//����������
void receive_task(void *pvParameters)
{
    BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
	u8 key=0;
	
	while(1)
    {
        key=KEY_Scan(0);
		if(key==KEY1_PRESS)
		{
			xReturn = xSemaphoreTake( CountSem_Handle,0 );//��ȡ�����ź���
			if( xReturn == pdTRUE )
				printf( "KEY1�����£��ͷ�1��ͣ��λ��\r\n" );
			else
				printf( "KEY1�����£������޳�λ�����ͷţ�\r\n" );
		}
		vTaskDelay(20);
    }
}

//����������
void send_task(void *pvParameters)
{
    BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
	u8 key=0;
	
	while(1)
    {
		key=KEY_Scan(0);
		if(key==KEY_UP_PRESS)
		{
			xReturn = xSemaphoreGive( CountSem_Handle );//���������ź���
			if( xReturn == pdTRUE )
				printf("KEY_UP�����£��ɹ����뵽ͣ��λ��\r\n");
			else
				printf("KEY_UP�����£�������˼������ͣ����������\r\n");
		}
		vTaskDelay(20);
    }
}
