/***************************************************************************/ /**
 * @file app.c
 * @brief Top level application functions
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "app.h"

#include "ml/platform/ml_clock_helper.h"
#include "sl_sleeptimer.h"
#include "voice_control_light.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "led_helper.hpp"
#ifdef __cplusplus
}
#endif

#include "tinydesk_siwg917.h"

#include "FreeRTOS.h"
#include "task.h"
#include "sl_iostream.h"
#include "sl_iostream_init_si91x_uart.h"

#include <cstring>

static void vcom_boot_message(const char* message) {
    (void)sl_iostream_write(sl_iostream_vcom_handle, message, std::strlen(message));
}

static void voice_task(void* argument) {
    (void)argument;
    // Signal Analyzer requests the shared dual-microphone pipeline on demand.
    voice_control_light_task();
    vTaskDelete(nullptr);
}

/*******************************************************************************
 * Initialize application.
 *
 ******************************************************************************/
extern "C" void app_init(void) {
    ml_configure_clocks_to_max_rate();

    // Initialize the LEDs
    led_init();
    vcom_boot_message("\r\nTinyDesk: application startup\r\n");

    // The serial desktop is independent of AIML model initialization.
    if (!tinydesk_siwg917_start()) {
        vcom_boot_message("TinyDesk: task creation failed\r\n");
        return;
    }
    vcom_boot_message("TinyDesk: VCOM and desktop tasks created\r\n");
    if (xTaskCreate(voice_task, "aiml", 1536, nullptr, 4, nullptr) != pdPASS) {
        vcom_boot_message("TinyDesk: AIML task creation failed\r\n");
    }
}

/*******************************************************************************
 * App ticking function.
 ******************************************************************************/
extern "C" void app_process_action(void) {
    // TinyDesk and AIML run in their own FreeRTOS tasks.
}
