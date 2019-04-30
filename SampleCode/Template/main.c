/******************************************************************************
 * @file     main.c
 * @version  V1.00
 * @brief    A project template for M031 MCU.
 *
 * Copyright (C) 2017 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

#include "FreeRTOS.h"
#include "Task.h"
#include "Queue.h"
#include "semphr.h"


#define mainNORMAL_TASK_PRIORITY             	( tskIDLE_PRIORITY + 0UL )
#define mainABOVENORMAL_TASK_PRIORITY        	( tskIDLE_PRIORITY + 1UL )
#define mainHIGH_TASK_PRIORITY             		( tskIDLE_PRIORITY + 2UL )


#define printf(...)	\
{	\
	vTaskSuspendAll();	\
	printf(__VA_ARGS__);	\
	fflush(stdout); \
	xTaskResumeAll();	\
}	\

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
		printf("vTaskLogMsg500ms:Log(%4dms):%4d (%4d bytes)\r\n" ,millisec ,++Cnt_LogMsg,xPortGetFreeHeapSize());

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
		printf("vTaskLogMsg5s:Log(%4dms):%4d (%4d bytes)\r\n" ,millisec ,++Cnt_LogMsg,xPortGetFreeHeapSize());

	}
  
}

void vTaskLogMsg1s( void *pvParameters )
{	
	static uint32_t Cnt_LogMsg = 0;
	uint32_t millisec = 1000;

	portTickType xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();

	(void) pvParameters;
	for( ;; )
	{
        vTaskDelayUntil( &xLastWakeTime, ( millisec *configTICK_RATE_HZ / ( ( TickType_t ) 1000 ) ));
		printf("vTaskLogMsg1s:Log(%4dms):%4d (%4d bytes)\r\n" ,millisec ,++Cnt_LogMsg,xPortGetFreeHeapSize());

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


    xTaskCreate( vTaskLogMsg1s, "LogMsg1s", configMINIMAL_STACK_SIZE , NULL, mainNORMAL_TASK_PRIORITY, NULL );
    xTaskCreate( vTaskLogMsg5s, "LogMsg5s", configMINIMAL_STACK_SIZE , NULL, mainNORMAL_TASK_PRIORITY, NULL );
    xTaskCreate( vTaskLogMsg500ms, "LogMsg500ms", configMINIMAL_STACK_SIZE , NULL, mainNORMAL_TASK_PRIORITY, NULL );
	
    vTaskStartScheduler();

    for( ;; );
}

/*** (C) COPYRIGHT 2017 Nuvoton Technology Corp. ***/
