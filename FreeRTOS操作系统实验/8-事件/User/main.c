#include "system.h"
#include "SysTick.h"
#include "led.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "key.h"
#include "event_groups.h"


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
#define KEY_TASK_PRIO		4
//�����ջ��С	
#define KEY_STK_SIZE 		512  
//������
TaskHandle_t KEYTask_Handler;
//������
void key_task(void *pvParameters);


EventGroupHandle_t Event_Handle =NULL;
#define KEY1_EVENT  (0x01 << 0)//�����¼������λ0
#define KEY2_EVENT  (0x01 << 1)//�����¼������λ1


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
	printf("FreeRTOS�¼���־��ʵ��\r\n");
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
     
	/* ���� Event_Handle */
	Event_Handle = xEventGroupCreate();	
	
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
				
	//����KEY����
    xTaskCreate((TaskFunction_t )key_task,     
                (const char*    )"key_task",   
                (uint16_t       )KEY_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )KEY_TASK_PRIO,
                (TaskHandle_t*  )&KEYTask_Handler);
				
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
    EventBits_t r_event;  /* ����һ���¼����ձ��� */
	
	while(1)
    {
        r_event = xEventGroupWaitBits(Event_Handle,  /* �¼������� */
                                  KEY1_EVENT|KEY2_EVENT,/* �����̸߳���Ȥ���¼� */
                                  pdTRUE,   /* �˳�ʱ����¼�λ */
                                  pdTRUE,   /* �������Ȥ�������¼� */
                                  portMAX_DELAY);/* ָ����ʱ�¼�,һֱ�� */
                        
		if((r_event & (KEY1_EVENT|KEY2_EVENT)) == (KEY1_EVENT|KEY2_EVENT))
		{
			printf ( "KEY1��KEY2������\n");
			LED2=!LED2;
		}
		else
			printf ("�¼�����\n");
    }
}

//KEY������
void key_task(void *pvParameters)
{
    u8 key=0;
	
	while(1)
    {
        key=KEY_Scan(0);
		if(key==KEY1_PRESS)
		{
			printf ( "KEY1������\n" );
			/* ����һ���¼�1 */
			xEventGroupSetBits(Event_Handle,KEY1_EVENT);
		}
		else if(key==KEY2_PRESS)
		{
			printf ( "KEY2������\n" );	
			/* ����һ���¼�2 */
			xEventGroupSetBits(Event_Handle,KEY2_EVENT);
		}
		vTaskDelay(20);
    }
}
