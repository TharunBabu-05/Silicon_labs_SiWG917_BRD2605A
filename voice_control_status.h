#ifndef VOICE_CONTROL_STATUS_H
#define VOICE_CONTROL_STATUS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VOICE_CONTROL_WAVEFORM_SAMPLES 64u

typedef struct {
  const char *state;
  const char *label;
  uint8_t score;
  uint32_t inferences;
  uint32_t commands;
  int16_t baseline;
  uint8_t level_percent;
  uint8_t peak_percent;
  uint8_t microphones_active;
  uint32_t audio_samples;
  int16_t waveform[VOICE_CONTROL_WAVEFORM_SAMPLES];
} voice_control_status_t;

void voice_control_get_status(voice_control_status_t *out);
void voice_control_request_microphones(void);

#ifdef __cplusplus
}
#endif

#endif
