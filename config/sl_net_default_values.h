#pragma once

#include "sl_net_wifi_types.h"

#define REQUIRED_IP_TYPE SL_IPV4

/*
 * MiniFace supplies credentials at runtime from the Network window.  These
 * empty defaults only satisfy the Network Manager during its initial setup.
 */
#define DEFAULT_WIFI_CLIENT_PROFILE \
  (sl_net_wifi_client_profile_t) { \
    .config = { \
      .ssid.length = 0u, \
      .channel.channel = SL_WIFI_AUTO_CHANNEL, \
      .channel.band = SL_WIFI_AUTO_BAND, \
      .channel.bandwidth = SL_WIFI_AUTO_BANDWIDTH, \
      .channel_bitmap.channel_bitmap_2_4 = SL_WIFI_DEFAULT_CHANNEL_BITMAP, \
      .bss_type = SL_WIFI_BSS_TYPE_INFRASTRUCTURE, \
      .security = SL_WIFI_OPEN, \
      .encryption = SL_WIFI_DEFAULT_ENCRYPTION, \
      .credential_id = SL_WIFI_NO_CREDENTIAL_ID, \
    }, \
    .ip = { .mode = SL_IP_MANAGEMENT_DHCP, .type = REQUIRED_IP_TYPE }, \
    .priority = 0u, \
  }

#define DEFAULT_WIFI_ACCESS_POINT_PROFILE \
  (sl_net_wifi_ap_profile_t) { \
    .config = { \
      .ssid.length = 0u, \
      .channel.channel = SL_WIFI_AUTO_CHANNEL, \
      .channel.band = SL_WIFI_AUTO_BAND, \
      .channel.bandwidth = SL_WIFI_AUTO_BANDWIDTH, \
      .security = SL_WIFI_OPEN, \
      .encryption = SL_WIFI_NO_ENCRYPTION, \
      .credential_id = SL_WIFI_NO_CREDENTIAL_ID, \
    }, \
    .ip = { .mode = SL_IP_MANAGEMENT_STATIC_IP, .type = SL_IPV4 }, \
  }

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif
static sl_net_wifi_psk_credential_entry_t default_wifi_client_credential = {
  .type = SL_NET_WIFI_PSK,
  .data_length = 0u,
};

static sl_net_wifi_psk_credential_entry_t default_wifi_ap_credential = {
  .type = SL_NET_WIFI_PSK,
  .data_length = 0u,
};
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
