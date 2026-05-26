/*
 * Percepio TraceRecorder for Tracealyzer v989.878.767
 * Copyright 2025 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * FreeRTOS specific definitions needed by the trace recorder
 */

#ifndef TRC_KERNEL_PORT_H
#define TRC_KERNEL_PORT_H

#include <freertos/FreeRTOS.h>
#include <trcDefines.h>
#include <esp_idf_version.h>
#include <trcFreeRTOS.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_PERCEPIO_TRACERECORDER_ENABLED
#define TRC_USE_TRACEALYZER_RECORDER 1
#else
#define TRC_USE_TRACEALYZER_RECORDER 0
#endif

/*** FreeRTOS version codes **************************************************/
#define TRC_ESP_IDF_VERSION_NOT_SET				0
#define TRC_ESP_IDF_VERSION_6_0_0				600

#if ESP_IDF_VERSION_MAJOR == 6 && ESP_IDF_VERSION_MINOR >= 0
/* Attempt at supporting future 6.X versions */
#define TRC_CFG_ESP_IDF_VERSION TRC_ESP_IDF_VERSION_6_0_0
#else
#define TRC_CFG_ESP_IDF_VERSION TRC_ESP_IDF_VERSION_NOT_SET
#endif

#define prvGetStreamBufferType(x) ((( StreamBuffer_t * )x )->ucFlags & sbFLAGS_IS_MESSAGE_BUFFER)

#define STRING_CAST(x) x
#define TraceKernelPortTickType_t TickType_t
#define TraceKernelPortTaskHandle_t TaskHandle_t

#if !CONFIG_FREERTOS_UNICORE
#undef TRC_CFG_GET_CURRENT_CORE
#define TRC_CFG_GET_CURRENT_CORE() (xPortGetCoreID())
#endif

#if !CONFIG_FREERTOS_UNICORE
#define TRC_KERNEL_PORT_ALLOC_CRITICAL_SECTION() 
void TRC_KERNEL_PORT_ENTER_CRITICAL_SECTION(void);
void TRC_KERNEL_PORT_EXIT_CRITICAL_SECTION(void);
#else
/* These are never used since trcHardwarePort.h will define the critical sections if CONFIG_FREERTOS_UNICORE */
#define TRC_KERNEL_PORT_ALLOC_CRITICAL_SECTION() 
#define TRC_KERNEL_PORT_ENTER_CRITICAL_SECTION() 
#define TRC_KERNEL_PORT_EXIT_CRITICAL_SECTION() 
#endif

#if (defined(TRC_USE_TRACEALYZER_RECORDER)) && (TRC_USE_TRACEALYZER_RECORDER == 1)

/**
 * @def TRC_PLATFORM_CFG
 * @brief DO NOT CHANGE THIS
 * This defines the basis for version specific lookup of
 * platform configuration files.
 */
#define TRC_PLATFORM_CFG "FreeRTOS"

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
#define TRC_PLATFORM_CFG_MINOR 2

/**
 * @def TRC_PLATFORM_CFG_PATCH
 * @brief DO NOT CHANGE THIS
 * Patchlevel release version for platform definition file.
 * Does NOT need to match the RTOS version.
 */
#define TRC_PLATFORM_CFG_PATCH 0

/**
 * @brief Global Tracerecorder ESP-IDF entry function
 *
 * NOTE: Although ESP-IDF global constructor functions are called
 * too late for vTraceEnable it is a suitable entry point for
 * initializing the control task.
 *
 * NOTE: ESP-IDF requires us to start the control task separately
 * from the traditional vTraceEnable which is called at an early
 * stage of the boot process. If we start the control task at that
 * early stage it will become primary task during the boot procedure
 * and cause all kinds of problems and kernel panics as the remainder
 * of the boot process is performed.
 */
__attribute__((constructor)) void vTraceInit();

#if defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1) && (TRC_CFG_SCHEDULING_ONLY == 0)

/**
 * @brief Specifies if the kernel allow tasks to be deleted. If the recorder knows that
 * tasks/threads cannot be deleted it can perform certain optimizations regarding stack
 * monitoring.
 */
#define TRC_CFG_ALLOW_TASK_DELETE INCLUDE_vTaskDelete

/* Required for stack monitoring */
#undef INCLUDE_uxTaskGetStackHighWaterMark
#define INCLUDE_uxTaskGetStackHighWaterMark 1

#endif

/* INCLUDE_xTaskGetCurrentTaskHandle must be set to 1 for tracing to work properly */
#undef INCLUDE_xTaskGetCurrentTaskHandle
#define INCLUDE_xTaskGetCurrentTaskHandle 1

#define TRC_KERNEL_PORT_KERNEL_CAN_SWITCH_TO_SAME_TASK 0

#include <trcHeap.h>

#define TRC_KERNEL_PORT_BUFFER_SIZE (sizeof(TraceHeapHandle_t) + sizeof(void*))

/**
 * @internal The kernel port data buffer
 */
typedef struct TraceKernelPortDataBuffer
{
	uint8_t buffer[TRC_KERNEL_PORT_BUFFER_SIZE];
} TraceKernelPortDataBuffer_t;

/**
 * @internal Initializes the kernel port
 * 
 * @param[in] pxBuffer Kernel port data buffer
 *
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceKernelPortInitialize(TraceKernelPortDataBuffer_t* pxBuffer);

/**
 * @internal Enables the kernel port
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceKernelPortEnable(void);

/**
 * @internal Calls on FreeRTOS vTaskDelay(...)
 *
 * @param[in] uiTicks Tick count to delay
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceKernelPortDelay(uint32_t uiTicks);

/**
 * @internal Query if FreeRTOS scheduler is suspended
 *
 * @retval 1 Scheduler suspended
 * @retval 0 Scheduler not suspended
 */
unsigned char xTraceKernelPortIsSchedulerSuspended(void);

/**
* @brief Kernel specific way to set interrupt mask
*/
#define TRC_KERNEL_PORT_SET_INTERRUPT_MASK() ((TraceBaseType_t)portSET_INTERRUPT_MASK_FROM_ISR())

#if (TRC_CFG_SCHEDULING_ONLY == 0)

/**
 * @brief Set the queue name
 *
 * @param[in] pvQueue Queue pointer
 * @param[in] szName Queue name
 */
void vTraceSetQueueName(void* pvQueue, const char* szName);

/**
 * @brief Set the semaphore name
 *
 * @param[in] pvSemaphore Semaphore pointer
 * @param[in] szName Semaphore name
 */
void vTraceSetSemaphoreName(void* pvSemaphore, const char* szName);

/**
 * @brief Set the mutex name
 *
 * @param[in] pvMutex Mutex pointer
 * @param[in] szName Mutex name
 */
void vTraceSetMutexName(void* pvMutex, const char* szName);

#if (TRC_CFG_INCLUDE_EVENT_GROUP_EVENTS == 1)

/**
 * @brief Set the event group name
 *
 * @param[in] pvEventGroup Event group pointer
 * @param[in] szName Event group name
 */
void vTraceSetEventGroupName(void* pvEventGroup, const char* szName);

#else

/**
 * @brief Disabled by TRC_CFG_INCLUDE_EVENT_GROUP_EVENTS
 */
#define vTraceSetEventGroupName(__pvEventGroup, __szName) ((void)(__pvEventGroup), (void)(__szName))

#endif

#if (TRC_CFG_INCLUDE_STREAM_BUFFER_EVENTS == 1)

/**
 * @brief Set the stream buffer name
 *
 * @param[in] pvStreamBuffer Stream buffer pointer
 * @param[in] szName Stream buffer name
 */
void vTraceSetStreamBufferName(void* pvStreamBuffer, const char* szName);

/**
 * @brief Set the message buffer name
 *
 * @param[in] pvMessageBuffer Message buffer pointer
 * @param[in] szName Message buffer name
 */
void vTraceSetMessageBufferName(void* pvMessageBuffer, const char* szName);

#else

/**
 * @brief Disabled by TRC_CFG_INCLUDE_STREAM_BUFFER_EVENTS
 */
#define vTraceSetStreamBufferName(__pvStreamBuffer, __szName) ((void)(__pvStreamBuffer), (void)(__szName))

/**
 * @brief Disabled by TRC_CFG_INCLUDE_STREAM_BUFFER_EVENTS
 */
#define vTraceSetMessageBufferName(__pvMessageBuffer, __szName) ((void)(__pvMessageBuffer), (void)(__szName))

#endif

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

#else

/**
 * @brief Disabled by TRC_CFG_SCHEDULING_ONLY
 */
#define vTraceSetQueueName(__pvQueue, __szName) ((void)(__pvQueue), (void)(__szName))

/**
 * @brief Disabled by TRC_CFG_SCHEDULING_ONLY
 */
#define vTraceSetSemaphoreName(__pvSemaphore, __szName) ((void)(__pvSemaphore), (void)(__szName))

/**
 * @brief Disabled by TRC_CFG_SCHEDULING_ONLY
 */
#define vTraceSetMutexName(__pvMutex, __szName) ((void)(__pvMutex), (void)(__szName))

/**
 * @brief Disabled by TRC_CFG_SCHEDULING_ONLY
 */
#define vTraceSetEventGroupName(__pvEventGroup, __szName) ((void)(__pvEventGroup), (void)(__szName))

/**
 * @brief Disabled by TRC_CFG_SCHEDULING_ONLY
 */
#define vTraceSetStreamBufferName(__pvStreamBuffer, __szName) ((void)(__pvStreamBuffer), (void)(__szName))

/**
 * @brief Disabled by TRC_CFG_SCHEDULING_ONLY
 */
#define vTraceSetMessageBufferName(__pvMessageBuffer, __szName) ((void)(__pvMessageBuffer), (void)(__szName))

/**
 * @brief Disabled by TRC_CFG_SCHEDULING_ONLY
 */
#define xTraceKernelPortGetUnusedStack(pvTask, puxUnusedStack) ((void)(pvTask), (void)(puxUnusedStack))

#endif

/* Required for ISR tracing and Streaming */
#undef INCLUDE_xTaskGetSchedulerState
#define INCLUDE_xTaskGetSchedulerState 1

/**
 * @internal Legacy ID used by Tracealyzer to identify FreeRTOS traces
 */
#define TRACE_KERNEL_VERSION 0x1AA1

/**
 * @internal Kernel specific tick rate frequency definition
 */
#define TRC_TICK_RATE_HZ configTICK_RATE_HZ /* Defined in "FreeRTOS.h" */

/**
 * @internal Kernel specific CPU clock frequency definition
 */
#define TRACE_CPU_CLOCK_HZ configCPU_CLOCK_HZ /* Defined in "FreeRTOSConfig.h" */

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

#if defined(configUSE_TIMERS)
#if (configUSE_TIMERS == 1)
#undef INCLUDE_xTimerGetTimerDaemonTaskHandle
#define INCLUDE_xTimerGetTimerDaemonTaskHandle 1
#endif /* configUSE_TIMERS == 1*/
#endif /* configUSE_TIMERS */

TraceHeapHandle_t xTraceKernelPortGetSystemHeapHandle(void);

/*** The event codes are defined in trcKernelPortHooks.h ***/


/*** The trace macros are implemented in trcKernelPortHooks.h ***/

#else
	
/* When recorder is disabled */
#define vTraceSetQueueName(object, name)
#define vTraceSetSemaphoreName(object, name)
#define vTraceSetMutexName(object, name)
#define vTraceSetEventGroupName(object, name)
#define vTraceSetStreamBufferName(object, name)
#define vTraceSetMessageBufferName(object, name)
	
#endif

#ifdef __cplusplus
}
#endif

#endif
