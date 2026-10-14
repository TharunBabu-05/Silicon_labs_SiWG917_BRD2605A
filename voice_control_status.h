#ifndef VOICE_CONTROL_STATUS_H
#define VOICE_CONTROL_STATUS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  const char *state;
  const char *label;
  uint8_t score;
  uint32_t inferences;
  uint32_t commands;
} voice_control_status_t;

void voice_control_get_status(voice_control_status_t *out);

#ifdef __cplusplus
}
#endif

#endif
