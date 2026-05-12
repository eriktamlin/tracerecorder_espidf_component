/*
 * Percepio TraceRecorder for Tracealyzer v989.878.767
 * Copyright 2025 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * uC/OS3 specific definitions needed by the trace recorder
 */

#ifndef TRC_KERNEL_PORT_H
#define TRC_KERNEL_PORT_H

#include <stdint.h>
#include <os.h>
#include <os_cfg.h>	                                        /* Defines OS_CFG_TRACE_EN (1: Enabled).               */
#include <os_cfg_app.h>	                                    /* Defines OS_CFG_TICK_RATE_HZ.                        */
#if(TRC_BSP_CLK_SOURCE == 1)
#include <bsp.h>
#endif
#include <trcDefines.h>

#ifdef __cplusplus
extern "C" {
#endif

#if (OS_VERSION >= 30600u)
#if (defined(OS_CFG_TRACE_EN) && (OS_CFG_TRACE_EN > 0u))
  #define TRC_USE_TRACEALYZER_RECORDER   1
#else
  #define TRC_USE_TRACEALYZER_RECORDER   0
#endif
#else /* (OS_VERSION >= 30600u) */
#if (defined(TRACE_CFG_EN) && (TRACE_CFG_EN > 0u))
  #define TRC_USE_TRACEALYZER_RECORDER   1
#else
  #define TRC_USE_TRACEALYZER_RECORDER   0
#endif
#endif /* (OS_VERSION >= 30600u) */

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

/**
 * @def TRC_PLATFORM_CFG
 * @brief DO NOT CHANGE THIS
 * This defines the basis for version specific lookup of
 * platform configuration files.
 */
#define TRC_PLATFORM_CFG "uC-OS3"

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

#if defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1) && (TRC_CFG_SCHEDULING_ONLY == 0)
	/* Required for this feature */
#undef OS_CFG_STAT_TASK_STK_CHK_EN
#define OS_CFG_STAT_TASK_STK_CHK_EN DEF_ENABLED
#endif /* defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1) && (TRC_CFG_SCHEDULING_ONLY == 0) */

#define TRACE_KERNEL_VERSION 0x2AA2
#define TRC_TICK_RATE_HZ  OS_CFG_TICK_RATE_HZ            /* OS tick rate in Hertz.           */

#if(TRC_BSP_CLK_SOURCE == 1 && OS_VERSION >= 30700u)
#define TRACE_CPU_CLOCK_HZ BSP_ClkFreqGet(CLK_ID_SYSCLK) 
#elif(TRC_BSP_CLK_SOURCE == 1 && OS_VERSION < 30700u)
#define TRACE_CPU_CLOCK_HZ  BSP_CPU_ClkFreq()              /* Peripheral clock speed in Hertz. */
#elif(TRC_BSP_CLK_SOURCE == 0)
/* Fallback */
#ifndef TRACE_CPU_CLOCK_HZ
#define TRACE_CPU_CLOCK_HZ TRC_HWTC_FREQ_HZ
#endif
#endif

/* Gives the currently executing task (wrapper for RTOS-specific function) */
void* prvTraceGetCurrentTaskHandle(void);
char* prvTraceAppend(const char* name, const char* suffix);

#if defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1)
traceResult xTraceKernelPortGetUnusedStack(void* task, TraceUnsignedBaseType_t* puxUnusedStack);
#endif /* defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1)*/

#define xTraceKernelPortGetSystemHeapHandle() 0

#include <trcHeap.h>

#define TRC_KERNEL_PORT_BUFFER_SIZE (sizeof(TraceUnsignedBaseType_t))

typedef struct TraceKernelPortDataBuffer	/* Aligned */
{
	uint8_t buffer[TRC_KERNEL_PORT_BUFFER_SIZE];
} TraceKernelPortDataBuffer_t;

traceResult xTraceKernelPortEnable(void);

traceResult xTraceKernelPortInitialize(TraceKernelPortDataBuffer_t* pxBuffer);

traceResult xTraceKernelPortDelay(uint32_t uiTicks);

unsigned char xTraceKernelPortIsSchedulerSuspended(void);

/* We have to override the default critical section status variable name for this kernel */
#undef TRACE_ALLOC_CRITICAL_SECTION_NAME
/* These may need to be set based on kernel version */
#define TRACE_ALLOC_CRITICAL_SECTION_NAME cpu_sr
#define TRACE_ALLOC_CRITICAL_SECTION_TYPE CPU_SR

#define TRC_KERNEL_PORT_ALLOC_CRITICAL_SECTION() TRACE_ALLOC_CRITICAL_SECTION_TYPE TRACE_ALLOC_CRITICAL_SECTION_NAME;

#define TRC_KERNEL_PORT_ENTER_CRITICAL_SECTION() TRACE_ALLOC_CRITICAL_SECTION_NAME = 0; CPU_CRITICAL_ENTER()

#define TRC_KERNEL_PORT_EXIT_CRITICAL_SECTION() (void)TRACE_ALLOC_CRITICAL_SECTION_NAME; CPU_CRITICAL_EXIT()

#define TRC_KERNEL_PORT_SET_INTERRUPT_MASK() (TRC_KERNEL_PORT_ENTER_CRITICAL_SECTION(), 0)

#define TRC_KERNEL_PORT_CLEAR_INTERRUPT_MASK(xMask) (void)xMask; TRC_KERNEL_PORT_EXIT_CRITICAL_SECTION()

#define TRACE_GET_CURRENT_TASK() prvTraceGetCurrentTaskHandle()

#if (TRC_CFG_RECORDER_BUFFER_ALLOCATION == TRC_RECORDER_BUFFER_ALLOCATION_DYNAMIC)
/**
 * @internal Kernel port specific heap initialization
 */
#define TRC_KERNEL_PORT_HEAP_INIT(size)

/**
 * @internal Kernel port specific heap malloc definition
 */
#define TRC_KERNEL_PORT_HEAP_MALLOC(size) pvPortMalloc(size)
#endif /* (TRC_CFG_RECORDER_BUFFER_ALLOCATION == TRC_RECORDER_BUFFER_ALLOCATION_DYNAMIC) */

extern uint16_t CurrentFilterMask;

extern uint16_t CurrentFilterGroup;

#if (OS_VERSION < 30700u)
extern void* prvTrcTickTask;
#endif /* (OS_VERSION < 30700u) */

#if !defined(TRC_CFG_INCLUDE_TICK_TASK)
#define TRC_CFG_INCLUDE_TICK_TASK 0
#endif

#if (TRC_CFG_INCLUDE_TICK_TASK == 0) && (OS_VERSION < 30700u)
#define TRACE_IF_NOT_TICK_TASK(p_tcb) if (prvTrcTickTask != (void*)p_tcb)
#else /* (TRC_CFG_INCLUDE_TICK_TASK == 0) && (OS_VERSION < 30700u) */
#define TRACE_IF_NOT_TICK_TASK(p_tcb)
#endif /* (TRC_CFG_INCLUDE_TICK_TASK == 0) && (OS_VERSION < 30700u) */

/* Tasks */
uint16_t prvTraceGetTaskNumberLow16(void* handle);
uint16_t prvTraceGetTaskNumberHigh16(void* handle);
void prvTraceSetTaskNumberLow16(void* handle, uint16_t value);
void prvTraceSetTaskNumberHigh16(void* handle, uint16_t value);

/* Task Queues */
#if (OS_CFG_TASK_Q_EN == DEF_ENABLED)
uint16_t prvTraceGetTaskQueueNumberLow16(void* handle);
uint16_t prvTraceGetTaskQueueNumberHigh16(void* handle);
void prvTraceSetTaskQueueNumberLow16(void* handle, uint16_t value);
void prvTraceSetTaskQueueNumberHigh16(void* handle, uint16_t value);
#endif /* (OS_CFG_TASK_Q_EN == DEF_ENABLED) */

/* Task Semaphores */
uint16_t prvTraceGetTaskSemNumberLow16(void* handle);
uint16_t prvTraceGetTaskSemNumberHigh16(void* handle);
void prvTraceSetTaskSemNumberLow16(void* handle, uint16_t value);
void prvTraceSetTaskSemNumberHigh16(void* handle, uint16_t value);

/* Queues */
uint16_t prvTraceGetQueueNumberLow16(void* handle);
uint16_t prvTraceGetQueueNumberHigh16(void* handle);
void prvTraceSetQueueNumberLow16(void* handle, uint16_t value);
void prvTraceSetQueueNumberHigh16(void* handle, uint16_t value);

/* Semaphores */
uint16_t prvTraceGetSemaphoreNumberLow16(void* handle);
uint16_t prvTraceGetSemaphoreNumberHigh16(void* handle);
void prvTraceSetSemaphoreNumberLow16(void* handle, uint16_t value);
void prvTraceSetSemaphoreNumberHigh16(void* handle, uint16_t value);

/* Mutexes */
uint16_t prvTraceGetMutexNumberLow16(void* handle);
uint16_t prvTraceGetMutexNumberHigh16(void* handle);
void prvTraceSetMutexNumberLow16(void* handle, uint16_t value);
void prvTraceSetMutexNumberHigh16(void* handle, uint16_t value);

/* Flags */
uint16_t prvTraceGetFlagNumberLow16(void* handle);
uint16_t prvTraceGetFlagNumberHigh16(void* handle);
void prvTraceSetFlagNumberLow16(void* handle, uint16_t value);
void prvTraceSetFlagNumberHigh16(void* handle, uint16_t value);

/* Mems */
uint16_t prvTraceGetMemNumberLow16(void* handle);
uint16_t prvTraceGetMemNumberHigh16(void* handle);
void prvTraceSetMemNumberLow16(void* handle, uint16_t value);
void prvTraceSetMemNumberHigh16(void* handle, uint16_t value);

/* Task Suspend and Resume */
void prvTraceTaskSuspendEnter(void* p_tcb);
void prvTraceTaskSuspendExit(uint32_t p_err);
void prvTraceTaskResumeEnter(void* p_tcb);
void prvTraceTaskResumeExit(uint32_t p_err);

/* Filters */
#define TRACE_GET_TASK_FILTER(pxTask) 1
#define TRACE_SET_TASK_FILTER(pxTask, group) 

#if (OS_CFG_TASK_Q_EN == DEF_ENABLED)
#define TRACE_GET_MICRIUM_TASK_MSG_Q_FILTER(pxObject) 1
#define TRACE_SET_MICRIUM_TASK_MSG_Q_FILTER(pxObject, group) 
#endif /* (OS_CFG_TASK_Q_EN == DEF_ENABLED) */

#define TRACE_GET_MICRIUM_TASK_SEM_FILTER(pxObject) 1
#define TRACE_SET_MICRIUM_TASK_SEM_FILTER(pxObject, group) 

#define TRACE_GET_MICRIUM_Q_FILTER(pxObject) 1
#define TRACE_SET_MICRIUM_Q_FILTER(pxObject, group) 

#define TRACE_GET_MICRIUM_SEM_FILTER(pxObject) 1
#define TRACE_SET_MICRIUM_SEM_FILTER(pxObject, group) 

#define TRACE_GET_MICRIUM_MUTEX_FILTER(pxObject) 1
#define TRACE_SET_MICRIUM_MUTEX_FILTER(pxObject, group) 

#define TRACE_GET_MICRIUM_FLAG_FILTER(pxObject) 1
#define TRACE_SET_MICRIUM_FLAG_FILTER(pxObject, group) 

#define TRACE_GET_MICRIUM_MEM_FILTER(pxObject) 1
#define TRACE_SET_MICRIUM_MEM_FILTER(pxObject, group) 

#define TRACE_GET_OBJECT_FILTER(CLASS, pxObject) TRACE_GET_##CLASS##_FILTER(pxObject)
#define TRACE_SET_OBJECT_FILTER(CLASS, pxObject, group) TRACE_SET_##CLASS##_FILTER(pxObject, group)

#undef OS_TRACE_INIT
#define OS_TRACE_INIT()                                      xTraceEnable(TRC_INIT)

#undef OS_TRACE_START
#define OS_TRACE_START()                                     xTraceEnable(TRC_START)

#undef OS_TRACE_STOP
#define OS_TRACE_STOP()                                      xTraceDisable()

/*******************************************************************************
 * The event codes - should match the offline config file.
 ******************************************************************************/

#define PSF_EVENT_NULL_EVENT								0x00

/* PSF event codes */
#define PSF_EVENT_TRACE_START								0x01
#define PSF_EVENT_TS_CONFIG									0x02
#define PSF_EVENT_OBJ_NAME									0x03
#define PSF_EVENT_TASK_PRIORITY								0x04
#define PSF_EVENT_TASK_PRIO_INHERIT							0x05
#define PSF_EVENT_TASK_PRIO_DISINHERIT						0x06
#define PSF_EVENT_DEFINE_ISR								0x07
#define PSF_EVENT_UNUSED_STACK								0x08

#define PSF_EVENT_TASK_CREATE								0x10
#define PSF_EVENT_TASK_SEM_CREATE							0x11
#define PSF_EVENT_TASK_MSG_Q_CREATE							0x12
#define PSF_EVENT_QUEUE_CREATE								0x13
#define PSF_EVENT_SEMAPHORE_CREATE					        0x14
#define PSF_EVENT_MUTEX_CREATE								0x15
#define PSF_EVENT_FLAG_CREATE							    0x16
#define PSF_EVENT_MEM_CREATE							    0x17
#define PSF_EVENT_TMR_CREATE							    0x18

#define PSF_EVENT_TASK_DELETE								0x20
#define PSF_EVENT_QUEUE_DELETE								0x21
#define PSF_EVENT_SEMAPHORE_DELETE							0x22
#define PSF_EVENT_MUTEX_DELETE								0x23
#define PSF_EVENT_FLAG_DELETE					   		    0x24
#define PSF_EVENT_MEM_DELETE								0x25
#define PSF_EVENT_TMR_DELETE								0x26
#define PSF_EVENT_TASK_SEM_DELETE							0x27
#define PSF_EVENT_TASK_MSG_Q_DELETE							0x28

#define PSF_EVENT_TASK_READY								0x30
#define PSF_EVENT_NEW_TIME									0x31
#define PSF_EVENT_NEW_TIME_SCHEDULER_SUSPENDED				0x32
#define PSF_EVENT_ISR_BEGIN									0x33
#define PSF_EVENT_ISR_RESUME								0x34
#define PSF_EVENT_TS_BEGIN									0x35
#define PSF_EVENT_TS_RESUME									0x36
#define PSF_EVENT_TASK_ACTIVATE								0x37

#define PSF_EVENT_MALLOC									0x38
#define PSF_EVENT_FREE										0x39

#define PSF_EVENT_LOWPOWER_BEGIN							0x3A
#define PSF_EVENT_LOWPOWER_END								0x3B

#define PSF_EVENT_IFE_NEXT									0x3C
#define PSF_EVENT_IFE_DIRECT								0x3D

#define PSF_EVENT_MALLOC_FAILED								0x3E
#define PSF_EVENT_FREE_FAILED								0x3F

#define PSF_EVENT_TASK_CREATE_FAILED						0x40
#define PSF_EVENT_QUEUE_CREATE_FAILED						0x41
#define PSF_EVENT_SEMAPHORE_CREATE_FAILED			        0x42
#define PSF_EVENT_MUTEX_CREATE_FAILED						0x43
#define PSF_EVENT_FLAG_CREATE_FAILED						0x44
#define PSF_EVENT_MEM_CREATE_FAILED					        0x45
#define PSF_EVENT_TMR_CREATE_FAILED			                0x46

#define PSF_EVENT_TASK_MSG_Q_SEND		    				0x50
#define PSF_EVENT_TASK_SEM_SEND		    					0x51
#define PSF_EVENT_SEM_SEND				        			0x52
#define PSF_EVENT_Q_SEND		    						0x53
#define PSF_EVENT_MUTEX_SEND								0x54
#define PSF_EVENT_FLAG_SEND						    		0x55
#define PSF_EVENT_MEM_SEND						    		0x56

#define PSF_EVENT_TASK_MSG_Q_SEND_FAILED		    		0x57
#define PSF_EVENT_TASK_SEM_SEND_FAILED		    			0x58
#define PSF_EVENT_SEM_SEND_FAILED				        	0x59
#define PSF_EVENT_Q_SEND_FAILED		    					0x5A
#define PSF_EVENT_MUTEX_SEND_FAILED					    	0x5B
#define PSF_EVENT_FLAG_SEND_FAILED					   		0x5C
#define PSF_EVENT_MEM_SEND_FAILED					   		0x5D

#define PSF_EVENT_TASK_MSG_Q_RECEIVE		   				0x60
#define PSF_EVENT_TASK_SEM_RECEIVE		    				0x61
#define PSF_EVENT_SEM_RECEIVE				       			0x62
#define PSF_EVENT_Q_RECEIVE		    						0x63
#define PSF_EVENT_MUTEX_RECEIVE								0x64
#define PSF_EVENT_FLAG_RECEIVE					    		0x65
#define PSF_EVENT_MEM_RECEIVE					    		0x66

#define PSF_EVENT_TASK_MSG_Q_RECEIVE_FAILED		   			0x70
#define PSF_EVENT_TASK_SEM_RECEIVE_FAILED		    		0x71
#define PSF_EVENT_SEM_RECEIVE_FAILED		      			0x72
#define PSF_EVENT_Q_RECEIVE_FAILED		    				0x73
#define PSF_EVENT_MUTEX_RECEIVE_FAILED						0x74
#define PSF_EVENT_FLAG_RECEIVE_FAILED			    		0x75
#define PSF_EVENT_MEM_RECEIVE_FAILED			    		0x76

#define PSF_EVENT_TASK_MSG_Q_RECEIVE_BLOCK		    		0x80
#define PSF_EVENT_TASK_SEM_RECEIVE_BLOCK		    		0x81
#define PSF_EVENT_SEM_RECEIVE_BLOCK				        	0x82
#define PSF_EVENT_Q_RECEIVE_BLOCK		    				0x83
#define PSF_EVENT_MUTEX_RECEIVE_BLOCK						0x84
#define PSF_EVENT_FLAG_RECEIVE_BLOCK					  	0x85
#define PSF_EVENT_MEM_RECEIVE_BLOCK				    		0x86

#define PSF_EVENT_STATEMACHINE_STATE_CREATE					0x87
#define PSF_EVENT_STATEMACHINE_CREATE						0x88
#define PSF_EVENT_STATEMACHINE_STATECHANGE					0x89

#define PSF_EVENT_INTERVAL_CHANNEL_CREATE					0x8A
#define PSF_EVENT_INTERVAL_CHANNEL_SET_CREATE				0x8B
#define PSF_EVENT_INTERVAL_START							0x8C
#define PSF_EVENT_INTERVAL_STOP								0x8D

#define PSF_EVENT_EXTENSION_CREATE							0x8E

#define PSF_EVENT_HEAP_CREATE								0x8F

#define PSF_EVENT_COUNTER_CREATE							0x90
#define PSF_EVENT_COUNTER_CHANGE							0x91
#define PSF_EVENT_COUNTER_LIMIT_EXCEEDED					0x92

#define PSF_EVENT_TASK_DELAY								0xAA
#define PSF_EVENT_TASK_SUSPEND								0xAB
#define PSF_EVENT_TASK_RESUME								0xAC
#define PSF_EVENT_TASK_SUSPEND_FAILED						0xAD
#define PSF_EVENT_TASK_RESUME_FAILED						0xAE

#define PSF_EVENT_USER_EVENT								0xB0

#define PSF_EVENT_USER_EVENT_FIXED							0xB8

#define PSF_EVENT_RUNNABLE_REGISTER							0xC0
#define PSF_EVENT_RUNNABLE_START							0xC1
#define PSF_EVENT_RUNNABLE_STOP								0xC2

#define PSF_EVENT_DEPENDENCY_REGISTER						0xC3

#define TRC_EVENT_LAST_ID									(PSF_EVENT_DEPENDENCY_REGISTER)

/*** The trace macros for streaming ******************************************/

/* Called in OSTaskCreate */
/* Need to protect critical section without reallocating the SR.                     */
#undef OS_TRACE_TASK_CREATE
#if (OS_VERSION >= 30700u)
#define OS_TRACE_TASK_CREATE(p_tcb) \
	xTraceTaskRegisterWithoutHandle((void*)(p_tcb), (const char*)(p_tcb)->NamePtr, (TraceUnsignedBaseType_t)(p_tcb)->Prio)
#else /* (OS_VERSION >= 30700u) */
#define OS_TRACE_TASK_CREATE(p_tcb) \
	if (p_tcb == OS_TickTask) \
    { \
    	prvTrcTickTask = (void*)p_tcb; /* This is the Tick Task */ \
    } \
	xTraceTaskRegisterWithoutHandle((void*)p_tcb, (const char*)p_tcb->NamePtr, (TraceUnsignedBaseType_t)p_tcb->Prio)
#endif /* (OS_VERSION >= 30700u) */

/* Called in OSTaskCreate, if it fails (typically if the stack can not be allocated) */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_CREATE_FAILED
#define OS_TRACE_TASK_CREATE_FAILED(p_tcb) \
	prvTraceEventCreate2(PSF_EVENT_TASK_CREATE_FAILED, 0, 0)

/* Called on OSTaskDel */
/* No need to protect critical section. */
/* If this macro give you problem try increase optimization. */
#undef OS_TRACE_TASK_DEL
#define OS_TRACE_TASK_DEL(p_tcb) \
	xTraceTaskUnregisterWithoutHandle((void*)(p_tcb), (TraceUnsignedBaseType_t)(p_tcb)->Prio)

/* Called for each task that becomes ready */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_READY
#define OS_TRACE_TASK_READY( p_tcb ) \
	TRACE_IF_NOT_TICK_TASK(p_tcb) \
	{ \
		xTraceTaskReady((void*)(p_tcb)); \
	}

/* Called on each OS tick. Will call uiPortGetTimestamp to make sure it is called at least once every OS tick. */
/* No need to protect critical section. */
#if (TRC_CFG_INCLUDE_OSTICK_EVENTS == 1)
#undef OS_TRACE_TICK_INCREMENT
#define OS_TRACE_TICK_INCREMENT( OSTickCtr ) \
	{ xTraceTimestampSetOsTickCount(OSTickCtr); prvTraceEventCreate1(PSF_EVENT_NEW_TIME, OSTickCtr); }
#else /* (TRC_CFG_INCLUDE_OSTICK_EVENTS == 1) */
#define OS_TRACE_TICK_INCREMENT( OSTickCtr ) 
#endif /* (TRC_CFG_INCLUDE_OSTICK_EVENTS == 1) */

/* Called on each task-switch */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_SWITCHED_IN
#define OS_TRACE_TASK_SWITCHED_IN(p_tcb) \
	TRACE_IF_NOT_TICK_TASK(p_tcb) \
	{ \
		xTraceTaskSwitch((void*)(p_tcb), (TraceUnsignedBaseType_t)(p_tcb)->Prio); \
	}
	
#if (TRC_CFG_SCHEDULING_ONLY == 0)

#if (OS_VERSION < 30700u)
/* Called on OSTaskSuspend */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_SUSPEND
#define OS_TRACE_TASK_SUSPEND( p_tcb ) \
	if ((p_tcb) == 0) \
	{ \
		prvTraceEventCreate1(PSF_EVENT_TASK_SUSPEND, OSTCBCurPtr); \
	} \
	else \
	{ \
		prvTraceEventCreate1(PSF_EVENT_TASK_SUSPEND, p_tcb); \
	}
#else /* (OS_VERSION < 30700u) */
/* Called on OSTaskSuspend */
/* Critical section needed to prevent variable to be overwritten. */
#undef OS_TRACE_TASK_SUSPEND_ENTER
#define OS_TRACE_TASK_SUSPEND_ENTER( p_tcb ) \
	prvTraceTaskSuspendEnter(p_tcb)

#undef OS_TRACE_TASK_SUSPEND_EXIT
#define OS_TRACE_TASK_SUSPEND_EXIT(p_err) \
	prvTraceTaskSuspendExit(p_err)
#endif /* (OS_VERSION < 30700u) */

#if (TRACE_CFG_USE_TICKLESS_IDLE != 0)

/* NOT USED */
/* No need to protect critical section. */
#undef traceLOW_POWER_IDLE_BEGIN
#define traceLOW_POWER_IDLE_BEGIN() \
	prvTraceEventCreate0(PSF_EVENT_LOWPOWER_BEGIN)

/* NOT USED */
/* No need to protect critical section. */
#undef traceLOW_POWER_IDLE_END
#define traceLOW_POWER_IDLE_END() \
	prvTraceEventCreate0(PSF_EVENT_LOWPOWER_END)

#endif

/* Called on OSTimeDly */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_DLY
#define OS_TRACE_TASK_DLY(dly_ticks) \
	prvTraceEventCreate1(PSF_EVENT_TASK_DELAY, dly_ticks)

/* Called on OSTaskDel */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_MSG_Q_DEL
#define OS_TRACE_TASK_MSG_Q_DEL(p_msg_q) \
	xTraceObjectUnregisterWithoutHandle(PSF_EVENT_TASK_MSG_Q_DELETE, (void*)(p_msg_q), (TraceUnsignedBaseType_t)((p_msg_q)->NbrEntries))

/* Called on OSTaskDel */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_SEM_DEL
#define OS_TRACE_TASK_SEM_DEL(p_tcb) \
	xTraceObjectUnregisterWithoutHandle(PSF_EVENT_TASK_SEM_DELETE, (void*)(&(p_tcb)->SemCtr), (TraceUnsignedBaseType_t)((p_tcb)->SemCtr))

/* Called on OSQDel */
/* No need to protect critical section. */
#undef OS_TRACE_Q_DEL
#define OS_TRACE_Q_DEL( p_q ) \
	xTraceObjectUnregisterWithoutHandle(PSF_EVENT_QUEUE_DELETE, (void*)(p_q), 0)

/* Called on OSMutexDel */
/* No need to protect critical section. */
#undef OS_TRACE_MUTEX_DEL
#define OS_TRACE_MUTEX_DEL( p_mutex ) \
	xTraceObjectUnregisterWithoutHandle(PSF_EVENT_MUTEX_DELETE, (void*)(p_mutex), 0)

/* Called on OSSemDel */
/* No need to protect critical section. */
#undef OS_TRACE_SEM_DEL
#define OS_TRACE_SEM_DEL( p_sem ) \
	xTraceObjectUnregisterWithoutHandle(PSF_EVENT_SEMAPHORE_DELETE, (void*)(p_sem), 0)

/* Called on OSFlagDel */
/* No need to protect critical section. */
#undef OS_TRACE_FLAG_DEL
#define OS_TRACE_FLAG_DEL( p_grp ) \
	xTraceObjectUnregisterWithoutHandle(PSF_EVENT_FLAG_DELETE, (void*)(p_grp), 0)

/* Called in OSTaskCreate, while it creates the built-in message queue */
/* Need to protect critical section without reallocating the SR.       */
#undef OS_TRACE_TASK_MSG_Q_CREATE
#define OS_TRACE_TASK_MSG_Q_CREATE(p_msg_q, p_name) \
	xTraceObjectRegisterWithoutHandle(PSF_EVENT_TASK_MSG_Q_CREATE, (void*)(p_msg_q), prvTraceAppend((const char*)(p_name), "Q"), (TraceUnsignedBaseType_t)(((OS_MSG_Q *)(p_msg_q))->NbrEntries))

/* Called in OSTaskCreate, while it creates the built-in semaphore */
/* Need to protect critical section without reallocating the SR.       */
#undef OS_TRACE_TASK_SEM_CREATE
#define OS_TRACE_TASK_SEM_CREATE(p_tcb, p_name) \
	xTraceObjectRegisterWithoutHandle(PSF_EVENT_TASK_SEM_CREATE, (void*)(&(p_tcb)->SemCtr), prvTraceAppend((const char*)(p_name), "Sem"), (TraceUnsignedBaseType_t)((p_tcb)->SemCtr))

/* Called from os_q.c. */
/* No need to protect critical section. */
#undef OS_TRACE_Q_CREATE
#define OS_TRACE_Q_CREATE(p_q, p_name ) \
	xTraceObjectRegisterWithoutHandle(PSF_EVENT_QUEUE_CREATE, (void*)(p_q), (const char*)(p_name), (TraceUnsignedBaseType_t)((p_q)->MsgQ.NbrEntriesSize))

/* Called from OSSemCreate. */
/* No need to protect critical section. */
#undef OS_TRACE_SEM_CREATE
#define OS_TRACE_SEM_CREATE( p_sem, p_name) \
	xTraceObjectRegisterWithoutHandle(PSF_EVENT_SEMAPHORE_CREATE, (void*)(p_sem), (const char*)(p_name), (TraceUnsignedBaseType_t)((p_sem)->Ctr))

/* Called from OSMutexCreate. */
/* No need to protect critical section. */
#undef OS_TRACE_MUTEX_CREATE
#define OS_TRACE_MUTEX_CREATE( p_mutex, p_name ) \
	xTraceObjectRegisterWithoutHandle(PSF_EVENT_MUTEX_CREATE, (void*)(p_mutex), (const char*)(p_name), 0)

/* Called from OSMemCreate. */
/* No need to protect critical section. */
#undef OS_TRACE_MEM_CREATE
#define OS_TRACE_MEM_CREATE( p_mem, p_name ) \
	xTraceObjectRegisterWithoutHandle(PSF_EVENT_MEM_CREATE, (void*)(p_mem), (const char*)(p_name), 0)

/* Called from OSFlagCreate. */
/* No need to protect critical section. */
#undef OS_TRACE_FLAG_CREATE
#define OS_TRACE_FLAG_CREATE( p_grp, p_name ) \
	xTraceObjectRegisterWithoutHandle(PSF_EVENT_FLAG_CREATE, (void*)(p_grp), (const char*)(p_name), 0)

/* Called when a message is sent to a task's queue */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_MSG_Q_POST
#define OS_TRACE_TASK_MSG_Q_POST(p_msg_q) \
	if ((p_msg_q)->NbrEntries >= (p_msg_q)->NbrEntriesSize || OSMsgPool.NbrFree == 0u) \
	{ \
		prvTraceEventCreate2(PSF_EVENT_TASK_MSG_Q_SEND_FAILED, p_msg_q, ((OS_MSG_Q *)(p_msg_q))->NbrEntries); \
	} \
	else \
	{ \
		prvTraceEventCreate2(PSF_EVENT_TASK_MSG_Q_SEND, p_msg_q, ((OS_MSG_Q *)(p_msg_q))->NbrEntries); \
	}

/* Called when a message failed to be sent to a task's queue (timeout) */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_MSG_Q_POST_FAILED
#define OS_TRACE_TASK_MSG_Q_POST_FAILED(p_msg_q) \
	prvTraceEventCreate2(PSF_EVENT_TASK_MSG_Q_SEND_FAILED, p_msg_q, ((OS_MSG_Q *)(p_msg_q))->NbrEntries)

/* Called when a message is sent to a task's semaphore */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_SEM_POST
#define OS_TRACE_TASK_SEM_POST(p_tcb) \
	TRACE_IF_NOT_TICK_TASK(p_tcb) \
	{ \
		OS_TCB* trcPtr; \
		if ((p_tcb) == 0) \
		{ \
			trcPtr = OSTCBCurPtr; \
		} \
		else \
		{ \
			trcPtr = p_tcb; \
		} \
		if (trcPtr->SemCtr == (OS_SEM_CTR)-1) \
		{ \
			prvTraceEventCreate2(PSF_EVENT_TASK_SEM_SEND_FAILED, &trcPtr->SemCtr, trcPtr->SemCtr); \
		} \
		else \
		{ \
			prvTraceEventCreate2(PSF_EVENT_TASK_SEM_SEND, &trcPtr->SemCtr, trcPtr->SemCtr); \
		} \
	}

/* Called when a signal failed to be sent to a task's semaphore (timeout) */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_SEM_POST_FAILED
#define OS_TRACE_TASK_SEM_POST_FAILED(p_tcb) \
	TRACE_IF_NOT_TICK_TASK(p_tcb) \
	{ \
		prvTraceEventCreate2(PSF_EVENT_TASK_SEM_SEND_FAILED, &(p_tcb)->SemCtr, (p_tcb)->SemCtr); \
	}

/* Called when a signal is sent to a queue */
/* No need to protect critical section. */
#undef OS_TRACE_Q_POST
#define OS_TRACE_Q_POST(p_q) \
	if ((p_q)->MsgQ.NbrEntries >= (p_q)->MsgQ.NbrEntriesSize || OSMsgPool.NbrFree == 0u) \
	{ \
		prvTraceEventCreate2(PSF_EVENT_Q_SEND_FAILED, p_q, (p_q)->PendList.NbrEntries); \
	} \
	else \
	{ \
		prvTraceEventCreate2(PSF_EVENT_Q_SEND, p_q, (p_q)->PendList.NbrEntries); \
	}

/* Called when a message failed to be sent to a queue (timeout) */
/* No need to protect critical section. */
#undef OS_TRACE_Q_POST_FAILED
#define OS_TRACE_Q_POST_FAILED(p_q) \
	prvTraceEventCreate2(PSF_EVENT_Q_SEND_FAILED, p_q, (p_q)->PendList.NbrEntries)

/* Called when a signal is sent to a semaphore */
/* No need to protect critical section. */
#undef OS_TRACE_SEM_POST
#define OS_TRACE_SEM_POST(p_sem) \
	if ((p_sem)->Ctr == (OS_SEM_CTR)-1) \
	{ \
		prvTraceEventCreate2(PSF_EVENT_SEM_SEND_FAILED, p_sem, (p_sem)->Ctr); \
	} \
	else \
	{ \
		prvTraceEventCreate2(PSF_EVENT_SEM_SEND, p_sem, (p_sem)->Ctr); \
	}

/* Called when a signal failed to be sent to a semaphore (timeout) */
/* No need to protect critical section. */
#undef OS_TRACE_SEM_POST_FAILED
#define OS_TRACE_SEM_POST_FAILED(p_sem) \
	prvTraceEventCreate2(PSF_EVENT_SEM_SEND_FAILED, p_sem, (p_sem)->Ctr)

/* Called when a signal is sent to a mutex */
/* No need to protect critical section. */
#undef OS_TRACE_MUTEX_POST
#define OS_TRACE_MUTEX_POST(p_mutex) \
	prvTraceEventCreate1(PSF_EVENT_MUTEX_SEND, p_mutex)

/* Called when a message failed to be sent to a mutex (timeout) */
/* No need to protect critical section. */
#undef OS_TRACE_MUTEX_POST_FAILED
#define OS_TRACE_MUTEX_POST_FAILED(p_mutex) \
	prvTraceEventCreate1(PSF_EVENT_MUTEX_SEND_FAILED, p_mutex)

/* Called when a signal is sent to a flag group */
/* No need to protect critical section. */
#undef OS_TRACE_FLAG_POST
#define OS_TRACE_FLAG_POST(p_grp) \
	switch (opt) \
	{ \
		case OS_OPT_POST_FLAG_SET: \
		case OS_OPT_POST_FLAG_SET | OS_OPT_POST_NO_SCHED: \
		case OS_OPT_POST_FLAG_CLR: \
		case OS_OPT_POST_FLAG_CLR | OS_OPT_POST_NO_SCHED: \
			prvTraceEventCreate1(PSF_EVENT_FLAG_SEND, p_grp); \
			break; \
		default: \
			prvTraceEventCreate1(PSF_EVENT_FLAG_SEND_FAILED, p_grp); \
			break; \
	}

/* Called when a message failed to be sent to a flag group (timeout) */
/* No need to protect critical section. */
#undef OS_TRACE_FLAG_POST_FAILED
#define OS_TRACE_FLAG_POST_FAILED(p_grp) \
	prvTraceEventCreate1(PSF_EVENT_FLAG_SEND_FAILED, p_grp)

/* Called when a block is returned to memory */
/* No need to protect critical section. */
#undef OS_TRACE_MEM_PUT
#define OS_TRACE_MEM_PUT(p_mem) \
	prvTraceEventCreate2(PSF_EVENT_MEM_SEND, p_mem, (p_mem)->NbrFree)

/* Called when a block fails to be returned to memory */
/* No need to protect critical section. */
#undef OS_TRACE_MEM_PUT_FAILED
#define OS_TRACE_MEM_PUT_FAILED(p_mem) \
	prvTraceEventCreate2(PSF_EVENT_MEM_SEND_FAILED, p_mem, 0)

/* Called when a block is retrieved from memory */
/* No need to protect critical section. */
#undef OS_TRACE_MEM_GET
#define OS_TRACE_MEM_GET(p_mem) \
	prvTraceEventCreate2(PSF_EVENT_MEM_RECEIVE_FAILED, p_mem, (p_mem)->NbrFree)

/* Called when a block fails to be retrieved from memory */
/* No need to protect critical section. */
#undef OS_TRACE_MEM_GET_FAILED
#define OS_TRACE_MEM_GET_FAILED(p_mem) \
	prvTraceEventCreate2(PSF_EVENT_MEM_RECEIVE_FAILED, p_mem, 0)

/* Called when a message is received from a task message queue */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_MSG_Q_PEND
#define OS_TRACE_TASK_MSG_Q_PEND(p_msg_q) \
	prvTraceEventCreate2(PSF_EVENT_TASK_MSG_Q_RECEIVE, p_msg_q, (p_msg_q)->NbrEntries)

/* Called when a message is received from a task message queue */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_MSG_Q_PEND_FAILED
#define OS_TRACE_TASK_MSG_Q_PEND_FAILED(p_msg_q) \
	prvTraceEventCreate2(PSF_EVENT_TASK_MSG_Q_RECEIVE_FAILED, p_msg_q, (p_msg_q)->NbrEntries)

/* Called when a message is received from a task message queue */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_MSG_Q_PEND_BLOCK
#define OS_TRACE_TASK_MSG_Q_PEND_BLOCK(p_msg_q) \
	prvTraceEventCreate2(PSF_EVENT_TASK_MSG_Q_RECEIVE_BLOCK, p_msg_q, (p_msg_q)->NbrEntries)

/* Called when a message is received from a task semaphore */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_SEM_PEND
#define OS_TRACE_TASK_SEM_PEND(p_tcb) \
	TRACE_IF_NOT_TICK_TASK(p_tcb) \
	{ \
		prvTraceEventCreate2(PSF_EVENT_TASK_SEM_RECEIVE, &(p_tcb)->SemCtr, (p_tcb)->SemCtr); \
	}

/* Called when a message is received from a task semaphore */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_SEM_PEND_FAILED
#define OS_TRACE_TASK_SEM_PEND_FAILED(p_tcb) \
	TRACE_IF_NOT_TICK_TASK(p_tcb) \
	{ \
		prvTraceEventCreate2(PSF_EVENT_TASK_SEM_RECEIVE_FAILED, &(p_tcb)->SemCtr, (p_tcb)->SemCtr); \
	}

/* Called when a message is received from a task semaphore */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_SEM_PEND_BLOCK
#define OS_TRACE_TASK_SEM_PEND_BLOCK(p_tcb) \
	TRACE_IF_NOT_TICK_TASK(p_tcb) \
	{ \
		prvTraceEventCreate2(PSF_EVENT_TASK_SEM_RECEIVE_BLOCK, &(p_tcb)->SemCtr, (p_tcb)->SemCtr); \
	}

/* Called when a message is received from a semaphore */
/* No need to protect critical section. */
#undef OS_TRACE_SEM_PEND
#define OS_TRACE_SEM_PEND(p_sem) \
	prvTraceEventCreate2(PSF_EVENT_SEM_RECEIVE, p_sem, (p_sem)->Ctr)

/* Called when a message is received from a semaphore */
/* No need to protect critical section. */
#undef OS_TRACE_SEM_PEND_FAILED
#define OS_TRACE_SEM_PEND_FAILED(p_sem) \
	prvTraceEventCreate2(PSF_EVENT_SEM_RECEIVE_FAILED, p_sem, (p_sem)->Ctr)

/* Called when a message is received from a semaphore */
/* No need to protect critical section. */
#undef OS_TRACE_SEM_PEND_BLOCK
#define OS_TRACE_SEM_PEND_BLOCK(p_sem) \
	prvTraceEventCreate2(PSF_EVENT_SEM_RECEIVE_BLOCK, p_sem, (p_sem)->Ctr)


/* Called when a message is received from a queue */
/* No need to protect critical section. */
#undef OS_TRACE_Q_PEND
#define OS_TRACE_Q_PEND(p_q) \
	prvTraceEventCreate2(PSF_EVENT_Q_RECEIVE, p_q, (p_q)->PendList.NbrEntries)

/* Called when a message is received from a queue */
/* No need to protect critical section. */
#undef OS_TRACE_Q_PEND_FAILED
#define OS_TRACE_Q_PEND_FAILED(p_q) \
	prvTraceEventCreate2(PSF_EVENT_Q_RECEIVE_FAILED, p_q, (p_q)->PendList.NbrEntries)

/* Called when a message is received from a queue */
/* No need to protect critical section. */
#undef OS_TRACE_Q_PEND_BLOCK
#define OS_TRACE_Q_PEND_BLOCK(p_q) \
	prvTraceEventCreate2(PSF_EVENT_Q_RECEIVE_BLOCK, p_q, (p_q)->PendList.NbrEntries)


/* Called when a signal is received from a mutex */
/* No need to protect critical section. */
#undef OS_TRACE_MUTEX_PEND
#define OS_TRACE_MUTEX_PEND(p_mutex) \
	prvTraceEventCreate1(PSF_EVENT_MUTEX_RECEIVE, p_mutex)

/* Called when a signal is received from a mutex */
/* No need to protect critical section. */
#undef OS_TRACE_MUTEX_PEND_FAILED
#define OS_TRACE_MUTEX_PEND_FAILED(p_mutex) \
	prvTraceEventCreate1(PSF_EVENT_MUTEX_RECEIVE_FAILED, p_mutex)

/* Called when a signal is received from a mutex */
/* No need to protect critical section. */
#undef OS_TRACE_MUTEX_PEND_BLOCK
#define OS_TRACE_MUTEX_PEND_BLOCK(p_mutex) \
	prvTraceEventCreate1(PSF_EVENT_MUTEX_RECEIVE_BLOCK, p_mutex)


/* Called when a signal is received from a flag group */
/* No need to protect critical section. */
#undef OS_TRACE_FLAG_PEND
#define OS_TRACE_FLAG_PEND(p_grp) \
	prvTraceEventCreate1(PSF_EVENT_FLAG_RECEIVE, p_grp)

/* Called when a signal is received from a flag group */
/* No need to protect critical section. */
#undef OS_TRACE_FLAG_PEND_FAILED
#define OS_TRACE_FLAG_PEND_FAILED(p_grp) \
	prvTraceEventCreate1(PSF_EVENT_FLAG_RECEIVE_FAILED, p_grp)

/* Called in OSTaskChangePrio */
#undef OS_TRACE_TASK_PRIO_CHANGE
#define OS_TRACE_TASK_PRIO_CHANGE( p_tcb, prio ) \
	prvTraceEventCreate2(PSF_EVENT_TASK_PRIORITY, p_tcb, prio)
	
/* Called in by mutex operations, when the task's priority is changed to the one from the mutex's owner */
/* No need to protect critical section. */
#undef OS_TRACE_MUTEX_TASK_PRIO_INHERIT
#define OS_TRACE_MUTEX_TASK_PRIO_INHERIT( p_tcb, prio ) \
	prvTraceEventCreate2(PSF_EVENT_TASK_PRIO_INHERIT, p_tcb, prio)

/* Called in by mutex operations, when the task's priority is changed back to its original */
/* No need to protect critical section. */
#undef OS_TRACE_MUTEX_TASK_PRIO_DISINHERIT
#define OS_TRACE_MUTEX_TASK_PRIO_DISINHERIT( p_tcb, prio ) \
	prvTraceEventCreate2(PSF_EVENT_TASK_PRIO_DISINHERIT, p_tcb, prio)

#if (OS_VERSION < 30700u)

/* Called in OSTaskResume */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_RESUME
#define OS_TRACE_TASK_RESUME( p_tcb ) \
	prvTraceEventCreate1(PSF_EVENT_TASK_RESUME, p_tcb)

#else /* (OS_VERSION < 30700u) */

/* Called in OSTaskResume */
/* Critical section needed to prevent variable to be overwritten. */
#undef OS_TRACE_TASK_RESUME_ENTER
#define OS_TRACE_TASK_RESUME_ENTER( p_tcb )\
	prvTraceTaskResumeEnter(p_tcb)

#undef OS_TRACE_TASK_RESUME_EXIT
#define OS_TRACE_TASK_RESUME_EXIT( p_err )\
	prvTraceTaskResumeExit(p_err)

#endif /* (OS_VERSION < 30700u) */

#else /* (TRC_CFG_SCHEDULING_ONLY == 0) */
	
/* SCHEDULING ONLY, DEFINE ALL AS EMPTY */
	
#undef OS_TRACE_TASK_SUSPEND
#define OS_TRACE_TASK_SUSPEND( p_tcb ) 

#undef OS_TRACE_TASK_SUSPEND_ENTER
#define OS_TRACE_TASK_SUSPEND_ENTER( p_tcb ) 

#undef OS_TRACE_TASK_SUSPEND_EXIT
#define OS_TRACE_TASK_SUSPEND_EXIT(p_err) 

#undef traceLOW_POWER_IDLE_BEGIN
#define traceLOW_POWER_IDLE_BEGIN() 

#undef traceLOW_POWER_IDLE_END
#define traceLOW_POWER_IDLE_END() 

#undef OS_TRACE_TASK_DLY
#define OS_TRACE_TASK_DLY(dly_ticks) 

#undef OS_TRACE_TASK_MSG_Q_DEL
#define OS_TRACE_TASK_MSG_Q_DEL(p_msg_q) 

#undef OS_TRACE_TASK_SEM_DEL
#define OS_TRACE_TASK_SEM_DEL(p_tcb) 

#undef OS_TRACE_Q_DEL
#define OS_TRACE_Q_DEL( p_q ) 

#undef OS_TRACE_MUTEX_DEL
#define OS_TRACE_MUTEX_DEL( p_mutex ) 

#undef OS_TRACE_SEM_DEL
#define OS_TRACE_SEM_DEL( p_sem ) 

#undef OS_TRACE_FLAG_DEL
#define OS_TRACE_FLAG_DEL( p_grp ) 

#undef OS_TRACE_TASK_MSG_Q_CREATE
#define OS_TRACE_TASK_MSG_Q_CREATE(p_msg_q, p_name) 

#undef OS_TRACE_TASK_SEM_CREATE
#define OS_TRACE_TASK_SEM_CREATE(p_tcb, p_name) 

#undef OS_TRACE_Q_CREATE
#define OS_TRACE_Q_CREATE(p_q, p_name ) 

#undef OS_TRACE_SEM_CREATE
#define OS_TRACE_SEM_CREATE( p_sem, p_name) 

#undef OS_TRACE_MUTEX_CREATE
#define OS_TRACE_MUTEX_CREATE( p_mutex, p_name ) 

#undef OS_TRACE_MEM_CREATE
#define OS_TRACE_MEM_CREATE( p_mem, p_name ) 

#undef OS_TRACE_FLAG_CREATE
#define OS_TRACE_FLAG_CREATE( p_grp, p_name ) 

#undef OS_TRACE_TASK_MSG_Q_POST
#define OS_TRACE_TASK_MSG_Q_POST(p_msg_q) 

#undef OS_TRACE_TASK_MSG_Q_POST_FAILED
#define OS_TRACE_TASK_MSG_Q_POST_FAILED(p_msg_q) 

#undef OS_TRACE_TASK_SEM_POST
#define OS_TRACE_TASK_SEM_POST(p_tcb) 

#undef OS_TRACE_TASK_SEM_POST_FAILED
#define OS_TRACE_TASK_SEM_POST_FAILED(p_tcb) 

#undef OS_TRACE_Q_POST
#define OS_TRACE_Q_POST(p_q) 

#undef OS_TRACE_Q_POST_FAILED
#define OS_TRACE_Q_POST_FAILED(p_q) 

#undef OS_TRACE_SEM_POST
#define OS_TRACE_SEM_POST(p_sem) 

#undef OS_TRACE_SEM_POST_FAILED
#define OS_TRACE_SEM_POST_FAILED(p_tcb) 

#undef OS_TRACE_MUTEX_POST
#define OS_TRACE_MUTEX_POST(p_mutex) 

#undef OS_TRACE_MUTEX_POST_FAILED
#define OS_TRACE_MUTEX_POST_FAILED(p_mutex) 

#undef OS_TRACE_FLAG_POST
#define OS_TRACE_FLAG_POST(p_grp) 

#undef OS_TRACE_FLAG_POST_FAILED
#define OS_TRACE_FLAG_POST_FAILED(p_grp) 

#undef OS_TRACE_MEM_PUT
#define OS_TRACE_MEM_PUT(p_mem) 

#undef OS_TRACE_MEM_PUT_FAILED
#define OS_TRACE_MEM_PUT_FAILED(p_mem) 

#undef OS_TRACE_TASK_MSG_Q_PEND
#define OS_TRACE_TASK_MSG_Q_PEND(p_msg_q) 

#undef OS_TRACE_TASK_MSG_Q_PEND_FAILED
#define OS_TRACE_TASK_MSG_Q_PEND_FAILED(p_msg_q) 

#undef OS_TRACE_TASK_MSG_Q_PEND_BLOCK
#define OS_TRACE_TASK_MSG_Q_PEND_BLOCK(p_msg_q) 

#undef OS_TRACE_TASK_SEM_PEND
#define OS_TRACE_TASK_SEM_PEND(p_tcb) 

#undef OS_TRACE_TASK_SEM_PEND_FAILED
#define OS_TRACE_TASK_SEM_PEND_FAILED(p_tcb) 

#undef OS_TRACE_TASK_SEM_PEND_BLOCK
#define OS_TRACE_TASK_SEM_PEND_BLOCK(p_tcb) 

#undef OS_TRACE_SEM_PEND
#define OS_TRACE_SEM_PEND(p_sem) 

#undef OS_TRACE_SEM_PEND_FAILED
#define OS_TRACE_SEM_PEND_FAILED(p_sem) 

#undef OS_TRACE_SEM_PEND_BLOCK
#define OS_TRACE_SEM_PEND_BLOCK(p_sem) 

#undef OS_TRACE_Q_PEND
#define OS_TRACE_Q_PEND(p_q) 

#undef OS_TRACE_Q_PEND_FAILED
#define OS_TRACE_Q_PEND_FAILED(p_q) 

#undef OS_TRACE_Q_PEND_BLOCK
#define OS_TRACE_Q_PEND_BLOCK(p_q) 

#undef OS_TRACE_MUTEX_PEND
#define OS_TRACE_MUTEX_PEND(p_mutex) 

#undef OS_TRACE_MUTEX_PEND_FAILED
#define OS_TRACE_MUTEX_PEND_FAILED(p_mutex) 

#undef OS_TRACE_MUTEX_PEND_BLOCK
#define OS_TRACE_MUTEX_PEND_BLOCK(p_mutex) 

#undef OS_TRACE_FLAG_PEND
#define OS_TRACE_FLAG_PEND(p_grp) 

#undef OS_TRACE_FLAG_PEND_FAILED
#define OS_TRACE_FLAG_PEND_FAILED(p_grp) 

#undef OS_TRACE_FLAG_PEND_BLOCK
#define OS_TRACE_FLAG_PEND_BLOCK(p_mem) 

#undef OS_TRACE_TASK_PRIO_CHANGE
#define OS_TRACE_TASK_PRIO_CHANGE( p_tcb, prio ) 

#undef OS_TRACE_MUTEX_TASK_PRIO_INHERIT
#define OS_TRACE_MUTEX_TASK_PRIO_INHERIT( p_tcb, prio ) 

#undef OS_TRACE_MUTEX_TASK_PRIO_DISINHERIT
#define OS_TRACE_MUTEX_TASK_PRIO_DISINHERIT( p_tcb, prio ) 

#undef OS_TRACE_TASK_RESUME
#define OS_TRACE_TASK_RESUME( p_tcb ) 

#undef OS_TRACE_TASK_RESUME_ENTER
#define OS_TRACE_TASK_RESUME_ENTER( p_tcb )

#undef OS_TRACE_TASK_RESUME_EXIT
#define OS_TRACE_TASK_RESUME_EXIT( p_err )

#endif /* (TRC_CFG_SCHEDULING_ONLY == 0) */

#undef OS_TRACE_ISR_REGISTER
#define OS_TRACE_ISR_REGISTER(isr_name, isr_prio) \
	vTraceSetISRProperties(isr_name, isr_prio);

#undef OS_TRACE_ISR_BEGIN
#define OS_TRACE_ISR_BEGIN(isr_name) \
	vTraceStoreISRBegin(isr_name);

#undef OS_TRACE_ISR_END
#define OS_TRACE_ISR_END(flag) \
	vTraceStoreISREnd(flag);

#define  OS_TRACE_ISR_ENTER()
#define  OS_TRACE_ISR_EXIT()
#define  OS_TRACE_ISR_EXIT_TO_SCHEDULER()

#else /*(TRC_USE_TRACEALYZER_RECORDER == 1)*/

#define vTraceExcludeQueue(handle)
#define vTraceExcludeSemaphore(handle)
#define vTraceExcludeMutex(handle)
#define vTraceExcludeTimer(handle)
#define vTraceExcludeEventGroup(handle)
#define vTraceExcludeDelays()

#endif /*(TRC_USE_TRACEALYZER_RECORDER == 1)*/

#ifdef __cplusplus
}
#endif

#endif /* TRC_KERNEL_PORT_H */
