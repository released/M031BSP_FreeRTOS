/******************************************************************************
 * @file     main.c
 * @version  V1.00
 * @brief    A project template for M031 MCU.
 *
 * Copyright (C) 2017 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "NuMicro.h"

#include "FreeRTOS.h"
#include "Task.h"
#include "Queue.h"
#include "semphr.h"

#include "cpu_utils.h"

#define mainNORMAL_TASK_PRIORITY           ( tskIDLE_PRIORITY + 0UL )
#define mainABOVENORMAL_TASK_PRIORITY     ( tskIDLE_PRIORITY + 1UL )
#define mainHIGH_TASK_PRIORITY             	( tskIDLE_PRIORITY + 2UL )

#define	DELAY_MS_1							(1)
#define	DELAY_MS_10						(10)
#define	DELAY_MS_100						(100)
#define	DELAY_MS_250						(250)
#define	DELAY_MS_1000						(1000)

unsigned char 								Flag_Func = 0;
TaskHandle_t									xTaskCreateSub;
unsigned int 									Cnt_Packet = 0;

unsigned char 								BitFlag = 0;
#define BitFlag_ON(flag)						(BitFlag|=flag)
#define BitFlag_OFF(flag)						(BitFlag&=~flag)
#define BitFlag_READ(flag)						((BitFlag&flag)?1:0)
#define ReadBit(bit)							(uint8_t)(1<<bit)

typedef struct
{
	unsigned int var_a;
	unsigned int var_b;
}PacketStructA;

typedef struct
{
	unsigned int var_a;
	unsigned int var_b;
}PacketStructB;

typedef struct
{
	unsigned int var_a;
	unsigned int var_b;
	unsigned int var_c;	
	float var_d;
}PacketStructC;

typedef enum
{ 
	Type_A = 0,
	Type_B,
	Type_C,  

	Type_NULL,
	
}PacketType;

typedef struct
{
	PacketStructA Struct_A;
	PacketStructB Struct_B;
	PacketStructC Struct_C;
}PacketStructX;

typedef struct
{
	PacketType type;
	PacketStructX structutre;	
}CommPacket;

typedef enum
{ 
 	BitFlag_AAA = 0,
 	BitFlag_BBB,
 	BitFlag_CCC,
 	BitFlag_DDD,
 	BitFlag_EEE,
	
}BitFlagType;

#define printf(...)	\
{	\
	vTaskSuspendAll();	\
	printf(__VA_ARGS__);	\
	fflush(stdout); \
	xTaskResumeAll();	\
}	\

//#define DEBUG_BIT_FLAG
//#define DEBUG_DISPLAY_TASKLIST
//#define DEBUG_RECEIVE_PACKET
//#define DEBUG_SEND_PACKET
//#define DEBUG_CREATE_TASK
//#define DEBUG_RISING_FLAG

#define DEBUG_LOG_MSG_500ms
//#define DEBUG_LOG_MSG_5s
//#define DEBUG_LOG_MSG_1s

CommPacket gPacket =
{
	Type_NULL,
	{
		{1,1},
		{2,2},
		{3,3,3,3}
	}
};

void SendPacket(CommPacket* pData, unsigned int iLen)
{
	switch(pData->type)
	{
		case Type_A:
			pData->structutre.Struct_A.var_a += 1;
			pData->structutre.Struct_A.var_b += 1;			
			break;
			
		case Type_B:
			pData->structutre.Struct_B.var_a += 10;
			pData->structutre.Struct_B.var_b += 10;	
			break;
			
		case Type_C:
			pData->structutre.Struct_C.var_a += 100;
			pData->structutre.Struct_C.var_b += 100;
			pData->structutre.Struct_C.var_c += 100;			
			break;
	}
}

unsigned int RecvPacket(CommPacket* pData, unsigned int iLen)
{

	memcpy(pData,&gPacket,sizeof(gPacket));
	
	return 0;
}


void BitFlag_SwitchTest(uint8_t On)
{
	//BitFlag TEST
	if (On)
	{
		BitFlag_ON(ReadBit(BitFlag_AAA));
		BitFlag_OFF(ReadBit(BitFlag_BBB));
		BitFlag_OFF(ReadBit(BitFlag_CCC));
		BitFlag_OFF(ReadBit(BitFlag_DDD));
		BitFlag_OFF(ReadBit(BitFlag_EEE));
	}
	else
	{
		BitFlag_OFF(ReadBit(BitFlag_AAA));
		BitFlag_ON(ReadBit(BitFlag_BBB));
		BitFlag_ON(ReadBit(BitFlag_CCC));
		BitFlag_ON(ReadBit(BitFlag_DDD));
		BitFlag_ON(ReadBit(BitFlag_EEE));
	}
}

void vTaskBitFlagSwitch( void * pvParameters )
{
	static uint8_t OnOff = 0;
	uint32_t millisec = DELAY_MS_1000;	
	portTickType xLastWakeTime;
//	Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();

	(void) pvParameters;
	for( ;; )
	{
		// Wait for the next cycle.
		vTaskDelayUntil( &xLastWakeTime, ( millisec *configTICK_RATE_HZ / ( ( TickType_t ) 1000 ) ));

		// Perform action here.	
		OnOff = (OnOff&1)?(0):(1);
		BitFlag_SwitchTest(OnOff);		
		
		#if defined (DEBUG_BIT_FLAG)
		printf("\r\n\r\n>>>>BitFlagSwitch (0x%2X)!\r\n",BitFlag);
		printf("0x%2X\r\n",BitFlag_READ(ReadBit(BitFlag_AAA)));		
		printf("0x%2X\r\n",BitFlag_READ(ReadBit(BitFlag_BBB)));
		printf("0x%2X\r\n",BitFlag_READ(ReadBit(BitFlag_CCC)));	
		printf("0x%2X\r\n",BitFlag_READ(ReadBit(BitFlag_DDD)));	
		printf("0x%2X\r\n\r\n\r\n",BitFlag_READ(ReadBit(BitFlag_EEE)));	
		#endif	/*DEBUG_13_PWM_MODULATION*/	
	}  
}

void vTaskTasklist( void * pvParameters )
{
	char buff[512];
	
	uint32_t millisec = DELAY_MS_1000;
	portTickType xLastWakeTime;
//	Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();

	(void) pvParameters;
	for( ;; )
	{
		// Wait for the next cycle.
		vTaskDelayUntil( &xLastWakeTime, ( millisec *configTICK_RATE_HZ / ( ( TickType_t ) 1000 ) ));
		
		// Perform action here.
		vTaskList(buff);

		#if defined (DEBUG_DISPLAY_TASKLIST)
		printf("\r\n>>>>Tasklist 	State Priority Stack   Num\r\n");
		printf("-------------------------------------------\r\n");
		printf("%s\r\n\r\n\r\n\r\n" ,buff);	
		#endif	/*DEBUG_DISPLAY_TASKLIST*/
	}  
}

void vTaskRecvPacket( void * pvParameters )
{
	CommPacket lPacket;
	
	uint32_t millisec = DELAY_MS_100*5;	
	portTickType xLastWakeTime;
//	Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();

	(void) pvParameters;	
	for( ;; )
	{
		// Wait for the next cycle.
		vTaskDelayUntil( &xLastWakeTime, ( millisec *configTICK_RATE_HZ / ( ( TickType_t ) 1000 ) ));
		
		// Perform action here
		RecvPacket(&lPacket,sizeof(lPacket));		

		#if defined (DEBUG_RECEIVE_PACKET)
		printf(">>>> RECEIVE_PACKET\r\n");		
		printf("RECEIVE(A:%4d)\r\n",lPacket.structutre.Struct_A.var_a);
		printf("RECEIVE(A:%4d)\r\n",lPacket.structutre.Struct_A.var_b);

		printf("RECEIVE(B:%4d)\r\n",lPacket.structutre.Struct_B.var_a);
		printf("RECEIVE(B:%4d)\r\n",lPacket.structutre.Struct_B.var_b);

		printf("RECEIVE(C:%4d)\r\n",lPacket.structutre.Struct_C.var_a);
		printf("RECEIVE(C:%4d)\r\n",lPacket.structutre.Struct_C.var_b);
		printf("RECEIVE(C:%4d)\r\n",lPacket.structutre.Struct_C.var_c);	
		printf("<<<< RECEIVE_PACKET\r\n");		
		#endif /*DEBUG_RECEIVE_PACKET*/	
	}  
}

void vTaskSendPacket( void * pvParameters )
{
	uint32_t millisec = DELAY_MS_100*5;
	portTickType xLastWakeTime;
//	Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();

	(void) pvParameters;	
	for( ;; )
	{
		// Wait for the next cycle.
		vTaskDelayUntil( &xLastWakeTime, ( millisec *configTICK_RATE_HZ / ( ( TickType_t ) 1000 ) ));
		
		// Perform action here.
		++Cnt_Packet;

		#if 1
		if((Cnt_Packet%3)==0)
		{
			gPacket.type = Type_A;
			SendPacket(&gPacket,sizeof(gPacket));

			#if defined (DEBUG_SEND_PACKET)
			printf(">>>> SEND_PACKET(%4d,A+1)\r\n",Cnt_Packet);
			#endif /*DEBUG_SEND_PACKET*/
		}
		else if ((Cnt_Packet%3)==1)
		{
			gPacket.type = Type_B;
			SendPacket(&gPacket,sizeof(gPacket));	

			#if defined (DEBUG_SEND_PACKET)
			printf(">>>> SEND_PACKET (%4d,B+10)\r\n",Cnt_Packet);		
			#endif /*DEBUG_SEND_PACKET*/
		}
		else if ((Cnt_Packet%3)==2)
		{
			gPacket.type = Type_C;
			SendPacket(&gPacket,sizeof(gPacket));

			#if defined (DEBUG_SEND_PACKET)
			printf(">>>> SEND_PACKET(%4d,C+100)\r\n",Cnt_Packet);	
			#endif /*DEBUG_SEND_PACKET*/
		}
		#endif	
	}  
}

void vTaskCreateSubTask(void * pvParameters)
{
	(void) pvParameters;
	for( ;; )
	{
		#if defined (DEBUG_CREATE_TASK)
		printf(">>>>TaskSub Delete\r\n");
		#endif /*DEBUG_CREATE_TASK*/
		
		Flag_Func = 0 ;

		/* Delete the Init Thread */ 
		vTaskDelete(xTaskCreateSub);
	}	
}

void vTaskCreateTask( void * pvParameters )
{
	(void) pvParameters;	
	for( ;; )
	{		
		if (Flag_Func)
		{
			// Perform action here.			
			xTaskCreate(vTaskCreateSubTask, "vTaskSub", configMINIMAL_STACK_SIZE/2, NULL, mainABOVENORMAL_TASK_PRIORITY, &xTaskCreateSub ); 
			
			#if defined (DEBUG_CREATE_TASK)
			printf(">>>>TaskSub Create\r\n");		
			#endif /*DEBUG_CREATE_TASK*/
		}
	}  
}

void vTaskRisingFlag( void * pvParameters )
{
	uint32_t millisec = DELAY_MS_100*5;
	static uint16_t cnt = 0;
	
	portTickType xLastWakeTime;
//	Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();

	(void) pvParameters;
	for( ;; )
	{
		// Wait for the next cycle.
		vTaskDelayUntil( &xLastWakeTime, ( millisec *configTICK_RATE_HZ / ( ( TickType_t ) 1000 ) ));

		// Perform action here.	
		Flag_Func = 1;		

		#if defined (DEBUG_RISING_FLAG)
		printf(">>>>RisingFlag(%4dms) :%4d\r\n" ,millisec, ++cnt);
		#else
		( void ) cnt;		//Remove compiler warning about cnt being set but never used.
		#endif	/*DEBUG_RISING_FLAG*/		
	}  
}

void vTaskLogMsg500ms( void *pvParameters )
{	
	static uint32_t Cnt_LogMsg = 0;
	uint32_t millisec = 500;

	portTickType xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();

	(void) pvParameters;
	for( ;; )
	{
        vTaskDelayUntil( &xLastWakeTime, ( millisec *configTICK_RATE_HZ / ( ( TickType_t ) 1000 ) ));

		#if defined (DEBUG_LOG_MSG_500ms)
		printf("vTaskLogMsg500ms:Log(%4dms):%4d (%4d bytes , CPU : %d )\r\n" ,millisec ,++Cnt_LogMsg,xPortGetFreeHeapSize(),osGetCPUUsage());
		#endif
	}  
}

void vTaskLogMsg5s( void *pvParameters )
{	
	static uint32_t Cnt_LogMsg = 0;
	uint32_t millisec = 5000;

	portTickType xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();

	(void) pvParameters;
	for( ;; )
	{
        vTaskDelayUntil( &xLastWakeTime, ( millisec *configTICK_RATE_HZ / ( ( TickType_t ) 1000 ) ));

		#if defined (DEBUG_LOG_MSG_5s)		
		printf("vTaskLogMsg5s:Log(%4dms):%4d (%4d bytes)\r\n" ,millisec ,++Cnt_LogMsg,xPortGetFreeHeapSize());
		#endif
	}  
}

void vTaskLogMsg1s( void *pvParameters )
{	
	static uint32_t Cnt_LogMsg = 0;
	uint32_t millisec = 1000;

	portTickType xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();

	GPIO_SetMode(PB, BIT14, GPIO_MODE_OUTPUT);

	(void) pvParameters;
	for( ;; )
	{
        vTaskDelayUntil( &xLastWakeTime, ( millisec *configTICK_RATE_HZ / ( ( TickType_t ) 1000 ) ));
		PB14 ^= 1;

		#if defined (DEBUG_LOG_MSG_1s)
		printf("vTaskLogMsg1s:Log(%4dms):%4d (%4d bytes)\r\n" ,millisec ,++Cnt_LogMsg,xPortGetFreeHeapSize());
		#endif
	}  
}

__weak void vApplicationIdleHook(void)
{

}

__weak void vApplicationTickHook( void )
{

}

__weak void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}

__weak void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}


void TMR3_IRQHandler(void)
{
	static uint32_t LOG = 0;
	static uint16_t CNT = 0;
	
    if(TIMER_GetIntFlag(TIMER3) == 1)
    {
        TIMER_ClearIntFlag(TIMER3);
	
		if (CNT++ >= 1000)
		{		
			CNT = 0;
//        	printf("%s : %4d\r\n",__FUNCTION__,LOG++);
		}
		
    }
}


void TIMER3_Init(void)
{
    TIMER_Open(TIMER3, TIMER_PERIODIC_MODE, 1000);
    TIMER_EnableInt(TIMER3);
    NVIC_EnableIRQ(TMR3_IRQn);	
    TIMER_Start(TIMER3);
}


void UART0_Init(void)
{
    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 baud rate */
    UART_Open(UART0, 115200);

	/* Set UART receive time-out */
//	UART_SetTimeoutCnt(UART0, 20);

	printf("\r\nCLK_GetCPUFreq : %8d\r\n",CLK_GetCPUFreq());
	printf("CLK_GetHXTFreq : %8d\r\n",CLK_GetHXTFreq());
	printf("CLK_GetLXTFreq : %8d\r\n",CLK_GetLXTFreq());	
	printf("CLK_GetPCLK0Freq : %8d\r\n",CLK_GetPCLK0Freq());
	printf("CLK_GetPCLK1Freq : %8d\r\n",CLK_GetPCLK1Freq());	
}

void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable HIRC clock (Internal RC 48MHz) */
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);
//    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk);
	
    /* Wait for HIRC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
//    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);
	
    /* Select HCLK clock source as HIRC and HCLK source divider as 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));

    /* Enable UART0 clock */
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_PCLK0, CLK_CLKDIV0_UART0(1));
	
    CLK_EnableModuleClock(TMR3_MODULE);
    CLK_SetModuleClock(TMR3_MODULE, CLK_CLKSEL1_TMR3SEL_PCLK1, 0);
	
    /* Update System Core Clock */
    SystemCoreClockUpdate();

    /* Set PB multi-function pins for UART0 RXD=PB.12 and TXD=PB.13 */
    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk))    |       \
                    (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);

    /* Lock protected registers */
    SYS_LockReg();
}

/*
 * This is a template project for M031 series MCU. Users could based on this project to create their
 * own application without worry about the IAR/Keil project settings.
 *
 * This template application uses external crystal as HCLK source and configures UART0 to print out
 * "Hello World", users may need to do extra system configuration based on their system design.
 */

int main()
{

    SYS_Init();

    UART0_Init();

	TIMER3_Init();


    xTaskCreate( vTaskLogMsg1s, "LogMsg1s"			,configMINIMAL_STACK_SIZE 	, NULL	, mainNORMAL_TASK_PRIORITY		, NULL);
    xTaskCreate( vTaskLogMsg5s, "LogMsg5s"			,configMINIMAL_STACK_SIZE 	, NULL	, mainNORMAL_TASK_PRIORITY		, NULL);
    xTaskCreate( vTaskLogMsg500ms, "LogMsg500ms"	,configMINIMAL_STACK_SIZE 	, NULL	, mainNORMAL_TASK_PRIORITY		, NULL);

	//function flag enable and create sub task
	xTaskCreate(vTaskRisingFlag	,"vTaskRising"			,configMINIMAL_STACK_SIZE	,NULL	,mainNORMAL_TASK_PRIORITY		,NULL);   	
	xTaskCreate(vTaskCreateTask	,"vTaskCreate"		,configMINIMAL_STACK_SIZE	,NULL	,mainNORMAL_TASK_PRIORITY		,NULL); 
	//send data into structure and read
	xTaskCreate(vTaskSendPacket	,"vTaskSend"			,configMINIMAL_STACK_SIZE	,NULL	,mainNORMAL_TASK_PRIORITY		,NULL); 
	xTaskCreate(vTaskRecvPacket	,"vTaskRecv"			,configMINIMAL_STACK_SIZE	,NULL	,mainNORMAL_TASK_PRIORITY		,NULL); 
	//display current task list
	xTaskCreate(vTaskTasklist		,"vTaskTasklist"		,configMINIMAL_STACK_SIZE*2	,NULL	,mainNORMAL_TASK_PRIORITY		,NULL); 	
	//bit flag switch
	xTaskCreate(vTaskBitFlagSwitch	,"vTaskBitFlag"		,configMINIMAL_STACK_SIZE	,NULL	,mainNORMAL_TASK_PRIORITY		,NULL); 

	
    vTaskStartScheduler();

    for( ;; );
}

/*** (C) COPYRIGHT 2017 Nuvoton Technology Corp. ***/
