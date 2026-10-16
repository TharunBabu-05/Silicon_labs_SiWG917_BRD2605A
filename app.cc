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

#include "miniface_siwg917.h"

#include "FreeRTOS.h"
#include "task.h"
#include "sl_iostream.h"
#include "sl_iostream_init_si91x_uart.h"

#include <cstring>

static void vcom_boot_message(const char* message) {
    (void)sl_iostream_write(sl_iostream_vcom_handle, message, std::strlen(message));
}

static volatile bool s_voice_task_started;

static void voice_task(void* argument) {
    (void)argument;
    voice_control_light_task();
    taskENTER_CRITICAL();
    s_voice_task_started = false;
    taskEXIT_CRITICAL();
    vTaskDelete(nullptr);
}

extern "C" bool voice_control_start(void) {
    taskENTER_CRITICAL();
    if (s_voice_task_started) {
        taskEXIT_CRITICAL();
        return true;
    }
    s_voice_task_started = true;
    taskEXIT_CRITICAL();

    if (xTaskCreate(voice_task, "miniface_audio", 1536, nullptr, 4, nullptr) == pdPASS) {
        return true;
    }

    taskENTER_CRITICAL();
    s_voice_task_started = false;
    taskEXIT_CRITICAL();
    vcom_boot_message("MiniFace: audio task creation failed\r\n");
    return false;
}

/*******************************************************************************
 * Initialize application.
 *
 ******************************************************************************/
extern "C" void app_init(void) {
    ml_configure_clocks_to_max_rate();

    led_init();
    vcom_boot_message("\r\nMiniFace: application startup\r\n");

    if (!miniface_siwg917_start()) {
        vcom_boot_message("MiniFace: task creation failed\r\n");
        return;
    }
    vcom_boot_message("MiniFace: VCOM and desktop tasks created\r\n");
    vcom_boot_message("MiniFace: audio service starts when an audio app opens\r\n");
}

/*******************************************************************************
 * App ticking function.
 ******************************************************************************/
extern "C" void app_process_action(void) {
    // MiniFace is driven by its own FreeRTOS task.
}
