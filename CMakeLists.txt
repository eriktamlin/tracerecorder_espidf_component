# Only add source/header dirs when Tracerecorder is enabled. This avoids
# conditional Kconfig parameters in headers when the recorder is disabled.
if(CONFIG_ESP_TRACE_LIB_EXTERNAL AND CONFIG_PERCEPIO_TRACERECORDER_ENABLED)
    set(src_dirs "")
    set(include_dirs "")

    if(CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_RINGBUFFER)
        set(src_dirs
            ${CMAKE_CURRENT_LIST_DIR}
            ${CMAKE_CURRENT_LIST_DIR}/streamports/RingBuffer
            ${CMAKE_CURRENT_LIST_DIR}/kernelports/ESP-IDF_FreeRTOS)
        set(include_dirs
            ${CMAKE_CURRENT_LIST_DIR}/include
            ${CMAKE_CURRENT_LIST_DIR}/kernelports/ESP-IDF_FreeRTOS/config
            ${CMAKE_CURRENT_LIST_DIR}/kernelports/ESP-IDF_FreeRTOS/include
            ${CMAKE_CURRENT_LIST_DIR}/streamports/RingBuffer/include
            ${CMAKE_CURRENT_LIST_DIR}/kernelports/ESP-IDF_FreeRTOS/streamports/RingBuffer/config)
    endif()
    if(CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_RTT)
        set(src_dirs
            ${CMAKE_CURRENT_LIST_DIR}
            ${CMAKE_CURRENT_LIST_DIR}/streamports/Jlink_RTT
            ${CMAKE_CURRENT_LIST_DIR}/kernelports/ESP-IDF_FreeRTOS)
        set(include_dirs
            ${CMAKE_CURRENT_LIST_DIR}/include
            ${CMAKE_CURRENT_LIST_DIR}/kernelports/ESP-IDF_FreeRTOS/config
            ${CMAKE_CURRENT_LIST_DIR}/kernelports/ESP-IDF_FreeRTOS/include
            ${CMAKE_CURRENT_LIST_DIR}/streamports/Jlink_RTT/include
            ${CMAKE_CURRENT_LIST_DIR}/streamports/Jlink_RTT/config)
    endif()
    if(CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_ESP_IDF_APPTRACE)
        set(src_dirs
            ${CMAKE_CURRENT_LIST_DIR}
            ${CMAKE_CURRENT_LIST_DIR}/kernelports/ESP-IDF_FreeRTOS/streamports/ESP_IDF_APPTRACE
            ${CMAKE_CURRENT_LIST_DIR}/kernelports/ESP-IDF_FreeRTOS)
        set(include_dirs
            ${CMAKE_CURRENT_LIST_DIR}/include
            ${CMAKE_CURRENT_LIST_DIR}/kernelports/ESP-IDF_FreeRTOS/config
            ${CMAKE_CURRENT_LIST_DIR}/kernelports/ESP-IDF_FreeRTOS/include
            ${CMAKE_CURRENT_LIST_DIR}/kernelports/ESP-IDF_FreeRTOS/streamports/ESP_IDF_APPTRACE/include)
    endif()
    if(CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_TCPIP)
        set(src_dirs
            ${CMAKE_CURRENT_LIST_DIR}
            ${CMAKE_CURRENT_LIST_DIR}/streamports/TCPIP
            ${CMAKE_CURRENT_LIST_DIR}/kernelports/ESP-IDF_FreeRTOS)
        set(include_dirs
            ${CMAKE_CURRENT_LIST_DIR}/include
            ${CMAKE_CURRENT_LIST_DIR}/kernelports/ESP-IDF_FreeRTOS/config
            ${CMAKE_CURRENT_LIST_DIR}/kernelports/ESP-IDF_FreeRTOS/include
            ${CMAKE_CURRENT_LIST_DIR}/streamports/TCPIP/include
            ${CMAKE_CURRENT_LIST_DIR}/kernelports/ESP-IDF_FreeRTOS/streamports/TCPIP/config)
    endif()
    if(CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_UDP)
        set(src_dirs
            ${CMAKE_CURRENT_LIST_DIR}
            ${CMAKE_CURRENT_LIST_DIR}/streamports/UDP
            ${CMAKE_CURRENT_LIST_DIR}/kernelports/ESP-IDF_FreeRTOS)
        set(include_dirs
            ${CMAKE_CURRENT_LIST_DIR}/include
            ${CMAKE_CURRENT_LIST_DIR}/kernelports/ESP-IDF_FreeRTOS/config
            ${CMAKE_CURRENT_LIST_DIR}/kernelports/ESP-IDF_FreeRTOS/include
            ${CMAKE_CURRENT_LIST_DIR}/streamports/UDP/include
            ${CMAKE_CURRENT_LIST_DIR}/kernelports/ESP-IDF_FreeRTOS/streamports/UDP/config)
    endif()
    if(CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_FILE)
        set(src_dirs
            ${CMAKE_CURRENT_LIST_DIR}
            ${CMAKE_CURRENT_LIST_DIR}/streamports/File
            ${CMAKE_CURRENT_LIST_DIR}/kernelports/ESP-IDF_FreeRTOS)
        set(include_dirs
            ${CMAKE_CURRENT_LIST_DIR}/include
            ${CMAKE_CURRENT_LIST_DIR}/kernelports/ESP-IDF_FreeRTOS/config
            ${CMAKE_CURRENT_LIST_DIR}/kernelports/ESP-IDF_FreeRTOS/include
            ${CMAKE_CURRENT_LIST_DIR}/streamports/File/include
            ${CMAKE_CURRENT_LIST_DIR}/kernelports/ESP-IDF_FreeRTOS/streamports/File/config)
    endif()

    # Same paths idf_build_set_property used to inject globally; component INCLUDE_DIRS covers them.
    if(NOT include_dirs)
        list(APPEND include_dirs
            ${CMAKE_CURRENT_LIST_DIR}/kernelports/ESP-IDF_FreeRTOS/config
            ${CMAKE_CURRENT_LIST_DIR}/kernelports/ESP-IDF_FreeRTOS/include
            ${CMAKE_CURRENT_LIST_DIR}/include)
    endif()

    idf_component_register(SRC_DIRS "${src_dirs}"
                           INCLUDE_DIRS "${include_dirs}"
                           LDFRAGMENTS "kernelports/ESP-IDF_FreeRTOS/lifra.fr"
                           # REQUIRES can't depend on config, so require all; only used modules end up in the image.
                           # RingBuffer: esp_timer
                           # ESP-IDF APPTRACE: esp_timer app_trace
                           # TCP/IP and UDP: esp_timer esp_event esp_netif
                           REQUIRES esp_timer esp_event esp_netif esp_trace app_trace
                           WHOLE_ARCHIVE TRUE
                           COMPONENT_SRCS adapter_encoder_percepio.c trcKernelPortHooks.c)

    # Expose recorder headers to the freertos component so it can find esp_trace_freertos_impl.h
    idf_component_get_property(freertos_lib freertos COMPONENT_LIB)
    target_include_directories(${freertos_lib} INTERFACE ${include_dirs})
else()
    idf_component_register(REQUIRES esp_timer esp_event esp_netif esp_trace app_trace)
endif()
