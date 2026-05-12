/*
 * Percepio TraceRecorder for Tracealyzer v989.878.767
 * Copyright 2025 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * The SAFERTOS specific parts of the trace recorder
 */

#define KERNEL_SOURCE_FILE

#include "SafeRTOS_API.h"
#include "trcRecorder.h"
#include "PortSpecifics.h"

#if (configUSE_TRACE_FACILITY == 1 && !defined(TRC_USE_TRACEALYZER_RECORDER))
#error Trace Recorder: You need to include trcRecorder.h at the end of your SafeRTOSConfig.h!
#endif

#if (defined(TRC_USE_TRACEALYZER_RECORDER) && TRC_USE_TRACEALYZER_RECORDER == 1)

#ifndef TRC_CFG_ENABLE_STACK_MONITOR
#define TRC_CFG_ENABLE_STACK_MONITOR 0
#endif

#if (configUSE_TICKLESS_IDLE != 0 && (TRC_HWTC_TYPE == TRC_OS_TIMER_INCR || TRC_HWTC_TYPE == TRC_OS_TIMER_DECR))
	/*
		The below error message is to alert you on the following issue:

		The hardware port selected in trcConfig.h uses a periodic interrupt timer for the
		timestamping, probably the same timer as used by SafeRTOS for the tick interrupt,
		e.g. SysTick on ARM Cortex-M.

		When using tickless idle, the recorder needs an independent time source in order to
		correctly record the length of the idle time, like a free-running counter.

		You may override this warning by defining the TRC_CFG_ACKNOWLEDGE_TICKLESS_IDLE_WARNING
		macro in your trcConfig.h file. But then the time scale may be incorrect during
		tickless idle periods.

		To get this correct, set up a hardware timer as a free-running counter, set the hardware
		port in trcConfig.h to TRC_HARDWARE_PORT_APPLICATION_DEFINED and define the HWTC macros
		accordingly (see trcHardwarePort.h for details).

		For ARM Cortex-M3, M4 and M7 MCUs this is not an issue, since the recorder uses the
		DWT cycle counter for timestamping when available.
	*/

	#ifndef TRC_CFG_ACKNOWLEDGE_TICKLESS_IDLE_WARNING
	#error Trace Recorder: This timestamping mode is not recommended with Tickless Idle.
	#endif
#endif

traceResult xTraceKernelPortDelay(uint32_t uiTicks)
{
	xTaskDelay(uiTicks);

	return TRC_SUCCESS;
}

unsigned char xTraceKernelPortIsSchedulerSuspended(void)
{
	return xTaskIsSchedulerSuspended() == pdTRUE;
}

#if defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1)

traceResult xTraceKernelPortGetUnusedStack(void* pvTask, TraceUnsignedBaseType_t* puxUnusedStack)
{
	portUnsignedBaseType xFreeStackBytes = 0;

	if (xTaskGetStackUsage(pvTask, &xFreeStackBytes) != pdPASS)
	{
		return TRC_FAIL;
	}

	*puxUnusedStack = (TraceUnsignedBaseType_t)xFreeStackBytes;

	return TRC_SUCCESS;
}

#endif

uint8_t prvTraceGetQueueType(void* handle)
{
	// This is either declared in header file in FreeRTOS 8 and later, or as extern above
	return (uint8_t)uxQueueGetQueueType(handle);
}

uint32_t prvGetTaskNumber(void* pxObject)
{
	return uxTaskGetTaskNumber(pxObject);
}

uint32_t prvGetQueueNumber(void* pxObject)
{
	return uxQueueGetQueueNumber(pxObject);
}

#if ((pdKERNEL_MAJOR_VERSION == 5 && pdKERNEL_MINOR_VERSION >= 10) || pdKERNEL_MAJOR_VERSION > 5)
uint32_t prvGetTimerNumber(void* pxObject)
{
	/* There is no uxTimerGetTimerNumber function */
	return ((timerControlBlockType*)pxObject)->uxTimerNumber;
}
#endif /* ((pdKERNEL_MAJOR_VERSION == 5 && pdKERNEL_MINOR_VERSION >= 10) || pdKERNEL_MAJOR_VERSION > 5) */

#if ((pdKERNEL_MAJOR_VERSION == 5 && pdKERNEL_MINOR_VERSION >= 10) || pdKERNEL_MAJOR_VERSION > 5)
uint32_t prvGetEventGroupNumber(void* pxObject)
{
	return uxTaskGetEventGroupNumber(pxObject);
}
#endif /* ((pdKERNEL_MAJOR_VERSION == 5 && pdKERNEL_MINOR_VERSION >= 10) || pdKERNEL_MAJOR_VERSION > 5) */

#if ( ( pdKERNEL_MAJOR_VERSION == 9 && pdKERNEL_MINOR_VERSION >= 5 && ( defined( pdKERNEL_INSTRUMENTATION_VERSION ) && pdKERNEL_INSTRUMENTATION_VERSION >= 1 ) ) || pdKERNEL_MAJOR_VERSION > 9 )
uint32_t prvGetStreambufferNumber(void* pxObject)
{
    return uxTaskGetStreambufferNumber(pxObject);
}
#endif /* ( ( pdKERNEL_MAJOR_VERSION == 9 && pdKERNEL_MINOR_VERSION >= 5 && ( defined( pdKERNEL_INSTRUMENTATION_VERSION ) && pdKERNEL_INSTRUMENTATION_VERSION >= 1 ) ) || pdKERNEL_MAJOR_VERSION > 9 ) */

#if ( ( pdKERNEL_MAJOR_VERSION == 9 && pdKERNEL_MINOR_VERSION >= 5 && ( defined( pdKERNEL_INSTRUMENTATION_VERSION ) && pdKERNEL_INSTRUMENTATION_VERSION >= 1 ) ) || pdKERNEL_MAJOR_VERSION > 9 )
uint32_t prvGetEvtMplxNumber(void* pxObject)
{
    return uxTaskGetEvtMplxNumber(pxObject);
}
#endif /* ( ( pdKERNEL_MAJOR_VERSION == 9 && pdKERNEL_MINOR_VERSION >= 5 && ( defined( pdKERNEL_INSTRUMENTATION_VERSION ) && pdKERNEL_INSTRUMENTATION_VERSION >= 1 ) ) || pdKERNEL_MAJOR_VERSION > 9 ) */

static portTaskHandleType HandleTzCtrl = (void*)0;       /* TzCtrl task TCB */

static xTCB tcbTzCtrl = { 0 };

/* The TzCtrl task - receives commands from Tracealyzer (start/stop) */
static void TzCtrl( void *pvParameters );

#if ((pdKERNEL_MAJOR_VERSION == 5 && pdKERNEL_MINOR_VERSION > 10) || pdKERNEL_MAJOR_VERSION > 5)
static const xTaskParameters TzCtrlParameters =
{
	TzCtrl,
	"TzCtrl",
	&tcbTzCtrl,
	acTzCtrlStack,  /* defined in PortSpecifics.h */
	TRC_CFG_CTRL_TASK_STACK_SIZE,
	(void*)0,
	TRC_CFG_CTRL_TASK_PRIORITY,
	(void*)0,
	xTracealyzerTzCtrlTaskPortParameters
};
#else /* ((pdKERNEL_MAJOR_VERSION == 5 && pdKERNEL_MINOR_VERSION > 10) || pdKERNEL_MAJOR_VERSION > 5) */
static const xTaskParameters TzCtrlParameters =
{
	TzCtrl,
	"TzCtrl",
	&tcbTzCtrl,
	acTzCtrlStack,  /* defined in PortSpecifics.h */
	TRC_CFG_CTRL_TASK_STACK_SIZE,
	(void*)0,
	TRC_CFG_CTRL_TASK_PRIORITY,
	xTracealyzerTzCtrlTaskPortParameters
};
#endif /* ((pdKERNEL_MAJOR_VERSION == 5 && pdKERNEL_MINOR_VERSION > 10) || pdKERNEL_MAJOR_VERSION > 5) */

traceResult xTraceKernelPortInitialize(TraceKernelPortDataBuffer_t *pxBuffer)
{
	(void)pxBuffer;

	return TRC_SUCCESS;
}

traceResult xTraceKernelPortEnable(void)
{
	if (HandleTzCtrl == (void*)0)
	{
		/* Creates the TzCtrl task - receives trace commands (start, stop, ...) */
		xTaskCreate( &TzCtrlParameters, &HandleTzCtrl );
		if (HandleTzCtrl == (void*)0)
		{
			xTraceError(TRC_ERROR_TZCTRLTASK_NOT_CREATED);

			return TRC_FAIL;
		}
	}

	return TRC_SUCCESS;
}

/*******************************************************************************
 * TzCtrl
 *
 * Task for receiving commands from Tracealyzer and for recorder diagnostics.
 *
 ******************************************************************************/
static void TzCtrl( void *pvParameters )
{
	(void)pvParameters;

	while (1)
	{
		xTraceTzCtrl();

		xTaskDelay(TRC_CFG_CTRL_TASK_DELAY);	/* 10ms */
	}
}

#endif /*(TRC_USE_TRACEALYZER_RECORDER == 1)*/
