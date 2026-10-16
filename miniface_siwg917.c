#include "miniface_siwg917.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "sl_iostream.h"
#include "sl_iostream_init_si91x_uart.h"
#include "system_si91x.h"

#include "led_helper.hpp"
#include "miniface/td.h"
#include "td_apps.h"
#include "miniface_siwg917_network.h"
#include "voice_control_status.h"

#define TD_RX_RING_SIZE 512u
#define SIGNAL_SCOPE_MIN_SCALE 512

/* heap_4.c references this application-owned FreeRTOS heap. */
uint8_t ucHeap[configTOTAL_HEAP_SIZE] __attribute__((section(".miniface_psram"), aligned(8)));

static uint8_t s_rx_ring[TD_RX_RING_SIZE];
static volatile unsigned int s_rx_head;
static volatile unsigned int s_rx_tail;
static bool s_started;
static bool s_led_on;

void miniface_siwg917_trace(const char *message)
{
  (void)sl_iostream_write(sl_iostream_vcom_handle, message, strlen(message));
}

static td_window_t *s_status_window;
static td_window_t *s_aiml_window;
static td_window_t *s_led_window;
static td_window_t *s_signal_window;
static td_widget_t *s_status_uptime;
static td_widget_t *s_status_heap;
static td_widget_t *s_status_link;
static td_widget_t *s_aiml_state;
static td_widget_t *s_aiml_label;
static td_widget_t *s_aiml_counts;
static td_widget_t *s_led_state;
static voice_control_status_t s_signal_snapshot;

static void rx_push(uint8_t byte)
{
  taskENTER_CRITICAL();
  unsigned int next = (s_rx_head + 1u) & (TD_RX_RING_SIZE - 1u);
  if (next != s_rx_tail) {
    s_rx_ring[s_rx_head] = byte;
    s_rx_head = next;
  }
  taskEXIT_CRITICAL();
}

static int hal_read_byte(void *context)
{
  (void)context;
  int byte = -1;
  taskENTER_CRITICAL();
  if (s_rx_tail != s_rx_head) {
    byte = s_rx_ring[s_rx_tail];
    s_rx_tail = (s_rx_tail + 1u) & (TD_RX_RING_SIZE - 1u);
  }
  taskEXIT_CRITICAL();
  return byte;
}

static int hal_write(void *context, const uint8_t *buffer, int length)
{
  (void)context;
  if (length <= 0) {
    return 0;
  }
  return sl_iostream_write(sl_iostream_vcom_handle, buffer, (size_t)length) == SL_STATUS_OK ? length : 0;
}

static uint32_t hal_millis(void *context)
{
  (void)context;
  return (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);
}

static void hal_sleep_ms(void *context, uint32_t ms)
{
  (void)context;
  vTaskDelay(pdMS_TO_TICKS(ms == 0u ? 1u : ms));
}

static const td_hal_t s_hal = {
  .read_byte = hal_read_byte,
  .write = hal_write,
  .millis = hal_millis,
  .sleep_ms = hal_sleep_ms,
  .ctx = NULL,
};

static uint32_t sys_free_heap(void)
{
  return xPortGetFreeHeapSize();
}

static uint32_t sys_min_free_heap(void)
{
  return xPortGetMinimumEverFreeHeapSize();
}

static uint32_t sys_total_heap(void)
{
  return configTOTAL_HEAP_SIZE;
}

static int sys_task_count(void)
{
  return (int)uxTaskGetNumberOfTasks();
}

static int sys_cpu_mhz(void)
{
  return (int)(SystemCoreClock / 1000000u);
}

static td_sysinfo_t s_sysinfo = {
  .platform = "SiWG917 BRD2605A",
  .chip = "SiWG917 wireless SoC",
  .sdk_version = "Simplicity SDK 2026.6.1",
  .free_heap = sys_free_heap,
  .min_free_heap = sys_min_free_heap,
  .total_heap = sys_total_heap,
  .task_count = sys_task_count,
  .cpu_mhz = sys_cpu_mhz,
  .extra = "VCOM desktop at 921600 baud; screen pools in PSRAM",
};

static void status_update(void)
{
  uint32_t seconds = td_uptime_ms() / 1000u;
  const td_stats_t *stats = td_stats();
  td_widget_printf(s_status_uptime, "Uptime: %lu s  CPU: %d MHz", (unsigned long)seconds, sys_cpu_mhz());
  td_widget_printf(s_status_heap, "Free heap: %lu / %lu B", (unsigned long)sys_free_heap(),
                   (unsigned long)sys_total_heap());
  td_widget_printf(s_status_link, "VCOM: %s  %lu B/s", stats->link_up ? "connected" : "waiting",
                   (unsigned long)stats->bytes_per_sec);
}

static void aiml_update(void)
{
  voice_control_status_t voice;
  voice_control_get_status(&voice);
  td_widget_printf(s_aiml_state, "State: %s", voice.state ? voice.state : "starting");
  td_widget_printf(s_aiml_label, "Result: %s  score: %u", voice.label ? voice.label : "-", voice.score);
  td_widget_printf(s_aiml_counts, "Inferences: %lu  commands: %lu", (unsigned long)voice.inferences,
                   (unsigned long)voice.commands);
}

static void on_status_tick(td_window_t *window)
{
  (void)window;
  status_update();
}

static void on_aiml_tick(td_window_t *window)
{
  (void)window;
  aiml_update();
}

static void on_status_close(td_window_t *window)
{
  (void)window;
  s_status_window = NULL;
}

static void on_aiml_close(td_window_t *window)
{
  (void)window;
  s_aiml_window = NULL;
}

static void on_led_close(td_window_t *window)
{
  (void)window;
  s_led_window = NULL;
}

static int clamp_int(int value, int low, int high)
{
  if (value < low) {
    return low;
  }
  if (value > high) {
    return high;
  }
  return value;
}

static void draw_signal_scope(td_window_t *window, int width, int height)
{
  (void)window;
  const td_theme_t *theme = td_theme();
  const int graph_x = 1;
  const int graph_y = 3;
  const int graph_width = width - 2;
  const int graph_height = height - 6;
  char line[80];

  snprintf(line, sizeof(line), "Level %3u%%  Peak %3u%%  Base %+6d  Samples %lu",
           (unsigned)s_signal_snapshot.level_percent, (unsigned)s_signal_snapshot.peak_percent,
           (int)s_signal_snapshot.baseline, (unsigned long)s_signal_snapshot.audio_samples);
  td_text(1, 0, line, theme->win_fg, theme->win_bg, TD_BOLD);
  if (s_signal_snapshot.microphones_active != 0u) {
    td_text(1, 1, "Dual microphones powered; live PCM from the shared AIML stream", theme->dim, theme->win_bg, 0);
  } else {
    td_text(1, 1, "Starting the shared dual-microphone pipeline...", theme->accent, theme->win_bg, TD_BOLD);
  }

  if (graph_width < 6 || graph_height < 5) {
    return;
  }

  td_box(td_rect(graph_x, graph_y, graph_width, graph_height), TD_BOX_SINGLE, theme->frame_fg, theme->win_bg);
  const int plot_x = graph_x + 1;
  const int plot_y = graph_y + 1;
  const int plot_width = graph_width - 2;
  const int plot_height = graph_height - 2;
  const int mid_y = plot_y + plot_height / 2;

  for (int x = 0; x < plot_width; ++x) {
    td_putc(plot_x + x, mid_y, '-', theme->dim, theme->win_bg, 0);
  }

  int scale = SIGNAL_SCOPE_MIN_SCALE;
  for (uint32_t i = 0; i < VOICE_CONTROL_WAVEFORM_SAMPLES; ++i) {
    const int sample = (int)s_signal_snapshot.waveform[i];
    const int magnitude = sample < 0 ? -sample : sample;
    if (magnitude > scale) {
      scale = magnitude;
    }
  }

  for (uint32_t i = 0; i < VOICE_CONTROL_WAVEFORM_SAMPLES; ++i) {
    const int x = plot_x + (int)((i * (uint32_t)(plot_width - 1)) /
                                  (VOICE_CONTROL_WAVEFORM_SAMPLES - 1u));
    const int offset = ((int)s_signal_snapshot.waveform[i] * (plot_height / 2)) / scale;
    const int y = clamp_int(mid_y - offset, plot_y, plot_y + plot_height - 1);
    td_putc(x, y, '*', theme->accent, theme->win_bg, TD_BOLD);
  }

  snprintf(line, sizeof(line), "Scale +/- %d PCM", scale);
  td_text(2, graph_y + graph_height - 1, line, theme->dim, theme->win_bg, 0);
}

static void on_signal_tick(td_window_t *window)
{
  voice_control_get_status(&s_signal_snapshot);
  td_win_invalidate(window);
}

static void on_signal_close(td_window_t *window)
{
  (void)window;
  s_signal_window = NULL;
}

static void led_update(void)
{
  td_widget_printf(s_led_state, "Status: %s", s_led_on ? "ON" : "OFF");
}

static void on_led_on(td_widget_t *widget, void *user)
{
  (void)widget;
  (void)user;
  led_turn_on(DETECTION_ON);
  s_led_on = true;
  led_update();
}

static void on_led_off(td_widget_t *widget, void *user)
{
  (void)widget;
  (void)user;
  led_turn_off(DETECTION_ON);
  s_led_on = false;
  led_update();
}

static void launch_status(void)
{
  if (td_win_is_open(s_status_window)) {
    td_win_focus(s_status_window);
    return;
  }
  td_window_desc_t desc = {
    .title = "MiniFace Board Console",
    .rect = td_rect(2, 1, 60, 11),
    .flags = TD_WIN_MOVABLE | TD_WIN_CLOSABLE,
    .on_close = on_status_close,
    .on_tick = on_status_tick,
    .tick_ms = 500,
  };
  s_status_window = td_win_create(&desc);
  if (s_status_window == NULL) {
    return;
  }
  td_label(s_status_window, 2, 1, 0, "MINIFACE  |  SiWG917 BRD2605A");
  s_status_uptime = td_label(s_status_window, 2, 3, 0, "");
  s_status_heap = td_label(s_status_window, 2, 4, 0, "");
  s_status_link = td_label(s_status_window, 2, 5, 0, "");
  td_label(s_status_window, 2, 7, 0, "Open an app to activate its board service.");
  td_label(s_status_window, 2, 8, 0, "VCOM desktop ready. Network and audio stay off at boot.");
  status_update();
}

static void launch_aiml(void)
{
  if (td_win_is_open(s_aiml_window)) {
    td_win_focus(s_aiml_window);
    return;
  }
  (void)voice_control_start();
  voice_control_request_microphones();
  td_window_desc_t desc = {
    .title = "Audio Intelligence",
    .rect = td_rect(12, 5, 54, 9),
    .flags = TD_WIN_MOVABLE | TD_WIN_CLOSABLE,
    .on_close = on_aiml_close,
    .on_tick = on_aiml_tick,
    .tick_ms = 250,
  };
  s_aiml_window = td_win_create(&desc);
  if (s_aiml_window == NULL) {
    return;
  }
  s_aiml_state = td_label(s_aiml_window, 2, 1, 0, "");
  s_aiml_label = td_label(s_aiml_window, 2, 2, 0, "");
  s_aiml_counts = td_label(s_aiml_window, 2, 3, 0, "");
  aiml_update();
}

static void launch_led(void)
{
  if (td_win_is_open(s_led_window)) {
    td_win_focus(s_led_window);
    return;
  }
  td_window_desc_t desc = {
    .title = "LED Control",
    .rect = td_rect(20, 8, 34, 8),
    .flags = TD_WIN_MOVABLE | TD_WIN_CLOSABLE,
    .on_close = on_led_close,
  };
  s_led_window = td_win_create(&desc);
  if (s_led_window == NULL) {
    return;
  }
  s_led_state = td_label(s_led_window, 2, 1, 0, "");
  td_button(s_led_window, 2, 3, "Turn on", on_led_on, NULL);
  td_button(s_led_window, 15, 3, "Turn off", on_led_off, NULL);
  td_label(s_led_window, 2, 5, 0, "Voice commands can also change this LED.");
  led_update();
}

static void launch_signal_analyzer(void)
{
  if (td_win_is_open(s_signal_window)) {
    td_win_focus(s_signal_window);
    return;
  }
  (void)voice_control_start();
  voice_control_request_microphones();
  td_window_desc_t desc = {
    .title = "Signal Strength Analyzer",
    .rect = td_rect(-1, -1, 82, 21),
    .flags = TD_WIN_MOVABLE | TD_WIN_RESIZABLE | TD_WIN_CLOSABLE,
    .min_w = 44,
    .min_h = 12,
    .on_draw = draw_signal_scope,
    .on_close = on_signal_close,
    .on_tick = on_signal_tick,
    .tick_ms = 80,
  };
  s_signal_window = td_win_create(&desc);
  if (s_signal_window != NULL) {
    on_signal_tick(s_signal_window);
  }
}

static const td_app_t s_status_app = {"Board Status", launch_status, "[]"};
static const td_app_t s_aiml_app = {"AIML Monitor", launch_aiml, "AI"};
static const td_app_t s_led_app = {"LED Control", launch_led, "**"};
static const td_app_t s_signal_app = {"Signal Analyzer", launch_signal_analyzer, "~~"};

static void vcom_rx_task(void *argument)
{
  (void)argument;
  for (;;) {
    uint8_t byte;
    size_t received = 0u;
    sl_status_t status = sl_iostream_read(sl_iostream_vcom_handle, &byte, 1u, &received);
    if (status == SL_STATUS_OK && received == 1u) {
      rx_push(byte);
    } else {
      vTaskDelay(pdMS_TO_TICKS(1));
    }
  }
}

static void desktop_task(void *argument)
{
  (void)argument;
  static const char banner[] = "MiniFace: board console starting\r\n";
  (void)sl_iostream_write(sl_iostream_vcom_handle, banner, sizeof(banner) - 1u);
  td_set_sysinfo(&s_sysinfo);
  if (td_init(&s_hal) == 0) {
    static const char initialized[] = "MiniFace: workspace initialized\r\n";
    (void)sl_iostream_write(sl_iostream_vcom_handle, initialized, sizeof(initialized) - 1u);
    td_app_register(&s_status_app);
    td_app_register(&s_aiml_app);
    td_app_register(&s_led_app);
    td_app_register(&s_signal_app);
    td_counter_register();
    td_network_register();
    td_sysmon_register();
    td_about_register();
    launch_status();
    static const char running[] = "MiniFace: interactive desktop ready\r\n";
    (void)sl_iostream_write(sl_iostream_vcom_handle, running, sizeof(running) - 1u);
    td_run();
    td_shutdown();
  }
  vTaskDelete(NULL);
}

bool miniface_siwg917_start(void)
{
  if (s_started) {
    return true;
  }
  if (xTaskCreate(vcom_rx_task, "td_vcom_rx", 512u, NULL, 5u, NULL) != pdPASS) {
    return false;
  }
  if (!miniface_siwg917_network_start()) {
    return false;
  }
  s_sysinfo.net = miniface_siwg917_network_ops();
  if (xTaskCreate(desktop_task, "miniface", 1536u, NULL, 6u, NULL) != pdPASS) {
    return false;
  }
  s_started = true;
  return true;
}
