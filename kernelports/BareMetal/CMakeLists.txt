# CMakeLists.txt for kernelports/BareMetal
# In order to include TraceRecorder source files and headers, this CMakeLists.txt
# exports the TRACERECORDER_SRCS and TRACERECORDER_INCLUDES variables to its parent scope.
# These can be used to build an executable that includes TraceRecorder support.
# Alternatively, if a TRACERECORDER_CONFIG_DIR variable is defined in the parent scope,
# this file will create a TraceRecorder static library that can instead be linked to the executable.

cmake_minimum_required(VERSION 3.16)

project(TraceRecorderProject LANGUAGES C)

set(TRACERECORDER_DIR "${CMAKE_CURRENT_LIST_DIR}/../..")
set(KERNELPORT_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(STREAMPORT_DIR "${TRACERECORDER_DIR}/streamports/File")

# Export variables to parent scope
file(GLOB TRACERECORDER_SRCS "${TRACERECORDER_DIR}/*.c" "${KERNELPORT_DIR}/*.c" "${STREAMPORT_DIR}/*.c" PARENT_SCOPE)
set(TRACERECORDER_INCLUDES "${TRACERECORDER_DIR}/include" "${KERNELPORT_DIR}/include" "${STREAMPORT_DIR}/include" PARENT_SCOPE)

if (DEFINED TRACERECORDER_CONFIG_DIR)
    # Use the provided configuration directory to create the TraceRecorder library
    add_library(TraceRecorder STATIC ${TRACERECORDER_SRCS})
    target_include_directories(TraceRecorder
        PUBLIC
            "${TRACERECORDER_DIR}/include"
            "${KERNELPORT_DIR}/include"
            "${STREAMPORT_DIR}/include"
            ${TRACERECORDER_CONFIG_DIR}
        )
endif()
