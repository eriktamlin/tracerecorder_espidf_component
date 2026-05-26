#include <trcFreeRTOS.h>
#include <trcRecorder.h>

void prvTraceEventCreate0_actual(uint32_t eventCode)
{
    (void)prvTraceEventCreate0(eventCode);
}

void prvTraceEventCreate1_actual(uint32_t eventCode, TraceKernelPortParam_t param1)
{
    (void)prvTraceEventCreate1(eventCode, (TraceUnsignedBaseType_t)param1);
}

void prvTraceEventCreate2_actual(uint32_t eventCode, TraceKernelPortParam_t param1, TraceKernelPortParam_t param2)
{
    (void)prvTraceEventCreate2(eventCode, (TraceUnsignedBaseType_t)param1, (TraceUnsignedBaseType_t)param2);
}

void prvTraceEventCreate3_actual(uint32_t eventCode, TraceKernelPortParam_t param1, TraceKernelPortParam_t param2, TraceKernelPortParam_t param3)
{
    (void)prvTraceEventCreate3(eventCode, (TraceUnsignedBaseType_t)param1, (TraceUnsignedBaseType_t)param2, (TraceUnsignedBaseType_t)param3);
}

void prvTraceEventCreate4_actual(uint32_t eventCode, TraceKernelPortParam_t param1, TraceKernelPortParam_t param2, TraceKernelPortParam_t param3, TraceKernelPortParam_t param4)
{
    (void)prvTraceEventCreate4(eventCode, (TraceUnsignedBaseType_t)param1, (TraceUnsignedBaseType_t)param2, (TraceUnsignedBaseType_t)param3, (TraceUnsignedBaseType_t)param4);
}

void prvTraceEventCreate5_actual(uint32_t eventCode, TraceKernelPortParam_t param1, TraceKernelPortParam_t param2, TraceKernelPortParam_t param3, TraceKernelPortParam_t param4, TraceKernelPortParam_t param5)
{
    (void)prvTraceEventCreate5(eventCode, (TraceUnsignedBaseType_t)param1, (TraceUnsignedBaseType_t)param2, (TraceUnsignedBaseType_t)param3, (TraceUnsignedBaseType_t)param4, (TraceUnsignedBaseType_t)param5);
}

void prvTraceEventCreate6_actual(uint32_t eventCode, TraceKernelPortParam_t param1, TraceKernelPortParam_t param2, TraceKernelPortParam_t param3, TraceKernelPortParam_t param4, TraceKernelPortParam_t param5, TraceKernelPortParam_t param6)
{
    (void)prvTraceEventCreate6(eventCode, (TraceUnsignedBaseType_t)param1, (TraceUnsignedBaseType_t)param2, (TraceUnsignedBaseType_t)param3, (TraceUnsignedBaseType_t)param4, (TraceUnsignedBaseType_t)param5, (TraceUnsignedBaseType_t)param6);
}

void prvTraceIncreaseTickCount(uint32_t xCount)
{
    uint32_t uiTraceTickCount;
    (void)xTraceTimestampGetOsTickCount(&uiTraceTickCount);
    (void)xTraceTimestampSetOsTickCount(uiTraceTickCount + xCount);
}

void prvTraceSetTickCount(uint32_t xTickCount)
{
    (void)xTraceTimestampSetOsTickCount(xTickCount);
}

void prvTraceTaskRegister(void* pxTCB, const char* pcName, uint32_t uxPriority)
{
    (void)xTraceTaskRegisterWithoutHandle(pxTCB, pcName, uxPriority);
}

void prvTraceTaskUnregister(void* pxTCB, uint32_t uxPriority)
{
    (void)xTraceTaskUnregisterWithoutHandle(pxTCB, uxPriority);
}

void prvTraceTaskSwitch(void* pxTCB, uint32_t uxPriority)
{
    (void)xTraceTaskSwitch(pxTCB, uxPriority);
}

void prvTraceTaskReady(void* pxTCB)
{
    (void)xTraceTaskReady(pxTCB);
}

void prvTraceTaskSetPriority(void* pxTCB, uint32_t uxNewPriority)
{
    (void)xTraceTaskSetPriorityWithoutHandle(pxTCB, uxNewPriority);
}

void prvTraceObjectRegister(uint32_t eventCode, void* pvObject, const char* pcName, TraceKernelPortParam_t uxParameter)
{
    (void)xTraceObjectRegisterWithoutHandle(eventCode, pvObject, pcName, (TraceUnsignedBaseType_t)uxParameter);
}

void prvTraceObjectUnregister(uint32_t eventCode, void* pvObject, TraceKernelPortParam_t uxParameter)
{
    (void)xTraceObjectUnregisterWithoutHandle(eventCode, pvObject, (TraceUnsignedBaseType_t)uxParameter);
}

void prvTraceObjectSetName(void* pvObject, const char* szName)
{
    (void)xTraceObjectSetNameWithoutHandle(pvObject, szName);
}

void prvTraceHeapAlloc(void* pvAddress, TraceKernelPortParam_t uiSize)
{
	if (xTraceIsRecorderEnabled())
	{
		(void)xTraceHeapAlloc(xTraceKernelPortGetSystemHeapHandle(), pvAddress, (TraceUnsignedBaseType_t)uiSize);
	}
}

void prvTraceHeapFree(void* pvAddress, TraceKernelPortParam_t uiSize)
{
    if (xTraceIsRecorderEnabled())
    {
        (void)xTraceHeapFree(xTraceKernelPortGetSystemHeapHandle(), pvAddress, (TraceUnsignedBaseType_t)uiSize);
    }
}
