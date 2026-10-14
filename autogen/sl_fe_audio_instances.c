/***************************************************************************//**
 * @file sl_fe_audio_instances.c
 * @brief Audio frontend per-instance @c sl_fe_audio_<inst>_cfg definitions (autogen)
 *******************************************************************************
 * SPDX-License-Identifier: Zlib
 *
 * Defines @c const sl_fe_audio_<inst>_cfg for each component instance.
 * Include this from one .c/.cc per translation unit that needs the symbols (including
 * multiple TUs duplicates ROM for each static copy).
 *
 * Each instance pulls SL_FE_* symbols from sl_fe_audio_<inst>_config.h (per-model
 * TFLite metadata). When present, model-scoped macros come from
 * @c <inst>_generated_parameters.h (AIML compiler:
 * @c ModelParametersHeaderSettings.filename in @c aiml/tool/compiler/app/api/_types.py).
 * If that header is absent, @c sl_fe_audio_<inst>_config.h supplies @c #ifndef defaults.
 ******************************************************************************/

#include <stdbool.h>

#if __has_include("keyword_spotting_on_off_v3_generated_parameters.h")
#include "keyword_spotting_on_off_v3_generated_parameters.h"
#else
#pragma message("keyword_spotting_on_off_v3_generated_parameters.h not found; using defaults from sl_fe_audio_keyword_spotting_on_off_v3_config.h")
#endif
#include "sl_fe_audio_keyword_spotting_on_off_v3_config.h"

#include "sl_fe_audio_instances.h"

#ifdef __cplusplus
extern "C" {
#endif




const sl_fe_audio_config_t sl_fe_audio_keyword_spotting_on_off_v3_cfg = {
  .frontend_config = {
    .window = {
      .size_ms = (size_t)SL_FE_FRONTEND_KEYWORD_SPOTTING_ON_OFF_V3_WINDOW_SIZE_MS,
      .step_size_ms = (size_t)SL_FE_FRONTEND_KEYWORD_SPOTTING_ON_OFF_V3_WINDOW_STEP_MS,
    },
    .filterbank = {
      .num_channels = SL_FE_FRONTEND_KEYWORD_SPOTTING_ON_OFF_V3_FILTERBANK_N_CHANNELS,
      .upper_band_limit = SL_FE_FRONTEND_KEYWORD_SPOTTING_ON_OFF_V3_FILTERBANK_UPPER_BAND_LIMIT,
      .lower_band_limit = SL_FE_FRONTEND_KEYWORD_SPOTTING_ON_OFF_V3_FILTERBANK_LOWER_BAND_LIMIT,
      .output_scale_shift = 0,
    },
    .noise_reduction = {
      .enable_noise_reduction = SL_FE_FRONTEND_KEYWORD_SPOTTING_ON_OFF_V3_NOISE_REDUCTION_ENABLE,
      .smoothing_bits = SL_FE_FRONTEND_KEYWORD_SPOTTING_ON_OFF_V3_NOISE_REDUCTION_SMOOTHING_BITS,
      .even_smoothing = SL_FE_FRONTEND_KEYWORD_SPOTTING_ON_OFF_V3_NOISE_REDUCTION_EVEN_SMOOTHING,
      .odd_smoothing = SL_FE_FRONTEND_KEYWORD_SPOTTING_ON_OFF_V3_NOISE_REDUCTION_ODD_SMOOTHING,
      .min_signal_remaining = SL_FE_FRONTEND_KEYWORD_SPOTTING_ON_OFF_V3_NOISE_REDUCTION_MIN_SIGNAL_REMAINING,
    },
    .pcan_gain_control = {
      .enable_pcan = SL_FE_FRONTEND_KEYWORD_SPOTTING_ON_OFF_V3_PCAN_ENABLE,
      .strength = SL_FE_FRONTEND_KEYWORD_SPOTTING_ON_OFF_V3_PCAN_STRENGTH,
      .offset = SL_FE_FRONTEND_KEYWORD_SPOTTING_ON_OFF_V3_PCAN_OFFSET,
      .gain_bits = SL_FE_FRONTEND_KEYWORD_SPOTTING_ON_OFF_V3_PCAN_GAIN_BITS,
    },
    .log_scale = {
      .enable_log = SL_FE_FRONTEND_KEYWORD_SPOTTING_ON_OFF_V3_LOG_SCALE_ENABLE,
      .scale_shift = SL_FE_FRONTEND_KEYWORD_SPOTTING_ON_OFF_V3_LOG_SCALE_SHIFT,
    },
    .activity_detection = {
      .enable_activation_detection = SL_FE_FRONTEND_KEYWORD_SPOTTING_ON_OFF_V3_ACTIVITY_DETECTION_ENABLE,
      .alpha_a = SL_FE_FRONTEND_KEYWORD_SPOTTING_ON_OFF_V3_ACTIVITY_DETECTION_ALPHA_A,
      .alpha_b = SL_FE_FRONTEND_KEYWORD_SPOTTING_ON_OFF_V3_ACTIVITY_DETECTION_ALPHA_B,
      .arm_threshold = SL_FE_FRONTEND_KEYWORD_SPOTTING_ON_OFF_V3_ACTIVITY_DETECTION_ARM_THRESHOLD,
      .trip_threshold = SL_FE_FRONTEND_KEYWORD_SPOTTING_ON_OFF_V3_ACTIVITY_DETECTION_TRIP_THRESHOLD,
    },
    .dc_notch_filter = {
      .enable_dc_notch_filter = SL_FE_FRONTEND_KEYWORD_SPOTTING_ON_OFF_V3_DC_NOTCH_FILTER_ENABLE,
      .coefficient = SL_FE_FRONTEND_KEYWORD_SPOTTING_ON_OFF_V3_DC_NOTCH_FILTER_COEFFICIENT,
    },
  },
  .sample_rate_hz = SL_FE_FRONTEND_KEYWORD_SPOTTING_ON_OFF_V3_SAMPLE_RATE_HZ,
  .fft_length = (unsigned)SL_FE_FRONTEND_KEYWORD_SPOTTING_ON_OFF_V3_FFT_LENGTH,
  .sample_length_ms = (unsigned)SL_FE_FRONTEND_KEYWORD_SPOTTING_ON_OFF_V3_SAMPLE_LENGTH_MS,
  .audio_buffer_size = (size_t)SL_FE_AUDIO_FEATURE_GENERATION_KEYWORD_SPOTTING_ON_OFF_V3_AUDIO_BUFFER_SIZE,
  .audio_buffer_chunk_size = (size_t)SL_FE_AUDIO_FEATURE_GENERATION_KEYWORD_SPOTTING_ON_OFF_V3_AUDIO_BUFFER_CHUNK_SIZE,
  .audio_gain = SL_FE_AUDIO_FEATURE_GENERATION_KEYWORD_SPOTTING_ON_OFF_V3_AUDIO_GAIN,
  .quantize_dynamic_scale_enable = (int)SL_FE_AUDIO_FEATURE_GENERATION_KEYWORD_SPOTTING_ON_OFF_V3_QUANTIZE_DYNAMIC_SCALE_ENABLE,
  .quantize_dynamic_scale_range_db = SL_FE_AUDIO_FEATURE_GENERATION_KEYWORD_SPOTTING_ON_OFF_V3_QUANTIZE_DYNAMIC_SCALE_RANGE_DB,
  .quantize_feature_range_min = SL_FE_AUDIO_FEATURE_GENERATION_KEYWORD_SPOTTING_ON_OFF_V3_QUANTIZE_FEATURE_RANGE_MIN,
  .quantize_feature_range_max = SL_FE_AUDIO_FEATURE_GENERATION_KEYWORD_SPOTTING_ON_OFF_V3_QUANTIZE_FEATURE_RANGE_MAX,
  .samplewise_norm_rescale = (float)SL_FE_AUDIO_FEATURE_GENERATION_KEYWORD_SPOTTING_ON_OFF_V3_SAMPLEWISE_NORM_RESCALE,
  .samplewise_norm_mean_and_std = SL_FE_AUDIO_FEATURE_GENERATION_KEYWORD_SPOTTING_ON_OFF_V3_SAMPLEWISE_NORM_MEAN_AND_STD,
};


#ifdef __cplusplus
}
#endif
