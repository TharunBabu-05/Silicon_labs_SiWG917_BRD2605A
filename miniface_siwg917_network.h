#ifndef MINIFACE_SIWG917_NETWORK_H
#define MINIFACE_SIWG917_NETWORK_H

#include <stdbool.h>

#include "miniface/td_sysinfo.h"

bool miniface_siwg917_network_start(void);
const td_net_ops_t *miniface_siwg917_network_ops(void);

#endif
