#ifndef TINYDESK_SIWG917_NETWORK_H
#define TINYDESK_SIWG917_NETWORK_H

#include <stdbool.h>

#include "tinydesk/td_sysinfo.h"

bool tinydesk_siwg917_network_start(void);
const td_net_ops_t *tinydesk_siwg917_network_ops(void);

#endif
