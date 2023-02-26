#include "system.h"
#include "SysTick.h"
#include "led.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "key.h"
#include "timers.h"


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


TimerHandle_t Swtmr1_Handle =NULL;
TimerHandle_t Swtmr2_Handle =NULL;

static uint32_t TmrCb_Count1 = 0; /* ��¼�����ʱ��1�ص�����ִ�д��� */
static uint32_t TmrCb_Count2 = 0; /* ��¼�����ʱ��2�ص�����ִ�д��� */

static void Swtmr1_Callback(void* parameter);
static void Swtmr2_Callback(void* parameter);

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
	printf("FreeRTOS�����ʱ��ʵ��\r\n");
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
    
	//������ʱ��1
	Swtmr1_Handle=xTimerCreate((const char*		)"AutoReloadTimer",
                            (TickType_t			)1000,/* ��ʱ������ 1000(tick) */
                            (UBaseType_t		)pdTRUE,/* ����ģʽ */
                            (void*				  )1,/* Ϊÿ����ʱ������һ��������ΨһID */
                            (TimerCallbackFunction_t)Swtmr1_Callback); 
	if(Swtmr1_Handle != NULL) 
	{
		xTimerStart(Swtmr1_Handle,0);	//�������ڶ�ʱ��
	}
	
	//������ʱ��2
	Swtmr2_Handle=xTimerCreate((const char*			)"OneShotTimer",
                             (TickType_t			)5000,/* ��ʱ������ 5000(tick) */
                             (UBaseType_t			)pdFALSE,/* ����ģʽ */
                             (void*					  )2,/* Ϊÿ����ʱ������һ��������ΨһID */
                             (TimerCallbackFunction_t)Swtmr2_Callback); 
	if(Swtmr2_Handle != NULL) 
	{
		xTimerStart(Swtmr2_Handle,0);	//�������ڶ�ʱ��
	}
							 
    //����LED1����
    xTaskCreate((TaskFunction_t )led1_task,     
                (const char*    )"led1_task",   
                (uint16_t       )LED1_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )LED1_TASK_PRIO,
                (TaskHandle_t*  )&LED1Task_Handler); 
			
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

//��ʱ��1�ص�����
//�����ʱ����Ҫ��������������Ҳ��Ҫ������ѭ����Ӧ������
void Swtmr1_Callback(void* parameter)
{		
	TickType_t tick_num1;

	TmrCb_Count1++;						/* ÿ�ص�һ�μ�һ */

	tick_num1 = xTaskGetTickCount();	/* ��ȡ�δ�ʱ���ļ���ֵ */

	LED2=!LED2;

	printf("Swtmr1_Callback����ִ�� %d ��\n", TmrCb_Count1);
	printf("�δ�ʱ����ֵ=%d\n", tick_num1);
}

//��ʱ��2�ص�����
//�����ʱ����Ҫ��������������Ҳ��Ҫ������ѭ����Ӧ������
void Swtmr2_Callback(void* parameter)
{		
	TickType_t tick_num2;

	TmrCb_Count2++;						/* ÿ�ص�һ�μ�һ */

	tick_num2 = xTaskGetTickCount();	/* ��ȡ�δ�ʱ���ļ���ֵ */

	printf("Swtmr2_Callback����ִ�� %d ��\n", TmrCb_Count2);
	printf("�δ�ʱ����ֵ=%d\n", tick_num2);
}
