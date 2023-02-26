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
//�����ջ
StackType_t StartTaskStack[START_STK_SIZE];
//������ƿ�
StaticTask_t StartTaskTCB;
//������
TaskHandle_t StartTask_Handler;
//������
void start_task(void *pvParameters);


//�������ȼ�
#define TASK1_TASK_PRIO		2
//�����ջ��С	
#define TASK1_STK_SIZE 		128  
//�����ջ
StackType_t Task1TaskStack[TASK1_STK_SIZE];
//������ƿ�
StaticTask_t Task1TaskTCB;
//������
TaskHandle_t Task1Task_Handler;
//������
void task1_task(void *pvParameters);


//�������ȼ�
#define TASK2_TASK_PRIO		3
//�����ջ��С	
#define TASK2_STK_SIZE 		128 
//�����ջ
StackType_t Task2TaskStack[TASK2_STK_SIZE];
//������ƿ�
StaticTask_t Task2TaskTCB;
//������
TaskHandle_t Task2Task_Handler;
//������
void task2_task(void *pvParameters);


/* �������������ջ */
static StackType_t Idle_Task_Stack[configMINIMAL_STACK_SIZE];
/* ��ʱ�������ջ */
static StackType_t Timer_Task_Stack[configTIMER_TASK_STACK_DEPTH];

/* ����������ƿ� */
static StaticTask_t Idle_Task_TCB;	
/* ��ʱ��������ƿ� */
static StaticTask_t Timer_Task_TCB;

//��ȡ��������������ջ��������ƿ��ڴ棬��Ϊ������ʹ�õ�
//��̬�ڴ棬��˿�������������ջ��������ƿ���ڴ��Ӧ��
//���û����ṩ��FreeRTOS�ṩ�˽ӿں���vApplicationGetIdleTaskMemory()
//ʵ�ִ˺������ɡ�
//ppxIdleTaskTCBBuffer:������ƿ��ڴ�
//ppxIdleTaskStackBuffer:�����ջ�ڴ�
//pulIdleTaskStackSize:�����ջ��С
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, 
								   StackType_t **ppxIdleTaskStackBuffer, 
								   uint32_t *pulIdleTaskStackSize)
{
	*ppxIdleTaskTCBBuffer=&Idle_Task_TCB;
	*ppxIdleTaskStackBuffer=Idle_Task_Stack;
	*pulIdleTaskStackSize=configMINIMAL_STACK_SIZE;
}

//��ȡ��ʱ������������ջ��������ƿ��ڴ�
//ppxTimerTaskTCBBuffer	:		������ƿ��ڴ�
//ppxTimerTaskStackBuffer:	�����ջ�ڴ�
//pulTimerTaskStackSize	:		�����ջ��С
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, 
									StackType_t **ppxTimerTaskStackBuffer, 
									uint32_t *pulTimerTaskStackSize)
{
	*ppxTimerTaskTCBBuffer=&Timer_Task_TCB;/* ������ƿ��ڴ� */
	*ppxTimerTaskStackBuffer=Timer_Task_Stack;/* �����ջ�ڴ� */
	*pulTimerTaskStackSize=configTIMER_TASK_STACK_DEPTH;/* �����ջ��С */
}


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
	StartTask_Handler=xTaskCreateStatic((TaskFunction_t	)start_task,		//������
										(const char* 	)"start_task",		//��������
										(uint32_t 		)START_STK_SIZE,	//�����ջ��С
										(void* 		  	)NULL,				//���ݸ��������Ĳ���
										(UBaseType_t 	)START_TASK_PRIO, 	//�������ȼ�
										(StackType_t*   )StartTaskStack,	//�����ջ
										(StaticTask_t*  )&StartTaskTCB);	//������ƿ�              
    vTaskStartScheduler();          //�����������
}

//��ʼ����������
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���
    //����TASK1����
	Task1Task_Handler=xTaskCreateStatic((TaskFunction_t	)task1_task,		
										(const char* 	)"task1_task",		
										(uint32_t 		)TASK1_STK_SIZE,	
										(void* 		  	)NULL,				
										(UBaseType_t 	)TASK1_TASK_PRIO, 	
										(StackType_t*   )Task1TaskStack,	
										(StaticTask_t*  )&Task1TaskTCB);	
    //����TASK2����
	Task2Task_Handler=xTaskCreateStatic((TaskFunction_t	)task2_task,		
										(const char* 	)"task2_task",		
										(uint32_t 		)TASK2_STK_SIZE,	
										(void* 		  	)NULL,				
										(UBaseType_t 	)TASK2_TASK_PRIO, 	
										(StackType_t*   )Task2TaskStack,	
										(StaticTask_t*  )&Task2TaskTCB);
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

//����1����
void task1_task(void *pvParameters)
{
    while(1)
    {
        LED1=0;
        vTaskDelay(200);
        LED1=1;
        vTaskDelay(800);
    }
}

//����2����
void task2_task(void *pvParameters)
{
    while(1)
    {
        LED2=0;
        vTaskDelay(800);
        LED2=1;
        vTaskDelay(200);
    }
}
