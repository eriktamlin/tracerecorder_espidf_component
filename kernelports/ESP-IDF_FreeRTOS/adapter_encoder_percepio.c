/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stddef.h>
#include <stdbool.h>

#include "esp_err.h"
#include "esp_trace_types.h"
#include "esp_trace_registry.h"
#include "esp_trace_port_encoder.h"

/**
 * @brief Initializes percepio encoder.
 *        This function is called for each core.
 *        Adapter implementations do NOT need their own multi-core protection. Core does it for them.
 *
 * @param enc Pointer to the encoder structure. Must not be NULL.
 * @param enc_cfg Pointer to the encoder configuration. Can be NULL for defaults.
 *
 * @return ESP_OK on success, otherwise \see esp_err_t
 */
static esp_err_t init(esp_trace_encoder_t *enc, const void *enc_cfg)
{
    (void)enc;
    (void)enc_cfg;

    /* TODO: Implement the initialization of the encoder */
    static bool initialized = false; // Ensure the encoder is initialized only once unless something todo for both cores

    if (!enc) {
        return ESP_ERR_INVALID_ARG;
    }

    if (initialized) {
        return ESP_OK;
    }

    initialized = true;

    return ESP_OK;
}

/**
 * @brief Panic handler
 *
 * Called during system panic to finalize encoder state.
 *
 * @param enc Pointer to the encoder structure. Must not be NULL.
 * @param info Panic information
 */
static void panic_handler(esp_trace_encoder_t *enc, const void *info)
{
    (void)enc;
    (void)info;
}

static const esp_trace_encoder_vtable_t s_percepio_vt = {
    .init                  = init,
    .panic_handler         = panic_handler,
};

ESP_TRACE_REGISTER_ENCODER("percepio", &s_percepio_vt);
