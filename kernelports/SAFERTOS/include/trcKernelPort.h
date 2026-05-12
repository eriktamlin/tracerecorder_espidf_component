/*
 * Percepio TraceRecorder for Tracealyzer v989.878.767
 * Copyright 2025 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * The SAFERTOS specific definitions of the trace recorder
 */

#ifndef TRC_KERNEL_PORT_H
#define TRC_KERNEL_PORT_H

#include "SafeRTOS_API.h"	/* Defines configUSE_TRACE_FACILITY */
#include "SafeRTOSConfigApp.h"
#include "task.h"

#ifdef __cplusplus
extern "C" {
#endif

#if ( configUSE_TRACE_FACILITY == 1 )
#define TRC_USE_TRACEALYZER_RECORDER 1
#else
#define TRC_USE_TRACEALYZER_RECORDER 0
#endif

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

/**
 * @def TRC_PLATFORM_CFG
 * @brief DO NOT CHANGE THIS
 * This defines the basis for version specific lookup of
 * platform configuration files.
 */
#define TRC_PLATFORM_CFG "SAFERTOS"

/**
 * @def TRC_PLATFORM_CFG_MAJOR
 * @brief DO NOT CHANGE THIS
 * Major release version for platform definition file.
 * Does NOT need to match the RTOS version.
 */
#define TRC_PLATFORM_CFG_MAJOR 1

/**
 * @def TRC_PLATFORM_CFG_MINOR
 * @brief DO NOT CHANGE THIS
 * Minor release version for platform definition file.
 * Does NOT need to match the RTOS version.
 */
#define TRC_PLATFORM_CFG_MINOR 0

/**
 * @def TRC_PLATFORM_CFG_PATCH
 * @brief DO NOT CHANGE THIS
 * Patchlevel release version for platform definition file.
 * Does NOT need to match the RTOS version.
 */
#define TRC_PLATFORM_CFG_PATCH 0

#if ( pdKERNEL_MAJOR_VERSION == 7 )
#define trcIS_TASK_VALID( pxTask ) ((pxTask != (void*)0) && xPortIsTaskHandleValid(pxTask))
#define trcIS_QUEUE_VALID( pxQueue ) ((pxQueue != (void*)0) && prvIS_QUEUE_VALID(pxQueue))
#define trcIS_TIMER_VALID( pxTimer ) (xTimerIsHandleValidForTrace(pxTimer))
#define trcIS_EVENTGROUP_VALID( pxEventGroup ) ((pxEventGroup != (void*)0) && prvIS_EVENT_GROUP_VALID(((eventGroupType*)pxEventGroup)))
#else
#define trcIS_TASK_VALID( pxTask ) (xPortIsTaskHandleValid(pxTask))
#define trcIS_QUEUE_VALID( pxQueue ) ((pxQueue != (void*)0) && xQueueIsQueueValid(pxQueue))
#define trcIS_TIMER_VALID( pxTimer ) (xTimerIsHandleValidForTrace(pxTimer))
#define trcIS_EVENTGROUP_VALID( pxEventGroup ) ((pxEventGroup != (void*)0) && xEventGroupIsEventGroupHandleValid(((eventGroupType*)pxEventGroup)))
#endif

uint32_t prvGetTaskNumber(void* pxObject);
uint32_t prvGetQueueNumber(void* pxObject);
uint32_t prvGetTimerNumber(void* pxObject);
uint32_t prvGetEventGroupNumber(void* pxObject);
uint32_t prvGetStreambufferNumber(void* pxObject);
uint32_t prvGetEvtMplxNumber(void* pxObject);

uint8_t prvTraceGetQueueType(void* handle);

#define TRC_KERNEL_PORT_KERNEL_CAN_SWITCH_TO_SAME_TASK 1

#define TRC_USE_HEAPS 0

#include <trcHeap.h>

#define TRC_KERNEL_PORT_BUFFER_SIZE (sizeof(TraceHeapHandle_t) + sizeof(void*))

typedef struct TraceKernelPortDataBuffer	/* Aligned */
{
	uint8_t buffer[TRC_KERNEL_PORT_BUFFER_SIZE];
} TraceKernelPortDataBuffer_t;

traceResult xTraceKernelPortEnable(void);

traceResult xTraceKernelPortInitialize(TraceKernelPortDataBuffer_t* pxBuffer);

traceResult xTraceKernelPortDelay(uint32_t uiTicks);

unsigned char xTraceKernelPortIsSchedulerSuspended(void);

#if defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1)

 /**
  * @internal Retrieves the unused stack for a task
  *
  * @param[in] pvTask Task pointer
  * @param[out] puxUnusedStack The unused stack
  *
  * @retval TRC_FAIL Failure
  * @retval TRC_SUCCESS Success
  */
traceResult xTraceKernelPortGetUnusedStack(void* pvTask, TraceUnsignedBaseType_t *puxUnusedStack);

#endif

/**
 * @brief Kernel specific way to properly allocate critical sections
 */
#define TRC_KERNEL_PORT_ALLOC_CRITICAL_SECTION() TraceUnsignedBaseType_t TRACE_ALLOC_CRITICAL_SECTION_NAME;

 /**
  * @brief Kernel specific way to properly allocate critical sections
  */
#define TRC_KERNEL_PORT_ENTER_CRITICAL_SECTION()    { TRACE_ALLOC_CRITICAL_SECTION_NAME = 0; portENTER_CRITICAL_WITHIN_API(); }

  /**
   * @brief Kernel specific way to properly allocate critical sections
   */
#define TRC_KERNEL_PORT_EXIT_CRITICAL_SECTION()     { (void)TRACE_ALLOC_CRITICAL_SECTION_NAME; portEXIT_CRITICAL_WITHIN_API(); }

#define TRC_KERNEL_PORT_SET_INTERRUPT_MASK()        ((TraceBaseType_t)safertosapiSET_INTERRUPT_MASK_FROM_ISR())

#define TRC_KERNEL_PORT_CLEAR_INTERRUPT_MASK(xMask) (safertosapiCLEAR_INTERRUPT_MASK_FROM_ISR((portUInt32Type)(xMask)))

/*******************************************************************************
 * Note: Setting names for event groups is difficult to support, this has been
 * excluded intentionally. This since we don't know if event_groups.c is
 * included in the build, so referencing it from the recorder may cause errors.
 ******************************************************************************/

#define TRACE_KERNEL_VERSION 0x6AA6
#define TRC_TICK_RATE_HZ configTICK_RATE_HZ
#define TRACE_CPU_CLOCK_HZ configCPU_CLOCK_HZ

#define xTraceKernelPortGetSystemHeapHandle() 0

#define vTraceSetQueueName(object, name) xTraceObjectSetNameWithoutHandle(object, name)
#define vTraceSetSemaphoreName(object, name) xTraceObjectSetNameWithoutHandle(object, name)
#define vTraceSetMutexName(object, name) xTraceObjectSetNameWithoutHandle(object, name)
#define vTraceSetEventGroupName(object, name) xTraceObjectSetNameWithoutHandle(object, name)
#define vTraceSetStreamBufferName(object, name) xTraceObjectSetNameWithoutHandle(object, name)
#define vTraceSetMessageBufferName(object, name) xTraceObjectSetNameWithoutHandle(object, name)
#define vTraceSetEvtMplxName(object, name) xTraceObjectSetNameWithoutHandle(object, name)

/*************************************************************************/
/* KERNEL SPECIFIC OBJECT CONFIGURATION									 */
/*************************************************************************/

/*******************************************************************************
 * The event codes - should match the offline config file.
 ******************************************************************************/

/*** Event codes - should match the Tracealyzer config file *****/
#define PSF_EVENT_NULL_EVENT								0x00

#define PSF_EVENT_TRACE_START								0x01
#define PSF_EVENT_TS_CONFIG									0x02
#define PSF_EVENT_OBJ_NAME									0x03
#define PSF_EVENT_OBJ_DATA									0x04
#define PSF_EVENT_DEFINE_ISR								0x05

#define PSF_EVENT_TASK_READY								0x06

#define PSF_EVENT_NEW_TIME									0x07
#define PSF_EVENT_NEW_TIME_SCHEDULER_SUSPENDED				0x08
#define PSF_EVENT_ISR_BEGIN									0x09
#define PSF_EVENT_ISR_RESUME								0x0A
#define PSF_EVENT_TS_BEGIN									0x0B
#define PSF_EVENT_TS_RESUME									0x0C
#define PSF_EVENT_TASK_ACTIVATE								0x0D

#define PSF_EVENT_LOWPOWER_BEGIN							0x0E
#define PSF_EVENT_LOWPOWER_END								0x0F

#define PSF_EVENT_IFE_NEXT									0x10
#define PSF_EVENT_IFE_DIRECT								0x11

#define PSF_EVENT_TASK_CREATE								0x12
#define PSF_EVENT_TASK_CREATE_FAILED						0x13
#define PSF_EVENT_TASK_DELETE								0x14
#define PSF_EVENT_TASK_DELAY_UNTIL							0x15
#define PSF_EVENT_TASK_DELAY								0x16
#define PSF_EVENT_TASK_SUSPEND								0x17
#define PSF_EVENT_TASK_RESUME								0x18
#define PSF_EVENT_TASK_RESUME_FROMISR						0x19
#define PSF_EVENT_TASK_PRIORITY								0x1A
#define PSF_EVENT_TASK_PRIO_INHERIT							0x1B
#define PSF_EVENT_TASK_PRIO_DISINHERIT						0x1C

#define PSF_EVENT_TASK_NOTIFY_SEND							0x1D
#define PSF_EVENT_TASK_NOTIFY_SEND_FAILED					0x1E
#define PSF_EVENT_TASK_NOTIFY_SEND_FROM_ISR					0x1F
#define PSF_EVENT_TASK_NOTIFY_SEND_FROM_ISR_FAILED			0x20
#define PSF_EVENT_TASK_NOTIFY_WAIT							0x21
#define PSF_EVENT_TASK_NOTIFY_WAIT_BLOCK					0x22
#define PSF_EVENT_TASK_NOTIFY_WAIT_FAILED					0x23

#define PSF_EVENT_QUEUE_CREATE								0x24
#define PSF_EVENT_QUEUE_CREATE_FAILED						0x25
#define PSF_EVENT_QUEUE_SEND								0x26
#define PSF_EVENT_QUEUE_SEND_FAILED							0x27
#define PSF_EVENT_QUEUE_SEND_BLOCK							0x28
#define PSF_EVENT_QUEUE_SEND_TO_FRONT						0x29
#define PSF_EVENT_QUEUE_SEND_TO_FRONT_FAILED				0x2A
#define PSF_EVENT_QUEUE_SEND_TO_FRONT_BLOCK					0x2B
#define PSF_EVENT_QUEUE_SEND_FROMISR						0x2C
#define PSF_EVENT_QUEUE_SEND_FROMISR_FAILED					0x2D
#define PSF_EVENT_QUEUE_SEND_TO_FRONT_FROMISR				0x2E
#define PSF_EVENT_QUEUE_SEND_TO_FRONT_FROMISR_FAILED		0x2F
#define PSF_EVENT_QUEUE_RECEIVE								0x30
#define PSF_EVENT_QUEUE_RECEIVE_FAILED						0x31
#define PSF_EVENT_QUEUE_RECEIVE_BLOCK						0x32
#define PSF_EVENT_QUEUE_RECEIVE_FROMISR						0x33
#define PSF_EVENT_QUEUE_RECEIVE_FROMISR_FAILED				0x34
#define PSF_EVENT_QUEUE_PEEK								0x35
#define PSF_EVENT_QUEUE_PEEK_FAILED							0x36
#define PSF_EVENT_QUEUE_PEEK_BLOCK							0x37

#define PSF_EVENT_SEMAPHORE_CREATE							0x38
#define PSF_EVENT_SEMAPHORE_CREATE_FAILED					0x39
#define PSF_EVENT_SEMAPHORE_SEND							0x3A
#define PSF_EVENT_SEMAPHORE_SEND_FAILED						0x3B
#define PSF_EVENT_SEMAPHORE_SEND_FROMISR					0x3C
#define PSF_EVENT_SEMAPHORE_SEND_FROMISR_FAILED				0x3D
#define PSF_EVENT_SEMAPHORE_RECEIVE							0x3E
#define PSF_EVENT_SEMAPHORE_RECEIVE_FAILED					0x3F
#define PSF_EVENT_SEMAPHORE_RECEIVE_BLOCK					0x40
#define PSF_EVENT_SEMAPHORE_RECEIVE_FROMISR					0x41
#define PSF_EVENT_SEMAPHORE_RECEIVE_FROMISR_FAILED			0x42

#define PSF_EVENT_MUTEX_CREATE								0x43
#define PSF_EVENT_MUTEX_CREATE_FAILED						0x44
#define PSF_EVENT_MUTEX_SEND								0x45
#define PSF_EVENT_MUTEX_SEND_FAILED							0x46
#define PSF_EVENT_MUTEX_RECEIVE								0x47
#define PSF_EVENT_MUTEX_RECEIVE_FAILED						0x48
#define PSF_EVENT_MUTEX_RECEIVE_BLOCK						0x49

#define PSF_EVENT_TIMER_CREATE								0x4A
#define PSF_EVENT_TIMER_CREATE_FAILED						0x4B
#define PSF_EVENT_TIMER_DELETE								0x4C
#define PSF_EVENT_TIMER_DELETE_FAILED						0x4D
#define PSF_EVENT_TIMER_START								0x4E
#define PSF_EVENT_TIMER_START_FAILED						0x4F
#define PSF_EVENT_TIMER_STOP								0x50
#define PSF_EVENT_TIMER_STOP_FAILED							0x51
#define PSF_EVENT_TIMER_CHANGEPERIOD						0x52
#define PSF_EVENT_TIMER_CHANGEPERIOD_FAILED					0x53
#define PSF_EVENT_TIMER_START_FROMISR						0x54
#define PSF_EVENT_TIMER_START_FROMISR_FAILED				0x55
#define PSF_EVENT_TIMER_STOP_FROMISR						0x56
#define PSF_EVENT_TIMER_STOP_FROMISR_FAILED					0x57
#define PSF_EVENT_TIMER_CHANGEPERIOD_FROMISR				0x58
#define PSF_EVENT_TIMER_CHANGEPERIOD_FROMISR_FAILED			0x59
#define PSF_EVENT_TIMER_PROCESS_START						0x5A
#define PSF_EVENT_TIMER_PROCESS_STOP						0x5B
#define PSF_EVENT_TIMER_PROCESS_CHANGEPERIOD				0x5C
#define PSF_EVENT_TIMER_PROCESS_DELETE						0x5D
#define PSF_EVENT_TIMER_CALLBACK							0x5E

#define PSF_EVENT_STATEMACHINE_STATE_CREATE					0x5F
#define PSF_EVENT_STATEMACHINE_CREATE						0x60
#define PSF_EVENT_STATEMACHINE_STATECHANGE					0x61

#define PSF_EVENT_UNUSED_STACK								0x62
/* #define PSF_EVENT_UNUSED									0x63 */

#define PSF_EVENT_EVENTGROUP_CREATE							0x64
#define PSF_EVENT_EVENTGROUP_CREATE_FAILED					0x65
#define PSF_EVENT_EVENTGROUP_DELETE							0x66
#define PSF_EVENT_EVENTGROUP_DELETE_FAILED					0x67
#define PSF_EVENT_EVENTGROUP_SETBITS						0x68
#define PSF_EVENT_EVENTGROUP_SETBITS_FAILED					0x69
#define PSF_EVENT_EVENTGROUP_SETBITS_FROMISR				0x6A
#define PSF_EVENT_EVENTGROUP_SETBITS_FROMISR_FAILED			0x6B
#define PSF_EVENT_EVENTGROUP_WAITBITS						0x6C
#define PSF_EVENT_EVENTGROUP_WAITBITS_FAILED				0x6D
#define PSF_EVENT_EVENTGROUP_WAITBITS_BLOCK					0x6E
#define PSF_EVENT_EVENTGROUP_CLEARBITS						0x6F
#define PSF_EVENT_EVENTGROUP_CLEARBITS_FAILED				0x70
#define PSF_EVENT_EVENTGROUP_CLEARBITS_FROMISR				0x71
#define PSF_EVENT_EVENTGROUP_CLEARBITS_FROMISR_FAILED		0x72

#define PSF_EVENT_EXTENSION_CREATE							0x73
#define PSF_EVENT_HEAP_CREATE								0x74

#define PSF_EVENT_COUNTER_CREATE							0x75
#define PSF_EVENT_COUNTER_CHANGE							0x76
#define PSF_EVENT_COUNTER_LIMIT_EXCEEDED					0x77

#define PSF_EVENT_INTERVAL_CHANNEL_SET_CREATE				0x78
#define PSF_EVENT_INTERVAL_CHANNEL_CREATE					0x79
#define PSF_EVENT_INTERVAL_START							0x7A
#define PSF_EVENT_INTERVAL_STOP								0x7B

#define PSF_EVENT_RUNNABLE_REGISTER							0x7C
#define PSF_EVENT_RUNNABLE_START							0x7D
#define PSF_EVENT_RUNNABLE_STOP								0x7E

#define PSF_EVENT_DEPENDENCY_REGISTER						0x7F

#define PSF_EVENT_EVTMPLX_CREATE                            0x80
#define PSF_EVENT_EVTMPLX_CREATE_FAILED                     0x81
#define PSF_EVENT_EVTMPLX_ADD_OBJECT                        0x82
#define PSF_EVENT_EVTMPLX_ADD_OBJECT_FAILED                 0x83
#define PSF_EVENT_EVTMPLX_MODIFY_OBJECT                     0x84
#define PSF_EVENT_EVTMPLX_MODIFY_OBJECT_FAILED              0x85
#define PSF_EVENT_EVTMPLX_REMOVE_OBJECT                     0x86
#define PSF_EVENT_EVTMPLX_REMOVE_OBJECT_FAILED              0x87
#define PSF_EVENT_EVTMPLX_WAIT                              0x88
#define PSF_EVENT_EVTMPLX_WAIT_BLOCK                        0x89
#define PSF_EVENT_EVTMPLX_WAIT_FAILED                       0x8A
#define PSF_EVENT_EVTMPLX_UPDATE                            0x8B
#define PSF_EVENT_EVTMPLX_DELETE                            0x8C

#define PSF_EVENT_STREAMBUFFER_CREATE                       0x90
#define PSF_EVENT_STREAMBUFFER_CREATE_FAILED                0x91
#define PSF_EVENT_STREAMBUFFER_SEND							0x92
#define PSF_EVENT_STREAMBUFFER_SEND_BLOCK					0x93
#define PSF_EVENT_STREAMBUFFER_SEND_FAILED					0x94
#define PSF_EVENT_STREAMBUFFER_RECEIVE						0x95
#define PSF_EVENT_STREAMBUFFER_RECEIVE_BLOCK				0x96
#define PSF_EVENT_STREAMBUFFER_RECEIVE_FAILED				0x97
#define PSF_EVENT_STREAMBUFFER_SEND_FROM_ISR				0x98
#define PSF_EVENT_STREAMBUFFER_SEND_FROM_ISR_FAILED			0x99
#define PSF_EVENT_STREAMBUFFER_RECEIVE_FROM_ISR				0x9A
#define PSF_EVENT_STREAMBUFFER_RECEIVE_FROM_ISR_FAILED		0x9B
#define PSF_EVENT_STREAMBUFFER_RESET						0x9C

#define PSF_EVENT_MESSAGEBUFFER_CREATE                      0xA0
#define PSF_EVENT_MESSAGEBUFFER_CREATE_FAILED               0xA1
#define PSF_EVENT_MESSAGEBUFFER_SEND						0xA2
#define PSF_EVENT_MESSAGEBUFFER_SEND_BLOCK					0xA3
#define PSF_EVENT_MESSAGEBUFFER_SEND_FAILED					0xA4
#define PSF_EVENT_MESSAGEBUFFER_RECEIVE						0xA5
#define PSF_EVENT_MESSAGEBUFFER_RECEIVE_BLOCK				0xA6
#define PSF_EVENT_MESSAGEBUFFER_RECEIVE_FAILED				0xA7
#define PSF_EVENT_MESSAGEBUFFER_SEND_FROM_ISR				0xA8
#define PSF_EVENT_MESSAGEBUFFER_SEND_FROM_ISR_FAILED		0xA9
#define PSF_EVENT_MESSAGEBUFFER_RECEIVE_FROM_ISR			0xAA
#define PSF_EVENT_MESSAGEBUFFER_RECEIVE_FROM_ISR_FAILED		0xAB
#define PSF_EVENT_MESSAGEBUFFER_RESET						0xAC

/* Placing user events near the end */
#define PSF_EVENT_USER_EVENT								0xF0

#define PSF_EVENT_USER_EVENT_FIXED							0xF8

#define TRC_EVENT_LAST_ID									((PSF_EVENT_USER_EVENT_FIXED) + 7)

/*** The trace macros ******************************************/

/* A macro that will update the tick count when returning from tickless idle */
#undef traceINCREASE_TICK_COUNT
/* Note: This can handle time adjustments of max 2^32 ticks, i.e., 35 seconds at 120 MHz. Thus, tick-less idle periods longer than 2^32 ticks will appear "compressed" on the time line.*/
#define traceINCREASE_TICK_COUNT( xCount ) { uint32_t uiTraceTickCount; xTraceTimestampGetOsTickCount(&uiTraceTickCount); xTraceTimestampSetOsTickCount(uiTraceTickCount + (xCount)); }

#if (TRC_CFG_INCLUDE_OSTICK_EVENTS == 1)
#define TRC_OS_TICK_EVENT(uxSchedulerSuspended, xTickCount) if ((uxSchedulerSuspended) == (portUnsignedBaseType) pdFALSE) { prvTraceEventCreate1(PSF_EVENT_NEW_TIME, xTickCount); }
#else
#define TRC_OS_TICK_EVENT(uxSchedulerSuspended, xTickCount)
#endif

/* Called on each OS tick. */
#undef traceTASK_INCREMENT_TICK
#define traceTASK_INCREMENT_TICK( xTickCount ) \
	if (uxSchedulerSuspended == ( portUnsignedBaseType ) pdTRUE || uxMissedTicks == 0) { xTraceTimestampSetOsTickCount((xTickCount) + 1); } \
	TRC_OS_TICK_EVENT(uxSchedulerSuspended, (xTickCount) + 1)

/* Called for each task that becomes ready */
#if (TRC_CFG_INCLUDE_READY_EVENTS == 1)
#undef traceMOVED_TASK_TO_READY_STATE
#define traceMOVED_TASK_TO_READY_STATE( pxTCB ) \
	prvTraceEventCreate1(PSF_EVENT_TASK_READY, pxTCB)
#else /* (TRC_CFG_INCLUDE_READY_EVENTS == 1) */
#define traceMOVED_TASK_TO_READY_STATE( pxTCB )
#endif /* (TRC_CFG_INCLUDE_READY_EVENTS == 1) */

/* Called on each task-switch */
#undef traceTASK_SWITCHED_IN
#define traceTASK_SWITCHED_IN() \
	xTraceTaskSwitch(pxCurrentTCB, pxCurrentTCB->uxPriority)

/* Called on vTaskCreate */
#undef traceTASK_CREATE
#define traceTASK_CREATE(pxNewTCB) \
	xTraceTaskRegisterWithoutHandle(pxNewTCB, (pxNewTCB)->pcNameOfTask, (pxNewTCB)->uxPriority)

/* Called in vTaskCreate, if it fails (typically if the stack can not be allocated) */
#undef traceTASK_CREATE_FAILED
#define traceTASK_CREATE_FAILED() \
	prvTraceEventCreate0(PSF_EVENT_TASK_CREATE_FAILED)

/* Called on vTaskDelete */
#undef traceTASK_DELETE
#define traceTASK_DELETE( pxTaskToDelete ) \
	xTraceTaskUnregisterWithoutHandle(pxTaskToDelete, (pxTaskToDelete)->uxPriority)

#if (TRC_CFG_SCHEDULING_ONLY == 0)

#if (configUSE_TICKLESS_IDLE != 0)

#undef traceLOW_POWER_IDLE_BEGIN
#define traceLOW_POWER_IDLE_BEGIN() \
	{ \
		prvTraceEventCreate1(PSF_EVENT_LOWPOWER_BEGIN, xExpectedIdleTime); \
	}

#undef traceLOW_POWER_IDLE_END
#define traceLOW_POWER_IDLE_END() \
	{ \
		prvTraceEventCreate0(PSF_EVENT_LOWPOWER_END); \
	}

#endif

/* Called on vTaskSuspend */
#undef traceTASK_SUSPEND
#define traceTASK_SUSPEND( pxTaskToSuspend ) \
	prvTraceEventCreate1(PSF_EVENT_TASK_SUSPEND, pxTaskToSuspend)

/* Called on vTaskDelay - note the use of variable xTicksToDelay */
#undef traceTASK_DELAY
#define traceTASK_DELAY() \
	prvTraceEventCreate1(PSF_EVENT_TASK_DELAY, xTicksToDelay)

/* Called on vTaskDelayUntil - note the use of variable xTimeToWake */
#undef traceTASK_DELAY_UNTIL
#define traceTASK_DELAY_UNTIL(xTimeToWake) \
	prvTraceEventCreate1(PSF_EVENT_TASK_DELAY_UNTIL, xTimeToWake)

/* Called in xQueueCreate, and thereby for all other object based on queues, such as semaphores. */
#undef traceQUEUE_CREATE
#define traceQUEUE_CREATE( pxNewQueue )\
	switch (uxQueueType) \
	{ \
	case queueQUEUE_IS_QUEUE: \
		prvTraceEventCreate2(PSF_EVENT_QUEUE_CREATE, pxNewQueue, (pxNewQueue)->uxMaxNumberOfItems); \
		break; \
	case queueQUEUE_IS_SEMAPHORE: \
		prvTraceEventCreate2(PSF_EVENT_SEMAPHORE_CREATE, pxNewQueue, (pxNewQueue)->uxMaxNumberOfItems); \
		break; \
	case queueQUEUE_IS_MUTEX: \
		prvTraceEventCreate1(PSF_EVENT_MUTEX_CREATE, pxNewQueue); \
		break; \
	}

/* Called in xQueueCreate, if the queue creation fails */
#undef traceQUEUE_CREATE_FAILED
#define traceQUEUE_CREATE_FAILED( pcBuffer, queueType, queueLength ) \
	switch (queueType) \
	{ \
	case queueQUEUE_IS_QUEUE: \
		prvTraceEventCreate2(PSF_EVENT_QUEUE_CREATE_FAILED, 0, queueLength); \
		break; \
	case queueQUEUE_IS_SEMAPHORE: \
		prvTraceEventCreate2(PSF_EVENT_SEMAPHORE_CREATE_FAILED, 0, queueLength); \
		break; \
	case queueQUEUE_IS_MUTEX: \
		prvTraceEventCreate1(PSF_EVENT_MUTEX_CREATE_FAILED, 0); \
		break; \
	}

/* Called when a message is sent to a queue */
#undef traceQUEUE_SEND
#define traceQUEUE_SEND( pxQueue ) \
	switch ((pxQueue)->uxQueueType) \
	{ \
	case queueQUEUE_IS_QUEUE: \
		prvTraceEventCreate3(xPosition == queueSEND_TO_BACK ? PSF_EVENT_QUEUE_SEND : PSF_EVENT_QUEUE_SEND_TO_FRONT, pxQueue, xTicksToWait, (pxQueue)->uxItemsWaiting + 1); \
		break; \
	case queueQUEUE_IS_SEMAPHORE: \
		prvTraceEventCreate2(PSF_EVENT_SEMAPHORE_SEND, pxQueue, (pxQueue)->uxItemsWaiting + 1); \
		break; \
	case queueQUEUE_IS_MUTEX: \
		prvTraceEventCreate2(PSF_EVENT_MUTEX_SEND, pxQueue, (pxQueue)->uxItemsWaiting + 1); \
		break; \
	}

/* Called when a message failed to be sent to a queue (timeout) */
#undef traceQUEUE_SEND_FAILED
#define traceQUEUE_SEND_FAILED( pxQueue ) \
	if (trcIS_QUEUE_VALID(pxQueue)) \
	{ \
		switch ((pxQueue)->uxQueueType) \
		{ \
		case queueQUEUE_IS_QUEUE: \
			prvTraceEventCreate3(xPosition == queueSEND_TO_BACK ? PSF_EVENT_QUEUE_SEND_FAILED : PSF_EVENT_QUEUE_SEND_TO_FRONT_FAILED, pxQueue, xTicksToWait, (pxQueue)->uxItemsWaiting); \
			break; \
		case queueQUEUE_IS_SEMAPHORE: \
			prvTraceEventCreate2(PSF_EVENT_SEMAPHORE_SEND_FAILED, pxQueue, (pxQueue)->uxItemsWaiting); \
			break; \
		case queueQUEUE_IS_MUTEX: \
			prvTraceEventCreate2(PSF_EVENT_MUTEX_SEND_FAILED, pxQueue, (pxQueue)->uxItemsWaiting); \
			break; \
		} \
	} \
	else \
	{ \
		prvTraceEventCreate3(PSF_EVENT_QUEUE_SEND_FAILED, pxQueue, xTicksToWait, 0); \
	}

/* Called when the task is blocked due to a send operation on a full queue */
/* Semaphores and mutexes don't block on send */
#undef traceBLOCKING_ON_QUEUE_SEND
#define traceBLOCKING_ON_QUEUE_SEND( pxQueue ) \
	prvTraceEventCreate3(xPosition == queueSEND_TO_BACK ? PSF_EVENT_QUEUE_SEND_BLOCK : PSF_EVENT_QUEUE_SEND_TO_FRONT_BLOCK, pxQueue, xTicksToWait, (pxQueue)->uxItemsWaiting)

/* Called when a message is sent from interrupt context, e.g., using xQueueSendFromISR */
/* Mutexes can't be given from ISR */
#undef traceQUEUE_SEND_FROM_ISR
#define traceQUEUE_SEND_FROM_ISR( pxQueue ) \
	switch ((pxQueue)->uxQueueType) \
	{ \
	case queueQUEUE_IS_QUEUE: \
		prvTraceEventCreate2(xPosition == queueSEND_TO_BACK ? PSF_EVENT_QUEUE_SEND_FROMISR : PSF_EVENT_QUEUE_SEND_TO_FRONT_FROMISR, pxQueue, (pxQueue)->uxItemsWaiting + 1); \
		break; \
	case queueQUEUE_IS_SEMAPHORE: \
		prvTraceEventCreate2(PSF_EVENT_SEMAPHORE_SEND_FROMISR, pxQueue, (pxQueue)->uxItemsWaiting + 1); \
		break; \
	}

/* Called when a message send from interrupt context fails (since the queue was full) */
/* Mutexes can't be given from ISR */
#undef traceQUEUE_SEND_FROM_ISR_FAILED
#define traceQUEUE_SEND_FROM_ISR_FAILED( pxQueue ) \
	if (trcIS_QUEUE_VALID(pxQueue)) \
	{ \
		switch ((pxQueue)->uxQueueType) \
		{ \
		case queueQUEUE_IS_QUEUE: \
			prvTraceEventCreate2(xPosition == queueSEND_TO_BACK ? PSF_EVENT_QUEUE_SEND_FROMISR_FAILED : PSF_EVENT_QUEUE_SEND_TO_FRONT_FROMISR_FAILED, pxQueue, (pxQueue)->uxItemsWaiting); \
			break; \
		case queueQUEUE_IS_SEMAPHORE: \
			prvTraceEventCreate2(PSF_EVENT_SEMAPHORE_SEND_FROMISR_FAILED, pxQueue, (pxQueue)->uxItemsWaiting); \
			break; \
		} \
	} \
	else \
	{ \
		prvTraceEventCreate2(xPosition == queueSEND_TO_BACK ? PSF_EVENT_QUEUE_SEND_FROMISR_FAILED : PSF_EVENT_QUEUE_SEND_TO_FRONT_FROMISR_FAILED, pxQueue, 0); \
	}

/* Called when a message is received from a queue */
#undef traceQUEUE_RECEIVE
#define traceQUEUE_RECEIVE( pxQueue ) \
	switch ((pxQueue)->uxQueueType) \
	{ \
	case queueQUEUE_IS_QUEUE: \
		prvTraceEventCreate3(PSF_EVENT_QUEUE_RECEIVE, pxQueue, xTicksToWait, (pxQueue)->uxItemsWaiting - 1); \
		break; \
	case queueQUEUE_IS_SEMAPHORE: \
		prvTraceEventCreate3(PSF_EVENT_SEMAPHORE_RECEIVE, pxQueue, xTicksToWait, (pxQueue)->uxItemsWaiting - 1); \
		break; \
	case queueQUEUE_IS_MUTEX: \
		prvTraceEventCreate3(PSF_EVENT_MUTEX_RECEIVE, pxQueue, xTicksToWait, (pxQueue)->uxItemsWaiting - 1); \
		break; \
	}

/* Called when a receive operation on a queue fails (timeout) */
#undef traceQUEUE_RECEIVE_FAILED
#define traceQUEUE_RECEIVE_FAILED( pxQueue ) \
	if (trcIS_QUEUE_VALID(pxQueue)) \
	{ \
		switch ((pxQueue)->uxQueueType) \
		{ \
		case queueQUEUE_IS_QUEUE: \
			prvTraceEventCreate3(PSF_EVENT_QUEUE_RECEIVE_FAILED, pxQueue, xTicksToWait, (pxQueue)->uxItemsWaiting); \
			break; \
		case queueQUEUE_IS_SEMAPHORE: \
			prvTraceEventCreate3(PSF_EVENT_SEMAPHORE_RECEIVE_FAILED, pxQueue, xTicksToWait, (pxQueue)->uxItemsWaiting); \
			break; \
		case queueQUEUE_IS_MUTEX: \
			prvTraceEventCreate3(PSF_EVENT_MUTEX_RECEIVE_FAILED, pxQueue, xTicksToWait, (pxQueue)->uxItemsWaiting); \
			break; \
		} \
	} \
	else \
	{ \
			prvTraceEventCreate3(PSF_EVENT_QUEUE_RECEIVE_FAILED, pxQueue, xTicksToWait, 0); \
	}

/* Called when the task is blocked due to a receive operation on an empty queue */
#undef traceBLOCKING_ON_QUEUE_RECEIVE
#define traceBLOCKING_ON_QUEUE_RECEIVE( pxQueue ) \
	switch ((pxQueue)->uxQueueType) \
	{ \
	case queueQUEUE_IS_QUEUE: \
		prvTraceEventCreate3(PSF_EVENT_QUEUE_RECEIVE_BLOCK, pxQueue, xTicksToWait, (pxQueue)->uxItemsWaiting); \
		break; \
	case queueQUEUE_IS_SEMAPHORE: \
		prvTraceEventCreate3(PSF_EVENT_SEMAPHORE_RECEIVE_BLOCK, pxQueue, xTicksToWait, (pxQueue)->uxItemsWaiting); \
		break; \
	case queueQUEUE_IS_MUTEX: \
		prvTraceEventCreate3(PSF_EVENT_MUTEX_RECEIVE_BLOCK, pxQueue, xTicksToWait, (pxQueue)->uxItemsWaiting); \
		break; \
	}

/* Called when a message is received in interrupt context, e.g., using xQueueReceiveFromISR */
/* Mutexes can't be taken from ISR */
#undef traceQUEUE_RECEIVE_FROM_ISR
#define traceQUEUE_RECEIVE_FROM_ISR( pxQueue ) \
	switch ((pxQueue)->uxQueueType) \
	{ \
	case queueQUEUE_IS_QUEUE: \
		prvTraceEventCreate2(PSF_EVENT_QUEUE_RECEIVE_FROMISR, pxQueue, (pxQueue)->uxItemsWaiting - 1); \
		break; \
	case queueQUEUE_IS_SEMAPHORE: \
		prvTraceEventCreate2(PSF_EVENT_SEMAPHORE_RECEIVE_FROMISR, pxQueue, (pxQueue)->uxItemsWaiting - 1); \
		break; \
	}

/* Called when a message receive from interrupt context fails (since the queue was empty) */
/* Mutexes can't be taken from ISR */
#undef traceQUEUE_RECEIVE_FROM_ISR_FAILED
#define traceQUEUE_RECEIVE_FROM_ISR_FAILED( pxQueue ) \
	if (trcIS_QUEUE_VALID(pxQueue)) \
	{ \
		switch ((pxQueue)->uxQueueType) \
		{ \
		case queueQUEUE_IS_QUEUE: \
			prvTraceEventCreate2(PSF_EVENT_QUEUE_RECEIVE_FROMISR_FAILED, pxQueue, (pxQueue)->uxItemsWaiting); \
			break; \
		case queueQUEUE_IS_SEMAPHORE: \
			prvTraceEventCreate2(PSF_EVENT_SEMAPHORE_RECEIVE_FROMISR_FAILED, pxQueue, (pxQueue)->uxItemsWaiting); \
			break; \
		} \
	} \
	else \
	{ \
		prvTraceEventCreate2(PSF_EVENT_QUEUE_RECEIVE_FROMISR_FAILED, pxQueue, 0); \
	}

/* Called on xQueuePeek */
/* Can't Peek Semaphores or Mutexes */
#undef traceQUEUE_PEEK
#define traceQUEUE_PEEK( pxQueue ) \
	prvTraceEventCreate3(PSF_EVENT_QUEUE_PEEK, pxQueue, xTicksToWait, (pxQueue)->uxItemsWaiting)

/* Called on xQueuePeek */
/* Can't Peek Semaphores or Mutexes */
#undef traceQUEUE_PEEK_FAILED
#define traceQUEUE_PEEK_FAILED( pxQueue ) \
	if (trcIS_QUEUE_VALID(pxQueue)) \
	{ \
		prvTraceEventCreate3(PSF_EVENT_QUEUE_PEEK_FAILED, pxQueue, xTicksToWait, (pxQueue)->uxItemsWaiting); \
	} \
	else \
	{ \
		prvTraceEventCreate3(PSF_EVENT_QUEUE_PEEK_FAILED, pxQueue, xTicksToWait, 0); \
	}

/* Called on xQueuePeek */
/* Can't Peek Semaphores or Mutexes */
#undef traceBLOCKING_ON_QUEUE_PEEK
#define traceBLOCKING_ON_QUEUE_PEEK( pxQueue ) \
	prvTraceEventCreate3(PSF_EVENT_QUEUE_PEEK_BLOCK, pxQueue, xTicksToWait, (pxQueue)->uxItemsWaiting)

/* Called in xTaskPrioritySet */
#undef traceTASK_PRIORITY_SET
#define traceTASK_PRIORITY_SET( pxTask, uxNewPriority ) \
	xTraceTaskSetPriorityWithoutHandle(pxTask, uxNewPriority)

/* Called in vTaskPriorityInherit, which is called by Mutex operations */
#undef traceTASK_PRIORITY_INHERIT
#define traceTASK_PRIORITY_INHERIT( pxTask, uxNewPriority ) \
	prvTraceEventCreate2(PSF_EVENT_TASK_PRIO_INHERIT, pxTask, uxNewPriority)

/* Called in vTaskPriorityDisinherit, which is called by Mutex operations */
#undef traceTASK_PRIORITY_DISINHERIT
#define traceTASK_PRIORITY_DISINHERIT( pxTask, uxNewPriority ) \
	prvTraceEventCreate2(PSF_EVENT_TASK_PRIO_DISINHERIT, pxTask, uxNewPriority)

/* Called in vTaskResume */
#undef traceTASK_RESUME
#define traceTASK_RESUME( pxTaskToResume ) \
	prvTraceEventCreate1(PSF_EVENT_TASK_RESUME, pxTaskToResume)

/* Called in vTaskResumeFromISR */
#undef traceTASK_RESUME_FROM_ISR
#define traceTASK_RESUME_FROM_ISR( pxTaskToResume ) \
	prvTraceEventCreate1(PSF_EVENT_TASK_RESUME_FROMISR, pxTaskToResume)

/* Called in timer.c - xTimerCreate */
#undef traceTIMER_CREATE
#define traceTIMER_CREATE(tmr) \
	if (pdPASS == xReturn) \
	{ \
		xTraceObjectRegisterWithoutHandle(PSF_EVENT_TIMER_CREATE, (void*)(tmr), (const char*)(tmr)->pcTimerName, (uint32_t)(tmr)->xTimerPeriodInTicks); \
	} \
	else \
	{ \
		prvTraceEventCreate2(PSF_EVENT_TIMER_CREATE_FAILED, 0, 0); \
	}

#undef traceTIMER_COMMAND_SEND
#define traceTIMER_COMMAND_SEND(tmr, xCommandID, xOptionalValue, xReturn) \
	switch(xCommandID) \
	{ \
		case timerCOMMAND_START: \
			prvTraceEventCreate1(((xReturn) == pdPASS) ? PSF_EVENT_TIMER_START : PSF_EVENT_TIMER_START_FAILED, tmr); \
			break; \
		case timerCOMMAND_STOP: \
			prvTraceEventCreate1(((xReturn) == pdPASS) ? PSF_EVENT_TIMER_STOP : PSF_EVENT_TIMER_STOP_FAILED, tmr); \
			break; \
		case timerCOMMAND_CHANGE_PERIOD: \
			prvTraceEventCreate2(((xReturn) == pdPASS) ? PSF_EVENT_TIMER_CHANGEPERIOD : PSF_EVENT_TIMER_CHANGEPERIOD_FAILED, tmr, xOptionalValue); \
			break; \
		case timerCOMMAND_DELETE: \
			prvTraceEventCreate1(((xReturn) == pdPASS) ? PSF_EVENT_TIMER_DELETE : PSF_EVENT_TIMER_DELETE_FAILED, tmr); \
			break; \
	}

#undef traceTIMER_COMMAND_SEND_FROM_ISR
#define traceTIMER_COMMAND_SEND_FROM_ISR(tmr, xCommandID, xOptionalValue, xReturn) \
	switch(xCommandID) \
	{ \
		case timerCOMMAND_START: \
			prvTraceEventCreate1(((xReturn) == pdPASS) ? PSF_EVENT_TIMER_START_FROMISR : PSF_EVENT_TIMER_START_FROMISR_FAILED, tmr); \
			break; \
		case timerCOMMAND_STOP: \
			prvTraceEventCreate1(((xReturn) == pdPASS) ? PSF_EVENT_TIMER_STOP_FROMISR : PSF_EVENT_TIMER_STOP_FROMISR_FAILED, tmr); \
			break; \
		case timerCOMMAND_CHANGE_PERIOD: \
			prvTraceEventCreate2(((xReturn) == pdPASS) ? PSF_EVENT_TIMER_CHANGEPERIOD_FROMISR : PSF_EVENT_TIMER_CHANGEPERIOD_FROMISR_FAILED, tmr, xOptionalValue); \
			break; \
	}

#undef traceTIMER_COMMAND_PROCESS
#define traceTIMER_COMMAND_PROCESS( tmr, xCommandID, xOptionalValue ) \
	switch(xCommandID) \
	{ \
		case timerCOMMAND_START: \
			prvTraceEventCreate1(PSF_EVENT_TIMER_PROCESS_START, tmr); \
			break; \
		case timerCOMMAND_STOP: \
			prvTraceEventCreate1(PSF_EVENT_TIMER_PROCESS_STOP, tmr); \
			break; \
		case timerCOMMAND_CHANGE_PERIOD: \
			prvTraceEventCreate2(PSF_EVENT_TIMER_PROCESS_CHANGEPERIOD, tmr, xOptionalValue); \
			break; \
		case timerCOMMAND_DELETE: \
			xTraceObjectUnregisterWithoutHandle(PSF_EVENT_TIMER_PROCESS_DELETE, (void*)(tmr), 0); \
			break; \
	}

#undef traceTIMER_CALLBACK
#define traceTIMER_CALLBACK( tmr, callbackFunction ) \
	prvTraceEventCreate2(PSF_EVENT_TIMER_CALLBACK, tmr, callbackFunction)

#undef traceEVENT_GROUP_CREATE
#define traceEVENT_GROUP_CREATE(eg) \
	xTraceObjectRegisterWithoutHandle(PSF_EVENT_EVENTGROUP_CREATE, (void*)(eg), "", 0)

#undef traceEVENT_GROUP_CREATE_FAILED
#define traceEVENT_GROUP_CREATE_FAILED() \
	prvTraceEventCreate2(PSF_EVENT_EVENTGROUP_CREATE_FAILED, 0, 0)

#undef traceEVENT_GROUP_DELETE
#define traceEVENT_GROUP_DELETE(eg) \
	xTraceObjectUnregisterWithoutHandle(PSF_EVENT_EVENTGROUP_DELETE, (void*)(eg), ((eventGroupType*)(eg))->xEventBits)

#undef traceEVENT_GROUP_DELETE_FAILED
#define traceEVENT_GROUP_DELETE_FAILED(eg) \
	prvTraceEventCreate2(PSF_EVENT_EVENTGROUP_DELETE_FAILED, eg, 0)

#undef traceEVENT_GROUP_SET_BITS
#define traceEVENT_GROUP_SET_BITS(eg, bitsToSet) \
	prvTraceEventCreate2(PSF_EVENT_EVENTGROUP_SETBITS, eg, bitsToSet)

#undef traceEVENT_GROUP_SET_BITS_FAILED
#define traceEVENT_GROUP_SET_BITS_FAILED(eg, bitsToSet) \
	prvTraceEventCreate2(PSF_EVENT_EVENTGROUP_SETBITS_FAILED, eg, bitsToSet)

#undef traceEVENT_GROUP_SET_BITS_FROM_ISR
#define traceEVENT_GROUP_SET_BITS_FROM_ISR(eg, bitsToSet) \
	prvTraceEventCreate2(PSF_EVENT_EVENTGROUP_SETBITS_FROMISR, eg, bitsToSet)

#undef traceEVENT_GROUP_SET_BITS_FROM_ISR_FAILED
#define traceEVENT_GROUP_SET_BITS_FROM_ISR_FAILED(eg, bitsToSet) \
	prvTraceEventCreate2(PSF_EVENT_EVENTGROUP_SETBITS_FROMISR_FAILED, eg, bitsToSet)

#undef traceEVENT_GROUP_WAIT_BITS
#define traceEVENT_GROUP_WAIT_BITS(eg, bitsToWaitFor, timeout) \
	prvTraceEventCreate3(PSF_EVENT_EVENTGROUP_WAITBITS, eg, bitsToWaitFor, timeout)

#undef traceEVENT_GROUP_WAIT_BITS_FAILED
#define traceEVENT_GROUP_WAIT_BITS_FAILED(eg, bitsToWaitFor, timeout) \
	prvTraceEventCreate3(PSF_EVENT_EVENTGROUP_WAITBITS_FAILED, eg, bitsToWaitFor, timeout)

#undef traceEVENT_GROUP_WAIT_BITS_BLOCK
#define traceEVENT_GROUP_WAIT_BITS_BLOCK(eg, bitsToWaitFor, timeout) \
	prvTraceEventCreate3(PSF_EVENT_EVENTGROUP_WAITBITS_BLOCK, eg, bitsToWaitFor, timeout)

#undef traceEVENT_GROUP_CLEAR_BITS
#define traceEVENT_GROUP_CLEAR_BITS(eg, bitsToClear) \
	prvTraceEventCreate2(PSF_EVENT_EVENTGROUP_CLEARBITS, eg, bitsToClear)

#undef traceEVENT_GROUP_CLEAR_BITS_FAILED
#define traceEVENT_GROUP_CLEAR_BITS_FAILED(eg, bitsToClear) \
	prvTraceEventCreate2(PSF_EVENT_EVENTGROUP_CLEARBITS_FAILED, eg, bitsToClear)

#undef traceEVENT_GROUP_CLEAR_BITS_FROM_ISR
#define traceEVENT_GROUP_CLEAR_BITS_FROM_ISR(eg, bitsToClear) \
	prvTraceEventCreate2(PSF_EVENT_EVENTGROUP_CLEARBITS_FROMISR, eg, bitsToClear)

#undef traceEVENT_GROUP_CLEAR_BITS_FROM_ISR_FAILED
#define traceEVENT_GROUP_CLEAR_BITS_FROM_ISR_FAILED(eg, bitsToClear) \
	prvTraceEventCreate2(PSF_EVENT_EVENTGROUP_CLEARBITS_FROMISR_FAILED, eg, bitsToClear)

#undef traceTASK_NOTIFY_WAIT
#define traceTASK_NOTIFY_WAIT() \
	prvTraceEventCreate2(PSF_EVENT_TASK_NOTIFY_WAIT, pxCurrentTCB, xTicksToWait)

#undef traceTASK_NOTIFY_WAIT_FAILED
#define traceTASK_NOTIFY_WAIT_FAILED() \
	prvTraceEventCreate2(PSF_EVENT_TASK_NOTIFY_WAIT_FAILED, pxCurrentTCB, xTicksToWait)

#undef traceTASK_NOTIFY_WAIT_BLOCK
#define traceTASK_NOTIFY_WAIT_BLOCK() \
	prvTraceEventCreate2(PSF_EVENT_TASK_NOTIFY_WAIT_BLOCK, pxCurrentTCB, xTicksToWait)

#undef traceTASK_NOTIFY_SEND
#define traceTASK_NOTIFY_SEND() \
	prvTraceEventCreate2(PSF_EVENT_TASK_NOTIFY_SEND, xTaskToNotify, uxValue)

#undef traceTASK_NOTIFY_SEND_FAILED
#define traceTASK_NOTIFY_SEND_FAILED() \
	prvTraceEventCreate2(PSF_EVENT_TASK_NOTIFY_SEND_FAILED, xTaskToNotify, uxValue)

#undef traceTASK_NOTIFY_SEND_FROM_ISR
#define traceTASK_NOTIFY_SEND_FROM_ISR() \
	prvTraceEventCreate2(PSF_EVENT_TASK_NOTIFY_SEND_FROM_ISR, xTaskToNotify, uxValue)

#undef traceTASK_NOTIFY_SEND_FROM_ISR_FAILED
#define traceTASK_NOTIFY_SEND_FROM_ISR_FAILED() \
	prvTraceEventCreate2(PSF_EVENT_TASK_NOTIFY_SEND_FROM_ISR_FAILED, xTaskToNotify, uxValue)

/* StreamBuffer and MessageBuffer event handlers */
#undef traceSTREAM_BUFFER_CREATE
#define traceSTREAM_BUFFER_CREATE( xStreamBuffer, uxIsMessageBuffer )                                       \
    if( pdFALSE == uxIsMessageBuffer )                                                                      \
    {                                                                                                       \
        xTraceObjectRegisterWithoutHandle(PSF_EVENT_STREAMBUFFER_CREATE, (void*)(xStreamBuffer), "", 0);    \
    }                                                                                                       \
    else                                                                                                    \
    {                                                                                                       \
        xTraceObjectRegisterWithoutHandle(PSF_EVENT_MESSAGEBUFFER_CREATE, (void*)(xStreamBuffer), "", 0);   \
    }

#undef traceSTREAM_BUFFER_CREATE_FAILED
#define traceSTREAM_BUFFER_CREATE_FAILED( uxIsMessageBuffer )                                               \
    if( pdFALSE == uxIsMessageBuffer )                                                                      \
    {                                                                                                       \
        prvTraceEventCreate0(PSF_EVENT_STREAMBUFFER_CREATE_FAILED);                                      \
    }                                                                                                       \
    else                                                                                                    \
    {                                                                                                       \
        prvTraceEventCreate0(PSF_EVENT_MESSAGEBUFFER_CREATE_FAILED);                                     \
    }

#undef traceSTREAM_BUFFER_RESET
#define traceSTREAM_BUFFER_RESET( xStreamBuffer, uxIsMessageBuffer )                                        \
    if( pdFALSE == uxIsMessageBuffer )                                                                      \
    {                                                                                                       \
        prvTraceEventCreate2(PSF_EVENT_STREAMBUFFER_RESET, xStreamBuffer, 0);                     \
    }                                                                                                       \
    else                                                                                                    \
    {                                                                                                       \
        prvTraceEventCreate2(PSF_EVENT_MESSAGEBUFFER_RESET, xStreamBuffer, 0);                    \
    }

#undef traceSTREAM_BUFFER_SEND
#define traceSTREAM_BUFFER_SEND( xStreamBuffer, uxIsMessageBuffer )                                                 \
    if( pdFALSE == uxIsMessageBuffer )                                                                              \
    {                                                                                                               \
        prvTraceEventCreate2(PSF_EVENT_STREAMBUFFER_SEND, xStreamBuffer, xStreamBuffer->uxBytesStored );  \
    }                                                                                                               \
    else                                                                                                            \
    {                                                                                                               \
        prvTraceEventCreate2(PSF_EVENT_MESSAGEBUFFER_SEND, xStreamBuffer, xStreamBuffer->uxBytesStored ); \
    }

#undef traceSTREAM_BUFFER_BLOCKING_ON_SEND
#define traceSTREAM_BUFFER_BLOCKING_ON_SEND( xStreamBuffer, uxIsMessageBuffer )                                             \
    if( pdFALSE == uxIsMessageBuffer )                                                                                      \
    {                                                                                                                       \
        prvTraceEventCreate2(PSF_EVENT_STREAMBUFFER_SEND_BLOCK, xStreamBuffer, xStreamBuffer->uxBytesStored );    \
    }                                                                                                                       \
    else                                                                                                                    \
    {                                                                                                                       \
        prvTraceEventCreate2(PSF_EVENT_MESSAGEBUFFER_SEND_BLOCK, xStreamBuffer, xStreamBuffer->uxBytesStored );   \
    }

#undef traceSTREAM_BUFFER_SEND_FAILED
#define traceSTREAM_BUFFER_SEND_FAILED( xStreamBuffer, uxIsMessageBuffer )                                                  \
    if( pdFALSE == uxIsMessageBuffer )                                                                                      \
    {                                                                                                                       \
        prvTraceEventCreate2(PSF_EVENT_STREAMBUFFER_SEND_FAILED, xStreamBuffer, xStreamBuffer->uxBytesStored );   \
    }                                                                                                                       \
    else                                                                                                                    \
    {                                                                                                                       \
        prvTraceEventCreate2(PSF_EVENT_MESSAGEBUFFER_SEND_FAILED, xStreamBuffer, xStreamBuffer->uxBytesStored );  \
    }

#undef traceSTREAM_BUFFER_RECEIVE
#define traceSTREAM_BUFFER_RECEIVE( xStreamBuffer, uxIsMessageBuffer )                                                  \
    if( pdFALSE == uxIsMessageBuffer )                                                                                  \
    {                                                                                                                   \
        prvTraceEventCreate2(PSF_EVENT_STREAMBUFFER_RECEIVE, xStreamBuffer, xStreamBuffer->uxBytesStored );   \
    }                                                                                                                   \
    else                                                                                                                \
    {                                                                                                                   \
        prvTraceEventCreate2(PSF_EVENT_MESSAGEBUFFER_RECEIVE, xStreamBuffer, xStreamBuffer->uxBytesStored );  \
    }

#undef traceSTREAM_BUFFER_BLOCKING_ON_RECEIVE
#define traceSTREAM_BUFFER_BLOCKING_ON_RECEIVE( xStreamBuffer, uxIsMessageBuffer )                                              \
    if( pdFALSE == uxIsMessageBuffer )                                                                                          \
    {                                                                                                                           \
        prvTraceEventCreate2(PSF_EVENT_STREAMBUFFER_RECEIVE_BLOCK, xStreamBuffer, xStreamBuffer->uxBytesStored );     \
    }                                                                                                                           \
    else                                                                                                                        \
    {                                                                                                                           \
        prvTraceEventCreate2(PSF_EVENT_MESSAGEBUFFER_RECEIVE_BLOCK, xStreamBuffer, xStreamBuffer->uxBytesStored );    \
    }

#undef traceSTREAM_BUFFER_RECEIVE_FAILED
#define traceSTREAM_BUFFER_RECEIVE_FAILED( xStreamBuffer, uxIsMessageBuffer )                                                   \
    if( pdFALSE == uxIsMessageBuffer )                                                                                          \
    {                                                                                                                           \
        prvTraceEventCreate2(PSF_EVENT_STREAMBUFFER_RECEIVE_FAILED, xStreamBuffer, xStreamBuffer->uxBytesStored );    \
    }                                                                                                                           \
    else                                                                                                                        \
    {                                                                                                                           \
        prvTraceEventCreate2(PSF_EVENT_MESSAGEBUFFER_RECEIVE_FAILED, xStreamBuffer, xStreamBuffer->uxBytesStored );   \
    }

#undef traceSTREAM_BUFFER_SEND_FROM_ISR
#define traceSTREAM_BUFFER_SEND_FROM_ISR( xStreamBuffer, uxIsMessageBuffer )                                                    \
    if( pdFALSE == uxIsMessageBuffer )                                                                                          \
    {                                                                                                                           \
        prvTraceEventCreate2(PSF_EVENT_STREAMBUFFER_SEND_FROM_ISR, xStreamBuffer, xStreamBuffer->uxBytesStored );     \
    }                                                                                                                           \
    else                                                                                                                        \
    {                                                                                                                           \
        prvTraceEventCreate2(PSF_EVENT_MESSAGEBUFFER_SEND_FROM_ISR, xStreamBuffer, xStreamBuffer->uxBytesStored );    \
    }

#undef traceSTREAM_BUFFER_SEND_FROM_ISR_FAILED
#define traceSTREAM_BUFFER_SEND_FROM_ISR_FAILED( xStreamBuffer, uxIsMessageBuffer )                                                 \
    if( pdFALSE == uxIsMessageBuffer )                                                                                              \
    {                                                                                                                               \
        prvTraceEventCreate2(PSF_EVENT_STREAMBUFFER_SEND_FROM_ISR_FAILED, xStreamBuffer, xStreamBuffer->uxBytesStored );  \
    }                                                                                                                               \
    else                                                                                                                            \
    {                                                                                                                               \
        prvTraceEventCreate2(PSF_EVENT_MESSAGEBUFFER_SEND_FROM_ISR_FAILED, xStreamBuffer, xStreamBuffer->uxBytesStored ); \
    }

#undef traceSTREAM_BUFFER_RECEIVE_FROM_ISR
#define traceSTREAM_BUFFER_RECEIVE_FROM_ISR( xStreamBuffer, uxIsMessageBuffer )                                                 \
    if( pdFALSE == uxIsMessageBuffer )                                                                                          \
    {                                                                                                                           \
        prvTraceEventCreate2(PSF_EVENT_STREAMBUFFER_RECEIVE_FROM_ISR, xStreamBuffer, xStreamBuffer->uxBytesStored );  \
    }                                                                                                                           \
    else                                                                                                                        \
    {                                                                                                                           \
        prvTraceEventCreate2(PSF_EVENT_MESSAGEBUFFER_RECEIVE_FROM_ISR, xStreamBuffer, xStreamBuffer->uxBytesStored ); \
    }

#undef traceSTREAM_BUFFER_RECEIVE_FROM_ISR_FAILED
#define traceSTREAM_BUFFER_RECEIVE_FROM_ISR_FAILED( xStreamBuffer, uxIsMessageBuffer )                                                  \
    if( pdFALSE == uxIsMessageBuffer )                                                                                                  \
    {                                                                                                                                   \
        prvTraceEventCreate2(PSF_EVENT_STREAMBUFFER_RECEIVE_FROM_ISR_FAILED, xStreamBuffer, xStreamBuffer->uxBytesStored );   \
    }                                                                                                                                   \
    else                                                                                                                                \
    {                                                                                                                                   \
        prvTraceEventCreate2(PSF_EVENT_MESSAGEBUFFER_RECEIVE_FROM_ISR_FAILED, xStreamBuffer, xStreamBuffer->uxBytesStored );  \
    }

/* EvtMplx event handlers */
#undef traceEVTMPLX_CREATE
#define traceEVTMPLX_CREATE(em) \
    xTraceObjectRegisterWithoutHandle(PSF_EVENT_EVTMPLX_CREATE, (void*)(em), "", 0)

#undef traceEVTMPLX_CREATE_FAILED
#define traceEVTMPLX_CREATE_FAILED() \
    prvTraceEventCreate0(PSF_EVENT_EVTMPLX_CREATE_FAILED)

#undef traceEVTMPLX_ADD_OBJECT_EVENTS
#define traceEVTMPLX_ADD_OBJECT_EVENTS(em) \
    prvTraceEventCreate1( PSF_EVENT_EVTMPLX_ADD_OBJECT, em )

#undef traceEVTMPLX_ADD_OBJECT_EVENTS_FAILED
#define traceEVTMPLX_ADD_OBJECT_EVENTS_FAILED(em) \
    prvTraceEventCreate1( PSF_EVENT_EVTMPLX_ADD_OBJECT_FAILED, em )

#undef traceEVTMPLX_MODIFY_OBJECT_EVENTS
#define traceEVTMPLX_MODIFY_OBJECT_EVENTS(em) \
    prvTraceEventCreate1( PSF_EVENT_EVTMPLX_MODIFY_OBJECT, em )

#undef traceEVTMPLX_MODIFY_OBJECT_EVENTS_FAILED
#define traceEVTMPLX_MODIFY_OBJECT_EVENTS_FAILED(em) \
    prvTraceEventCreate1( PSF_EVENT_EVTMPLX_MODIFY_OBJECT_FAILED, em )

#undef traceEVTMPLX_REMOVE_OBJECT_EVENTS
#define traceEVTMPLX_REMOVE_OBJECT_EVENTS(em) \
    prvTraceEventCreate1( PSF_EVENT_EVTMPLX_REMOVE_OBJECT, em )

#undef traceEVTMPLX_REMOVE_OBJECT_EVENTS_FAILED
#define traceEVTMPLX_REMOVE_OBJECT_EVENTS_FAILED(em) \
    prvTraceEventCreate1( PSF_EVENT_EVTMPLX_REMOVE_OBJECT_FAILED, em )

#undef traceEVTMPLX_WAIT
#define traceEVTMPLX_WAIT(em) \
    prvTraceEventCreate1( PSF_EVENT_EVTMPLX_WAIT, em )

#undef traceEVTMPLX_WAIT_FAILED
#define traceEVTMPLX_WAIT_FAILED(em) \
    prvTraceEventCreate1( PSF_EVENT_EVTMPLX_WAIT_FAILED, em )

#undef traceEVTMPLX_WAIT_BLOCK
#define traceEVTMPLX_WAIT_BLOCK(em) \
    prvTraceEventCreate1( PSF_EVENT_EVTMPLX_WAIT_BLOCK, em )

#undef traceEVTMPLX_UPDATE
#define traceEVTMPLX_UPDATE(em) \
    prvTraceEventCreate1( PSF_EVENT_EVTMPLX_UPDATE, em )

#undef traceEVTMPLX_DELETE
#define traceEVTMPLX_DELETE(em) \
    xTraceObjectUnregisterWithoutHandle(PSF_EVENT_EVTMPLX_DELETE, (void*)(em), 0 )

#endif /* (TRC_CFG_SCHEDULING_ONLY == 0) */

#if (TRC_CFG_SCHEDULING_ONLY == 1)
#define traceLOW_POWER_IDLE_BEGIN()
#define traceLOW_POWER_IDLE_END()
#define traceTASK_SUSPEND( pxTaskToSuspend )
#define traceTASK_DELAY()
#define traceTASK_DELAY_UNTIL(xTimeToWake)
#define traceQUEUE_CREATE( pxNewQueue )
#define traceQUEUE_CREATE_FAILED( pcBuffer, queueType, queueLength )
#define traceQUEUE_SEND( pxQueue )
#define traceQUEUE_SEND_FAILED( pxQueue )
#define traceBLOCKING_ON_QUEUE_SEND( pxQueue )
#define traceQUEUE_SEND_FROM_ISR( pxQueue )
#define traceQUEUE_SEND_FROM_ISR_FAILED( pxQueue )
#define traceQUEUE_RECEIVE( pxQueue )
#define traceQUEUE_RECEIVE_FAILED( pxQueue )
#define traceBLOCKING_ON_QUEUE_RECEIVE( pxQueue )
#define traceQUEUE_RECEIVE_FROM_ISR( pxQueue )
#define traceQUEUE_RECEIVE_FROM_ISR_FAILED( pxQueue )
#define traceQUEUE_PEEK( pxQueue )
#define traceQUEUE_PEEK_FAILED( pxQueue )
#define traceBLOCKING_ON_QUEUE_PEEK( pxQueue )
#define traceTASK_PRIORITY_SET( pxTask, uxNewPriority )
#define traceTASK_PRIORITY_INHERIT( pxTask, uxNewPriority )
#define traceTASK_PRIORITY_DISINHERIT( pxTask, uxNewPriority )
#define traceTASK_RESUME( pxTaskToResume )
#define traceTASK_RESUME_FROM_ISR( pxTaskToResume )
#define traceTIMER_CREATE(tmr)
#define traceTIMER_COMMAND_SEND(tmr, xCommandID, xOptionalValue, xReturn)
#define traceTIMER_COMMAND_SEND_FROM_ISR(tmr, xCommandID, xOptionalValue, xReturn)
#define traceTIMER_COMMAND_PROCESS( tmr, xCommandID, xOptionalValue )
#define traceTIMER_CALLBACK( tmr, callbackFunction )
#define traceEVENT_GROUP_CREATE(eg)
#define traceEVENT_GROUP_CREATE_FAILED()
#define traceEVENT_GROUP_DELETE(eg)
#define traceEVENT_GROUP_DELETE_FAILED(eg)
#define traceEVENT_GROUP_SET_BITS(eg, bitsToSet)
#define traceEVENT_GROUP_SET_BITS_FAILED(eg, bitsToSet)
#define traceEVENT_GROUP_SET_BITS_FROM_ISR(eg, bitsToSet)
#define traceEVENT_GROUP_SET_BITS_FROM_ISR_FAILED(eg, bitsToSet)
#define traceEVENT_GROUP_WAIT_BITS(eg, bitsToWaitFor, timeout)
#define traceEVENT_GROUP_WAIT_BITS_FAILED(eg, bitsToWaitFor, timeout)
#define traceEVENT_GROUP_WAIT_BITS_BLOCK(eg, bitsToWaitFor, timeout)
#define traceEVENT_GROUP_CLEAR_BITS(eg, bitsToClear)
#define traceEVENT_GROUP_CLEAR_BITS_FAILED(eg, bitsToClear)
#define traceEVENT_GROUP_CLEAR_BITS_FROM_ISR(eg, bitsToClear)
#define traceEVENT_GROUP_CLEAR_BITS_FROM_ISR_FAILED(eg, bitsToClear)
#define traceTASK_NOTIFY_WAIT()
#define traceTASK_NOTIFY_WAIT_FAILED()
#define traceTASK_NOTIFY_WAIT_BLOCK()
#define traceTASK_NOTIFY_SEND()
#define traceTASK_NOTIFY_SEND_FAILED()
#define traceTASK_NOTIFY_SEND_FROM_ISR()
#define traceTASK_NOTIFY_SEND_FROM_ISR_FAILED()
#define traceSTREAM_BUFFER_CREATE( xStreamBuffer, uxIsMessageBuffer )
#define traceSTREAM_BUFFER_CREATE_FAILED( uxIsMessageBuffer )
#define traceSTREAM_BUFFER_RESET( xStreamBuffer, uxIsMessageBuffer )
#define traceSTREAM_BUFFER_SEND( xStreamBuffer, uxIsMessageBuffer )
#define traceSTREAM_BUFFER_BLOCKING_ON_SEND( xStreamBuffer, uxIsMessageBuffer )
#define traceSTREAM_BUFFER_SEND_FAILED( xStreamBuffer, uxIsMessageBuffer )
#define traceSTREAM_BUFFER_RECEIVE( xStreamBuffer, uxIsMessageBuffer )
#define traceSTREAM_BUFFER_BLOCKING_ON_RECEIVE( xStreamBuffer, uxIsMessageBuffer )
#define traceSTREAM_BUFFER_RECEIVE_FAILED( xStreamBuffer, uxIsMessageBuffer )
#define traceSTREAM_BUFFER_SEND_FROM_ISR( xStreamBuffer, uxIsMessageBuffer )
#define traceSTREAM_BUFFER_SEND_FROM_ISR_FAILED( xStreamBuffer, uxIsMessageBuffer )
#define traceSTREAM_BUFFER_RECEIVE_FROM_ISR( xStreamBuffer, uxIsMessageBuffer )
#define traceSTREAM_BUFFER_RECEIVE_FROM_ISR_FAILED( xStreamBuffer, uxIsMessageBuffer )
#define traceEVTMPLX_CREATE(em)
#define traceEVTMPLX_CREATE_FAILED()
#define traceEVTMPLX_ADD_OBJECT_EVENTS(em)
#define traceEVTMPLX_ADD_OBJECT_EVENTS_FAILED(em)
#define traceEVTMPLX_MODIFY_OBJECT_EVENTS(em)
#define traceEVTMPLX_MODIFY_OBJECT_EVENTS_FAILED(em)
#define traceEVTMPLX_REMOVE_OBJECT_EVENTS(em)
#define traceEVTMPLX_REMOVE_OBJECT_EVENTS_FAILED(em)
#define traceEVTMPLX_WAIT(em)
#define traceEVTMPLX_WAIT_FAILED(em)
#define traceEVTMPLX_WAIT_BLOCK(em)
#define traceEVTMPLX_UPDATE_TASK(em)
#define traceEVTMPLX_UPDATE_QUEUE(em)
#define traceEVTMPLX_UPDATE_EVENTGROUP(em)
#endif /* (TRC_CFG_SCHEDULING_ONLY == 1) */

#else /*(TRC_USE_TRACEALYZER_RECORDER == 1)*/

/* When recorder is disabled */
#define vTraceSetQueueName(object, name)
#define vTraceSetSemaphoreName(object, name)
#define vTraceSetMutexName(object, name)
#define vTraceSetEventGroupName(object, name)
#define vTraceSetStreamBufferName(object, name)
#define vTraceSetMessageBufferName(object, name)
#define vTraceSetEvtMplxName(object, name)

#endif /*(TRC_USE_TRACEALYZER_RECORDER == 1)*/

#ifdef __cplusplus
}
#endif

#endif /* TRC_KERNEL_PORT_H */
