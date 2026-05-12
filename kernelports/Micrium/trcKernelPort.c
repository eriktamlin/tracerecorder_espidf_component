/*
 * Percepio TraceRecorder for Tracealyzer v989.878.767
 * Copyright 2025 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * uC/OS3 specific parts of the trace recorder
 */

#include <os.h>
#include <lib_str.h>
#include <trcRecorder.h>

#if (defined(TRC_USE_TRACEALYZER_RECORDER) && TRC_USE_TRACEALYZER_RECORDER == 1)

#if (OS_VERSION < 30700u)
/* This is used to keep track of the tick task so we are able to filter it */
void* prvTrcTickTask = 0;
#endif /* (OS_VERSION < 30700u) */

#include <stdint.h>

/* TzCtrl task Stack and TCB */
static  CPU_STK  TzCtrlStk[TRC_CFG_CTRL_TASK_STACK_SIZE];
static  OS_TCB   TzCtrlTCB;
static  OS_TCB*  ptrTzCtrlTCB = 0;

/* The TzCtrl task - receives commands from Tracealyzer (start/stop) */
static  void     TzCtrl (void  *p_arg);

#define TRC_MAX_STRING_APPEND_SIZE 64

/* Variables to hold the latest suspended and resumed task */
static void* lastResumedTask = 0;
static void* lastSuspendedTask = 0;

void* prvTraceGetCurrentTaskHandle(void)
{
	return (void*)OSTCBCurPtr;                                         /* OSTCBCurPtr is a pointer to currently running TCB    */
}

/*******************************************************************************
 * prvTraceIsSchedulerSuspended
 *
 * Returns true if the RTOS scheduler currently is disabled, thus preventing any
 * task-switches from occurring. Only called from vTraceStoreISREnd.
 ******************************************************************************/
unsigned char xTraceKernelPortIsSchedulerSuspended(void)
{
	return OSRunning != OS_STATE_OS_STOPPED;
}

char* prvTraceAppend(const char *name, const char *suffix)
{
	static char buf[TRC_MAX_STRING_APPEND_SIZE];
	uint32_t i = 0, j = 0;
	
	/* (i < ((TRC_MAX_STRING_APPEND_SIZE) - 2)) leaves room for ' ' and null termination */
	while ((i < ((TRC_MAX_STRING_APPEND_SIZE) - 2)) && (name[i] != 0))
	{
		buf[i] = name[i];
		i++;
	}
	
	buf[i] = ' ';
	i++;
	
	/* (i < ((TRC_MAX_STRING_APPEND_SIZE) - 1)) leaves room for null termination */
	while ((i < ((TRC_MAX_STRING_APPEND_SIZE) - 1)) && (suffix[j] != 0))
	{
		buf[i] = suffix[j];
		i++;
		j++;
	}
	
	/* Always null termination */
	buf[i] = 0;

	return (&buf[0]);
}

/* Suspend and resume */
void prvTraceTaskSuspendEnter(void* p_tcb)
{
	TRACE_ALLOC_CRITICAL_SECTION();
	TRACE_ENTER_CRITICAL_SECTION();
	lastSuspendedTask = p_tcb;
}

void prvTraceTaskSuspendExit(uint32_t p_err)
{
	if(p_err == OS_ERR_NONE){
		if (lastSuspendedTask == 0)
		{
			prvTraceEventCreate1(PSF_EVENT_TASK_SUSPEND, OSTCBCurPtr);
		}
		else
		{
			prvTraceEventCreate1(PSF_EVENT_TASK_SUSPEND, lastSuspendedTask);
		}
	}
	else{
		if (lastSuspendedTask == 0)
		{
			prvTraceEventCreate1(PSF_EVENT_TASK_SUSPEND_FAILED, OSTCBCurPtr);
		}
		else
		{
			prvTraceEventCreate1(PSF_EVENT_TASK_SUSPEND_FAILED, lastSuspendedTask);
		}
	}
	TRACE_ALLOC_CRITICAL_SECTION();
	TRACE_EXIT_CRITICAL_SECTION();
}
void prvTraceTaskResumeEnter(void* p_tcb)
{
	TRACE_ALLOC_CRITICAL_SECTION();
	TRACE_ENTER_CRITICAL_SECTION();
	lastResumedTask = p_tcb;
}

void prvTraceTaskResumeExit(uint32_t p_err)
{
	if(p_err == OS_ERR_NONE){
		prvTraceEventCreate1(PSF_EVENT_TASK_RESUME, lastResumedTask);
	}
	else{
		prvTraceEventCreate1(PSF_EVENT_TASK_RESUME_FAILED, lastResumedTask);
	}
	TRACE_ALLOC_CRITICAL_SECTION();
	TRACE_EXIT_CRITICAL_SECTION();
}


#if defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1) && (TRC_CFG_SCHEDULING_ONLY == 0)
traceResult xTraceKernelPortGetUnusedStack(void* task, TraceUnsignedBaseType_t* puxUnusedStack)
{
	CPU_STK_SIZE xFree = 0, xUsed = 0;
	OS_ERR err = OS_ERR_NONE;
	
	OSTaskStkChk(task, &xFree, &xUsed, &err);

	if (err != OS_ERR_NONE)
	{
		return TRC_FAIL;
	}
	*puxUnusedStack = (TraceUnsignedBaseType_t)xFree;
	
	return TRC_SUCCESS;
}
#endif /* defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1) && (TRC_CFG_SCHEDULING_ONLY == 0) */

traceResult xTraceKernelPortInitialize(TraceKernelPortDataBuffer_t* pxBuffer)
{
	(void)pxBuffer;
	return TRC_SUCCESS;
}

traceResult xTraceKernelPortEnable(void)
{
    OS_ERR err;
	
	if (ptrTzCtrlTCB == 0)
	{
        /* Creates the TzCtrl task - receives trace commands (start, stop, ...) */
		OSTaskCreate((OS_TCB     *)&TzCtrlTCB,             /* Create the control task                              */
					(CPU_CHAR   *)"Tracealyzer Control Task",
					(OS_TASK_PTR ) TzCtrl,
					(void       *) 0,
					(OS_PRIO     ) (TRC_CFG_CTRL_TASK_PRIORITY),
					(CPU_STK    *)&TzCtrlStk[0],
					(CPU_STK_SIZE) (TRC_CFG_CTRL_TASK_STACK_SIZE) / 10u,
					(CPU_STK_SIZE) (TRC_CFG_CTRL_TASK_STACK_SIZE),
					(OS_MSG_QTY  ) 0u,
					(OS_TICK     ) 0u,
					(void       *) 0,
					(OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
					(OS_ERR     *)&err);
	
		ptrTzCtrlTCB = &TzCtrlTCB;
	}

	return TRC_SUCCESS;
}

/*******************************************************************************
 * TzCtrl
 *
 * Task for sending the trace data from the internal buffer to the stream 
 * interface (assuming TRC_CFG_STREAM_PORT_USE_INTERNAL_BUFFER == 1) and for
 * receiving commands from Tracealyzer. Also does some diagnostics.
 ******************************************************************************/
static void TzCtrl(void  *p_arg)
{
    OS_ERR  err;
	(void)p_arg;

	while (1)
	{
		xTraceTzCtrl();

		OSTimeDly(TRC_CFG_CTRL_TASK_DELAY, OS_OPT_TIME_DLY, &err);	/* 10ms */
	}
}


#endif /*(TRC_USE_TRACEALYZER_RECORDER == 1)*/
