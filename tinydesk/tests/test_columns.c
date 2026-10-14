/*
 * test_columns.c - list columns line up when names have non-ASCII
 * characters: the Network window's signal bars and "secured" column for
 * SSIDs with multibyte UTF-8 and with bytes that are not UTF-8 at all.
 */
#include "td_test.h"
#include "td_apps.h"

static uint32_t s_now = 10000;
static int fake_read(void *ctx)
{
    (void)ctx;
    return -1;
}
static int fake_write(void *ctx, const uint8_t *b, int n)
{
    (void)ctx;
    (void)b;
    return n;
}
static uint32_t fake_millis(void *ctx)
{
    (void)ctx;
    return s_now;
}
static void fake_sleep(void *ctx, uint32_t ms)
{
    (void)ctx;
    s_now += ms;
}
static const td_hal_t s_hal = {fake_read, fake_write, fake_millis, fake_sleep, NULL};

/* ------------------------------------------------------- fake Wi-Fi */

static const td_wifi_ap_t s_found[] = {
    {"plain", -50, true, false},
    {"caf\xC3\xA9 \xE2\x98\x95 \xE4\xB8\xAD\xE6\x96\x87", -50, true, false}, /* café ☕ 中文 */
    {"\xF0\x9F\x93\xB6\xF0\x9F\x93\xB6\xF0\x9F\x93\xB6\xF0\x9F\x93\xB6\xF0\x9F\x93\xB6"
     "\xF0\x9F\x93\xB6\xF0\x9F\x93\xB6\xF0\x9F\x93\xB6",
     -65, false, false},                                  /* 8 x 4-byte emoji = 32 bytes */
    {"\xFF\xFEnot-utf8\xC3", -80, true, true},            /* arbitrary bytes */
    {"a-very-long-network-name-of-32-b", -50, false, false}, /* exactly 32 bytes */
};
#define FOUND (int)(sizeof(s_found) / sizeof(s_found[0]))

static void net_status(td_net_status_t *out)
{
    memset(out, 0, sizeof(*out));
}
static bool net_scan(void)
{
    return true;
}
static int net_results(td_wifi_ap_t *out, int max)
{
    int n = FOUND < max ? FOUND : max;
    memcpy(out, s_found, sizeof(s_found[0]) * (size_t)n);
    return n;
}
static const td_net_ops_t s_net = {.status = net_status, .scan = net_scan, .scan_results = net_results};
static const td_sysinfo_t s_info = {.net = &s_net};

/* ----------------------------------------------------------- screen */

static td_buffer_t s_back;

/* Column of the first cell on row y, from x, that is `cp`; -1 if none. */
static int find(int y, int x, int w, uint32_t cp)
{
    for (int i = x; i < x + w; i++)
        if (td_buffer_cell(&s_back, i, y)->ch == cp)
            return i;
    return -1;
}

static void test_network_columns(void)
{
    td_network_register();
    CHECK(td_app_launch("Network"));
    td_window_t *win = td_win_focused();
    CHECK(win != NULL);
    for (int i = 0; i < 3; i++) /* the window's tick picks up the scan */
    {
        s_now += 1000;
        td_timers_run(s_now);
    }

    td_wm_compose(&s_back);
    td_rect_t c = td_win_client(win);
    int bars_col = -1, rows = 0;
    for (int y = c.y; y < c.y + c.h; y++)
    {
        int b = find(y, c.x, c.w, 0x2588); /* the first signal bar */
        if (b < 0)
            continue;
        rows++;
        if (bars_col < 0)
            bars_col = b;
        CHECK_EQ(b, bars_col);
        /* "secured" / "open" four cells after the first bar (bars are 3 wide). */
        uint32_t word = td_buffer_cell(&s_back, bars_col + 4, y)->ch;
        CHECK(word == 's' || word == 'o');
    }
    CHECK_EQ(rows, FOUND);
    /* The SSID column is 32 cells, one space after it: bars at client + 1 + 33. */
    CHECK_EQ(bars_col, c.x + 1 + 33);
}

int main(void)
{
    td_set_sysinfo(&s_info);
    td_init(&s_hal); /* no size answer: 80x25 */
    test_network_columns();
    return TD_TEST_RESULT();
}
