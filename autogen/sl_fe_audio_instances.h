/***************************************************************************//**
 * @file sl_fe_audio_instances.h
 * @brief Audio frontend per-instance descriptor declarations (autogen)
 *******************************************************************************
 * SPDX-License-Identifier: Zlib
 *
 * Declares @c extern const sl_fe_audio_<inst>_cfg for each component instance.
 * Include this from one .c/.cc per translation unit that needs the symbols (including
 * multiple TUs duplicates ROM for each static copy).
 *
 * Each instance pulls SL_FE_* symbols from sl_fe_audio_<inst>_config.h (per-model
 * TFLite metadata). Model-scoped macros come from @c <inst>_generated_parameters.h
 * (AIML compiler: @c ModelParametersHeaderSettings.filename in
 * @c aiml/tool/compiler/app/api/_types.py).
 ******************************************************************************/
#ifndef SL_FE_AUDIO_INSTANCES_H
#define SL_FE_AUDIO_INSTANCES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#if defined(SL_BOARD_SI91X)
#include "sl_fe_audio_si91x.h"
#else
#include "sl_fe_audio.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif




extern const sl_fe_audio_config_t sl_fe_audio_keyword_spotting_on_off_v3_cfg;





/***************************************************************************//**
 * @brief
 *    Return the first fe_audio instance config (lexicographic instance name).
 *
 * @details
 *    Used by deprecated no-argument @ref sl_ml_audio_feature_generation_init and
 *    @ref sl_ml_audio_feature_generation_frontend_init compatibility wrappers.
 ******************************************************************************/
static inline const sl_fe_audio_config_t *sl_fe_audio_get_default_config(void)
{
  return &sl_fe_audio_keyword_spotting_on_off_v3_cfg;
}


#ifdef __cplusplus
}
#endif

#endif // SL_FE_AUDIO_INSTANCES_H
