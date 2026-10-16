#include "miniface_siwg917_network.h"

#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "sl_net.h"
#include "sl_net_wifi_types.h"
#include "sl_wifi.h"
#include "sl_wifi_callback_framework.h"

#define NETWORK_AP_MAX 16u
#define NETWORK_SSID_MAX 32u
#define NETWORK_PASSWORD_MAX 64u
#define NETWORK_SCAN_TIMEOUT_MS 15000u

typedef enum {
  NETWORK_REQUEST_NONE = 0,
  NETWORK_REQUEST_SCAN,
  NETWORK_REQUEST_CONNECT,
  NETWORK_REQUEST_DISCONNECT,
  NETWORK_REQUEST_FORGET,
  NETWORK_REQUEST_SHUTDOWN,
} network_request_type_t;

typedef struct {
  network_request_type_t type;
  char ssid[NETWORK_SSID_MAX + 1u];
  char password[NETWORK_PASSWORD_MAX + 1u];
} network_request_t;

static td_net_status_t s_status = {
  .message = "Ready. Scan or enter a network name.",
};
static td_wifi_ap_t s_access_points[NETWORK_AP_MAX];
static network_request_t s_request;
static volatile bool s_request_pending;
static volatile bool s_worker_running;
static volatile bool s_shutdown_requested;
static volatile bool s_initialized;
static volatile bool s_scan_active;
static volatile bool s_scan_complete;
static volatile bool s_started;
static volatile sl_status_t s_scan_status;
static unsigned int s_access_point_count;

static void copy_text(char *destination, size_t capacity, const char *source)
{
  if (capacity == 0u) {
    return;
  }

  size_t index = 0u;
  if (source != NULL) {
    while (index + 1u < capacity && source[index] != '\0') {
      destination[index] = source[index];
      ++index;
    }
  }
  destination[index] = '\0';
}

static void set_message(const char *message)
{
  taskENTER_CRITICAL();
  copy_text(s_status.message, sizeof(s_status.message), message);
  taskEXIT_CRITICAL();
}

static void finish_request(const char *message)
{
  taskENTER_CRITICAL();
  s_status.busy = false;
  copy_text(s_status.message, sizeof(s_status.message), message);
  taskEXIT_CRITICAL();
}

static void format_ipv4(char destination[16], const uint8_t bytes[4])
{
  (void)snprintf(destination, 16u, "%u.%u.%u.%u",
                 (unsigned int)bytes[0], (unsigned int)bytes[1],
                 (unsigned int)bytes[2], (unsigned int)bytes[3]);
}

static sl_status_t network_event_handler(sl_net_event_t event,
                                         sl_status_t status,
                                         void *data,
                                         uint32_t data_length)
{
  (void)event;
  (void)status;
  (void)data;
  (void)data_length;
  return SL_STATUS_OK;
}

static sl_status_t scan_callback(sl_wifi_event_t event,
                                 sl_status_t status,
                                 sl_wifi_scan_result_t *results,
                                 uint32_t results_length,
                                 void *argument)
{
  (void)event;
  (void)results_length;
  (void)argument;

  if (SL_WIFI_CHECK_IF_EVENT_FAILED(event) || status != SL_STATUS_OK || results == NULL) {
    s_scan_status = status;
    s_scan_complete = true;
    return status;
  }

  taskENTER_CRITICAL();
  const uint32_t count = results->scan_count < NETWORK_AP_MAX ? results->scan_count : NETWORK_AP_MAX;
  s_access_point_count = (unsigned int)count;
  for (uint32_t index = 0u; index < count; ++index) {
    td_wifi_ap_t *access_point = &s_access_points[index];
    const uint8_t *ssid = results->scan_info[index].ssid;
    size_t length = 0u;
    while (length < NETWORK_SSID_MAX && ssid[length] != 0u) {
      access_point->ssid[length] = (char)ssid[length];
      ++length;
    }
    access_point->ssid[length] = '\0';
    access_point->rssi = -(int)results->scan_info[index].rssi_val;
    access_point->secure = results->scan_info[index].security_mode != SL_WIFI_OPEN;
    access_point->saved = s_status.wifi_up
                          && strcmp(access_point->ssid, s_status.ssid) == 0;
  }
  taskEXIT_CRITICAL();
  s_scan_status = SL_STATUS_OK;
  s_scan_complete = true;
  return SL_STATUS_OK;
}

static sl_status_t ensure_initialized(void)
{
  if (s_initialized) {
    return SL_STATUS_OK;
  }

  set_message("Starting SiWG917 Wi-Fi...");
  sl_status_t status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE,
                                   NULL,
                                   NULL,
                                   network_event_handler);
  if (status == SL_STATUS_OK) {
    s_initialized = true;
  }
  return status;
}

static void run_scan(void)
{
  sl_status_t status = ensure_initialized();
  if (status != SL_STATUS_OK) {
    finish_request("Wi-Fi stack could not start.");
    return;
  }

  sl_wifi_scan_configuration_t configuration = { 0 };
  configuration.type = SL_WIFI_SCAN_TYPE_ACTIVE;
  // A zero bitmap means "scan no channels". Match the SDK default and
  // actively probe every 2.4 GHz Wi-Fi channel supported by the SiWG917.
  configuration.channel_bitmap_2g4 = 0xffffu;
  set_message("Scanning all 2.4 GHz Wi-Fi channels...");
  s_scan_active = true;
  s_scan_complete = false;
  s_scan_status = SL_STATUS_FAIL;
  status = sl_wifi_set_scan_callback_v2(scan_callback, NULL);
  if (status == SL_STATUS_OK) {
    status = sl_wifi_start_scan(SL_WIFI_CLIENT_2_4GHZ_INTERFACE,
                                NULL,
                                &configuration);
  }

  if (status == SL_STATUS_IN_PROGRESS) {
    const TickType_t started_at = xTaskGetTickCount();
    const TickType_t timeout_ticks = pdMS_TO_TICKS(NETWORK_SCAN_TIMEOUT_MS);
    while (!s_scan_complete && (xTaskGetTickCount() - started_at) <= timeout_ticks) {
      vTaskDelay(pdMS_TO_TICKS(10u));
    }
    status = s_scan_complete ? s_scan_status : SL_STATUS_TIMEOUT;
  }

  s_scan_active = false;
  if (status == SL_STATUS_OK) {
    char message[sizeof(s_status.message)];
    const unsigned int count = s_access_point_count;
    (void)snprintf(message, sizeof(message), "%u network%s found.",
                   count, count == 1u ? "" : "s");
    finish_request(message);
  } else if (status == SL_STATUS_TIMEOUT) {
    finish_request("Scan timed out. Use Join... to connect manually.");
  } else {
    finish_request("Scan failed. Use Join... or retry.");
  }
}

static void run_connect(const network_request_t *request)
{
  sl_status_t status = ensure_initialized();
  if (status != SL_STATUS_OK) {
    finish_request("Wi-Fi stack could not start.");
    return;
  }

  (void)sl_net_down(SL_NET_WIFI_CLIENT_INTERFACE);

  sl_net_wifi_client_profile_t profile = { 0 };
  const size_t ssid_length = strlen(request->ssid);
  const size_t password_length = strlen(request->password);

  profile.config.ssid.length = (uint8_t)ssid_length;
  memcpy(profile.config.ssid.value, request->ssid, ssid_length);
  profile.config.channel.channel = SL_WIFI_AUTO_CHANNEL;
  profile.config.channel.band = SL_WIFI_AUTO_BAND;
  profile.config.channel.bandwidth = SL_WIFI_AUTO_BANDWIDTH;
  profile.config.channel_bitmap.channel_bitmap_2_4 = SL_WIFI_DEFAULT_CHANNEL_BITMAP;
  profile.config.bss_type = SL_WIFI_BSS_TYPE_INFRASTRUCTURE;
  profile.config.encryption = SL_WIFI_DEFAULT_ENCRYPTION;
  profile.config.security = password_length == 0u ? SL_WIFI_OPEN : SL_WIFI_WPA2;
  profile.config.credential_id = password_length == 0u
                                   ? SL_WIFI_NO_CREDENTIAL_ID
                                   : SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID;
  profile.ip.mode = SL_IP_MANAGEMENT_DHCP;
  profile.ip.type = SL_IPV4;

  if (password_length != 0u) {
    status = sl_net_set_credential(SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID,
                                   SL_NET_WIFI_PSK,
                                   request->password,
                                   (uint32_t)password_length);
  }
  if (status == SL_STATUS_OK) {
    status = sl_net_set_profile(SL_NET_WIFI_CLIENT_INTERFACE,
                                SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID,
                                (const sl_net_profile_t *)&profile);
  }
  if (status == SL_STATUS_OK) {
    status = sl_net_up(SL_NET_WIFI_CLIENT_INTERFACE,
                       SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID);
  }
  if (status != SL_STATUS_OK) {
    (void)sl_net_down(SL_NET_WIFI_CLIENT_INTERFACE);
    finish_request("Connection failed. Check SSID and password.");
    return;
  }

  sl_net_ip_address_t ip_address = { 0 };
  status = sl_net_get_ip_address(SL_NET_WIFI_CLIENT_INTERFACE,
                                 &ip_address,
                                 10000u);
  if (status != SL_STATUS_OK) {
    (void)sl_net_down(SL_NET_WIFI_CLIENT_INTERFACE);
    finish_request("Connected, but DHCP did not supply an IP.");
    return;
  }

  taskENTER_CRITICAL();
  s_status.wifi_up = true;
  copy_text(s_status.ssid, sizeof(s_status.ssid), request->ssid);
  format_ipv4(s_status.wifi_ip, ip_address.v4.ip_address.bytes);
  for (unsigned int index = 0u; index < s_access_point_count; ++index) {
    s_access_points[index].saved = strcmp(s_access_points[index].ssid, request->ssid) == 0;
    if (s_access_points[index].saved) {
      s_status.rssi = s_access_points[index].rssi;
    }
  }
  s_status.busy = false;
  (void)snprintf(s_status.message, sizeof(s_status.message), "Connected. DHCP address assigned.");
  taskEXIT_CRITICAL();
}

static void run_disconnect(void)
{
  if (s_initialized) {
    (void)sl_net_down(SL_NET_WIFI_CLIENT_INTERFACE);
  }
  taskENTER_CRITICAL();
  s_status.wifi_up = false;
  s_status.ssid[0] = '\0';
  s_status.wifi_ip[0] = '\0';
  s_status.rssi = 0;
  taskEXIT_CRITICAL();
  finish_request("Disconnected.");
}

static void run_forget(void)
{
  run_disconnect();
  set_message("Disconnected. Credentials remain only until replaced.");
}

static void run_shutdown(void)
{
  sl_status_t status = SL_STATUS_OK;

  if (s_initialized) {
    (void)sl_net_down(SL_NET_WIFI_CLIENT_INTERFACE);
    status = sl_net_deinit(SL_NET_WIFI_CLIENT_INTERFACE);
    if (status == SL_STATUS_OK) {
      s_initialized = false;
    }
  }

  taskENTER_CRITICAL();
  s_scan_active = false;
  s_scan_complete = false;
  s_access_point_count = 0u;
  s_status.wifi_up = false;
  s_status.ssid[0] = '\0';
  s_status.wifi_ip[0] = '\0';
  s_status.rssi = 0;
  s_status.busy = false;
  copy_text(s_status.message, sizeof(s_status.message),
            status == SL_STATUS_OK ? "Wi-Fi off. Resources released."
                                   : "Wi-Fi shutdown failed.");
  taskEXIT_CRITICAL();
}

static void network_task(void *argument);

static bool start_worker(void)
{
  /* AIML runs at priority 4. A short-lived Network task must run above it. */
  if (xTaskCreate(network_task, "td_network", 2048u, NULL, 5u, NULL) == pdPASS) {
    return true;
  }

  taskENTER_CRITICAL();
  s_request_pending = false;
  s_worker_running = false;
  s_status.busy = false;
  copy_text(s_status.message, sizeof(s_status.message), "Network worker unavailable.");
  taskEXIT_CRITICAL();
  return false;
}

static bool submit_request(network_request_type_t type, const char *ssid, const char *password)
{
  taskENTER_CRITICAL();
  if (s_request_pending || s_worker_running || s_status.busy) {
    taskEXIT_CRITICAL();
    return false;
  }
  s_request.type = type;
  copy_text(s_request.ssid, sizeof(s_request.ssid), ssid);
  copy_text(s_request.password, sizeof(s_request.password), password);
  s_request_pending = true;
  s_worker_running = true;
  s_status.busy = true;
  copy_text(s_status.message, sizeof(s_status.message), "Working...");
  taskEXIT_CRITICAL();
  return start_worker();
}

static void network_task(void *argument)
{
  (void)argument;
  network_request_t request = { 0 };
  bool pending = false;
  bool shutdown = false;

  taskENTER_CRITICAL();
  if (s_request_pending) {
    request = s_request;
    s_request.type = NETWORK_REQUEST_NONE;
    s_request_pending = false;
    pending = true;
  }
  taskEXIT_CRITICAL();

  if (pending) {
    switch (request.type) {
      case NETWORK_REQUEST_SCAN:
        run_scan();
        break;
      case NETWORK_REQUEST_CONNECT:
        run_connect(&request);
        break;
      case NETWORK_REQUEST_DISCONNECT:
        run_disconnect();
        break;
      case NETWORK_REQUEST_FORGET:
        run_forget();
        break;
      case NETWORK_REQUEST_SHUTDOWN:
        run_shutdown();
        break;
      default:
        finish_request("No network action selected.");
        break;
    }
  }

  taskENTER_CRITICAL();
  shutdown = s_shutdown_requested;
  s_shutdown_requested = false;
  taskEXIT_CRITICAL();
  if (shutdown && request.type != NETWORK_REQUEST_SHUTDOWN) {
    run_shutdown();
  }

  taskENTER_CRITICAL();
  s_worker_running = false;
  taskEXIT_CRITICAL();
  vTaskDelete(NULL);
}

static void network_status(td_net_status_t *out)
{
  if (out == NULL) {
    return;
  }
  taskENTER_CRITICAL();
  *out = s_status;
  taskEXIT_CRITICAL();
}

static bool network_scan(void)
{
  return submit_request(NETWORK_REQUEST_SCAN, NULL, NULL);
}

static int network_scan_results(td_wifi_ap_t *out, int maximum)
{
  if (out == NULL || maximum <= 0 || s_scan_active) {
    return s_scan_active ? -1 : 0;
  }
  taskENTER_CRITICAL();
  const unsigned int count = s_access_point_count < (unsigned int)maximum
                               ? s_access_point_count : (unsigned int)maximum;
  for (unsigned int index = 0u; index < count; ++index) {
    out[index] = s_access_points[index];
  }
  taskEXIT_CRITICAL();
  return (int)count;
}

static bool network_connect(const char *ssid, const char *password)
{
  if (ssid == NULL || ssid[0] == '\0') {
    set_message("Enter a Wi-Fi network name first.");
    return false;
  }
  return submit_request(NETWORK_REQUEST_CONNECT, ssid, password != NULL ? password : "");
}

static bool network_disconnect(void)
{
  return submit_request(NETWORK_REQUEST_DISCONNECT, NULL, NULL);
}

static bool network_forget(const char *ssid)
{
  (void)ssid;
  return submit_request(NETWORK_REQUEST_FORGET, NULL, NULL);
}

static void network_shutdown(void)
{
  bool start = false;

  taskENTER_CRITICAL();
  if (s_worker_running) {
    s_shutdown_requested = true;
  } else if (s_initialized) {
    s_request.type = NETWORK_REQUEST_SHUTDOWN;
    s_request_pending = true;
    s_worker_running = true;
    s_status.busy = true;
    copy_text(s_status.message, sizeof(s_status.message), "Powering Wi-Fi off...");
    start = true;
  } else {
    s_scan_active = false;
    s_scan_complete = false;
    s_access_point_count = 0u;
    s_status.busy = false;
    copy_text(s_status.message, sizeof(s_status.message), "Wi-Fi off.");
  }
  taskEXIT_CRITICAL();

  if (start) {
    (void)start_worker();
  }
}

static const td_net_ops_t s_network_ops = {
  .status = network_status,
  .scan = network_scan,
  .scan_results = network_scan_results,
  .connect = network_connect,
  .disconnect = network_disconnect,
  .forget = network_forget,
  .shutdown = network_shutdown,
};

bool miniface_siwg917_network_start(void)
{
  if (s_started) {
    return true;
  }
  /* Wi-Fi and its worker are created only when the Network app asks for them. */
  s_started = true;
  return true;
}

const td_net_ops_t *miniface_siwg917_network_ops(void)
{
  return &s_network_ops;
}
