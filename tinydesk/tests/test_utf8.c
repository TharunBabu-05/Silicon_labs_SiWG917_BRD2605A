/*
 * test_utf8.c - UTF-8 encoder, string decoder and streaming decoder.
 */
#include "td_test.h"
#include "tinydesk/td_screen.h"

static void test_encode(void)
{
    uint8_t b[4];
    CHECK_EQ(td_utf8_encode('A', b), 1);
    CHECK_EQ(b[0], 'A');

    CHECK_EQ(td_utf8_encode(0xE9, b), 2);                 /* é */
    CHECK(b[0] == 0xC3 && b[1] == 0xA9);

    CHECK_EQ(td_utf8_encode(0x2500, b), 3);               /* ─ */
    CHECK(b[0] == 0xE2 && b[1] == 0x94 && b[2] == 0x80);

    CHECK_EQ(td_utf8_encode(0x1F600, b), 4);              /* emoji */
    CHECK(b[0] == 0xF0 && b[1] == 0x9F && b[2] == 0x98 && b[3] == 0x80);

    CHECK_EQ(td_utf8_encode(0xD800, b), 3);               /* surrogate -> U+FFFD */
    CHECK(b[0] == 0xEF && b[1] == 0xBF && b[2] == 0xBD);
    CHECK_EQ(td_utf8_encode(0x110000, b), 3);
}

static void test_decode_string(void)
{
    const char *s = "a\xC3\xA9\xE2\x94\x80\xF0\x9F\x98\x80";
    CHECK_EQ(td_utf8_next(&s), 'a');
    CHECK_EQ(td_utf8_next(&s), 0xE9);
    CHECK_EQ(td_utf8_next(&s), 0x2500);
    CHECK_EQ(td_utf8_next(&s), 0x1F600);
    CHECK_EQ(td_utf8_next(&s), 0);

    /* Malformed: stray continuation, overlong, truncated. */
    const char *bad = "\x80"
                      "\xC0\x80"
                      "\xE2\x94"
                      "z";
    CHECK_EQ(td_utf8_next(&bad), 0xFFFD);
    CHECK_EQ(td_utf8_next(&bad), 0xFFFD);   /* C0 is never valid */
    CHECK_EQ(td_utf8_next(&bad), 0xFFFD);   /* its 80 */
    CHECK_EQ(td_utf8_next(&bad), 0xFFFD);   /* E2 94 cut short by 'z' */
    CHECK_EQ(td_utf8_next(&bad), 0xFFFD);   /* the lone 94 */
    CHECK_EQ(td_utf8_next(&bad), 'z');

    CHECK_EQ(td_utf8_len("h\xC3\xA9llo"), 5);
}

static void test_stream(void)
{
    td_utf8_decoder_t d = {0};
    uint32_t out[2];
    const uint8_t seq[] = {0xE2, 0x94, 0x80};
    CHECK_EQ(td_utf8_feed(&d, seq[0], out), 0);
    CHECK_EQ(td_utf8_feed(&d, seq[1], out), 0);
    CHECK_EQ(td_utf8_feed(&d, seq[2], out), 1);
    CHECK_EQ(out[0], 0x2500);

    CHECK_EQ(td_utf8_feed(&d, 'x', out), 1);
    CHECK_EQ(out[0], 'x');

    /* A sequence interrupted by ASCII yields U+FFFD then the character. */
    CHECK_EQ(td_utf8_feed(&d, 0xC3, out), 0);
    CHECK_EQ(td_utf8_feed(&d, 'y', out), 2);
    CHECK_EQ(out[0], 0xFFFD);
    CHECK_EQ(out[1], 'y');

    /* Overlong three-byte form of '/' is rejected. */
    CHECK_EQ(td_utf8_feed(&d, 0xE0, out), 0);
    CHECK_EQ(td_utf8_feed(&d, 0x80, out), 0);
    CHECK_EQ(td_utf8_feed(&d, 0xAF, out), 1);
    CHECK_EQ(out[0], 0xFFFD);
}

/* td_utf8_pad(): `cols` cells as the screen counts them. */
static void test_pad(void)
{
    char out[64];

    /* ASCII: padded, and cut at `cols`. */
    CHECK_EQ(td_utf8_pad(out, sizeof(out), "abc", 6), 6);
    CHECK(strcmp(out, "abc   ") == 0);
    CHECK_EQ(td_utf8_pad(out, sizeof(out), "abcdefgh", 5), 5);
    CHECK(strcmp(out, "abcde") == 0);
    CHECK_EQ(td_utf8_pad(out, sizeof(out), NULL, 3), 3);
    CHECK(strcmp(out, "   ") == 0);
    CHECK_EQ(td_utf8_pad(out, sizeof(out), "abc", 0), 0);
    CHECK(out[0] == '\0');

    /* 2-, 3- and 4-byte characters are one cell each. */
    CHECK_EQ(td_utf8_pad(out, sizeof(out), "caf\xC3\xA9", 6), 7); /* café + 2 spaces */
    CHECK(strcmp(out, "caf\xC3\xA9  ") == 0);
    CHECK_EQ(td_utf8_len(out), 6);
    CHECK_EQ(td_utf8_pad(out, sizeof(out), "\xE4\xB8\xAD\xE6\x96\x87", 4), 8); /* 中文 */
    CHECK(strcmp(out, "\xE4\xB8\xAD\xE6\x96\x87  ") == 0);
    CHECK_EQ(td_utf8_pad(out, sizeof(out), "a\xF0\x9F\x98\x80"
                                           "b",
                         3),
             6); /* a😀b */
    CHECK(strcmp(out, "a\xF0\x9F\x98\x80"
                      "b") == 0);

    /* Truncation stops at a character boundary. */
    CHECK_EQ(td_utf8_pad(out, sizeof(out), "\xE4\xB8\xAD\xE6\x96\x87xyz", 1), 3);
    CHECK(strcmp(out, "\xE4\xB8\xAD") == 0);
    CHECK_EQ(td_utf8_pad(out, sizeof(out), "ab\xE2\x98\x95", 2), 2);
    CHECK(strcmp(out, "ab") == 0);

    /* Invalid bytes are one cell each, as td_utf8_next() reads them, and
     * are copied as they are: the copy draws the same cells. */
    static const char bad[] = "\xFF"
                              "a\xC3"
                              "b\xE4\xB8"; /* stray, cut-short 2- and 3-byte */
    int n = td_utf8_pad(out, sizeof(out), bad, 8);
    CHECK_EQ(n, (int)strlen(bad) + 2);
    CHECK(memcmp(out, bad, strlen(bad)) == 0);
    CHECK_EQ(td_utf8_len(bad), 6);
    CHECK_EQ(td_utf8_len(out), 8);
    CHECK_EQ(td_utf8_pad(out, sizeof(out), bad, 3), 3);
    CHECK(memcmp(out, "\xFF"
                      "a\xC3",
                 3) == 0 &&
          out[3] == '\0');
    CHECK_EQ(td_utf8_len(out), 3);

    /* A buffer too small: whole characters only, always NUL-terminated. */
    CHECK_EQ(td_utf8_pad(out, 4, "\xE4\xB8\xAD\xE6\x96\x87", 4), 3);
    CHECK(strcmp(out, "\xE4\xB8\xAD") == 0);
    CHECK_EQ(td_utf8_pad(out, 3, "\xE4\xB8\xAD", 4), 2); /* no room for 中: spaces */
    CHECK(strcmp(out, "  ") == 0);
    CHECK_EQ(td_utf8_pad(out, 1, "abc", 4), 0);
    CHECK(out[0] == '\0');
    out[0] = 'x';
    CHECK_EQ(td_utf8_pad(out, 0, "abc", 4), 0);
    CHECK(out[0] == 'x'); /* cap 0: nothing written */
}

/* td_utf8_copy() and td_utf8_skip(): cut and skip by code points. */
static void test_copy_and_skip(void)
{
    char out[16];
    /* At most `cols` characters, no padding. */
    CHECK_EQ(td_utf8_copy(out, sizeof(out), "caf\xC3\xA9 au lait", 4), 5);
    CHECK(strcmp(out, "caf\xC3\xA9") == 0);
    CHECK_EQ(td_utf8_copy(out, sizeof(out), "ab", 10), 2);
    CHECK(strcmp(out, "ab") == 0);
    CHECK_EQ(td_utf8_copy(out, sizeof(out), NULL, 3), 0);
    CHECK(out[0] == '\0');
    /* Cut to fit the buffer, never inside a character: 5 x 3 bytes in 8. */
    CHECK_EQ(td_utf8_copy(out, 8, "\xE4\xB8\xAD\xE6\x96\x87\xE4\xB8\xAD\xE6\x96\x87\xE4\xB8\xAD", 1000), 6);
    CHECK(strcmp(out, "\xE4\xB8\xAD\xE6\x96\x87") == 0);

    /* Skipping: by characters; past the end gives the end. */
    const char *s = "\xC3\xA9t\xC3\xA9 x";
    CHECK(td_utf8_skip(s, 0) == s);
    CHECK(td_utf8_skip(s, 1) == s + 2);
    CHECK(td_utf8_skip(s, 3) == s + 5);
    CHECK(*td_utf8_skip(s, 99) == '\0');
    CHECK(td_utf8_skip(NULL, 2) == NULL);
}

int main(void)
{
    test_encode();
    test_decode_string();
    test_stream();
    test_pad();
    test_copy_and_skip();
    return TD_TEST_RESULT();
}
