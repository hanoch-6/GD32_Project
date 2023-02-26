#include "system.h"
#include "SysTick.h"
#include "led.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "key.h"
#include "limits.h"

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
#define KEY_TASK_PRIO		4
//�����ջ��С	
#define KEY_STK_SIZE 		512  
//������
TaskHandle_t KEYTask_Handler;
//������
void key_task(void *pvParameters);


uint8_t *Test_Ptr = NULL;


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
	printf("FreeRTOS�ڴ����ʵ��\r\n");
	printf("����KEY1�����ڴ棬����KEY2�ͷ��ڴ�\n");
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

//KEY������
void key_task(void *pvParameters)
{
    u8 key=0;
	uint32_t g_memsize;
	
	while(1)
    {
        key=KEY_Scan(0);
		if(key==KEY1_PRESS)
		{
			if(NULL == Test_Ptr)
			{
				/* ��ȡ��ǰ�ڴ��С */
				g_memsize = xPortGetFreeHeapSize();
				printf("ϵͳ��ǰ�ڴ��СΪ %d �ֽڣ���ʼ�����ڴ�\n",g_memsize);
				Test_Ptr = pvPortMalloc(1024);
				if(NULL != Test_Ptr)
				{
					printf("�ڴ�����ɹ�\n");
					printf("���뵽���ڴ��ַΪ%#x\n",(int)Test_Ptr);

					/* ��ȡ��ǰ��ʣ����С */
					g_memsize = xPortGetFreeHeapSize();
					printf("ϵͳ��ǰ�ڴ�ʣ����СΪ %d �ֽ�\n",g_memsize);
						  
					//��Test_Ptr��д�뵱����:��ǰϵͳʱ��
					sprintf((char*)Test_Ptr,"��ǰϵͳTickCount = %d \n",xTaskGetTickCount());
					printf("д��������� %s \n",(char*)Test_Ptr);
				}
			}
			else
				printf("���Ȱ���KEY2�ͷ��ڴ�������\n");
		}
		
		if(key==KEY2_PRESS)
		{
			if(NULL != Test_Ptr)
			{
				printf("�ͷ��ڴ�\n");
				vPortFree(Test_Ptr);//�ͷ��ڴ�
				Test_Ptr=NULL;
				/* ��ȡ��ǰ��ʣ����С */
				g_memsize = xPortGetFreeHeapSize();
				printf("ϵͳ��ǰ�ڴ��СΪ %d �ֽڣ��ڴ��ͷ����\n",g_memsize);
			}
			else
				printf("���Ȱ���KEY1�����ڴ����ͷ�\n");
		}
		vTaskDelay(20);
    }
}
