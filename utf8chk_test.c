
#define UTF8CHK_IMPL

#include <stdio.h>
#include <stdlib.h>

#include "utf8chk.h"

static const char *utf8chk_strerr(utf8chk_error_t err) {
    switch (err) {
    case UTF8CHK_OK:
        return "UTF8CHK_OK";
    case UTF8CHK_ERR_UNEXPECTED_CONT:
        return "UTF8CHK_ERR_UNEXPECTED_CONT";
    case UTF8CHK_ERR_INVALID_START_BYTE:
        return "UTF8CHK_ERR_INVALID_START_BYTE";
    case UTF8CHK_ERR_RANGE:
        return "UTF8CHK_ERR_RANGE";
    case UTF8CHK_ERR_OVERLONG:
        return "UTF8CHK_ERR_OVERLONG";
    case UTF8CHK_ERR_NONCHARACTER:
        return "UTF8CHK_ERR_NONCHARACTER";
    case UTF8CHK_ERR_SURROGATE:
        return "UTF8CHK_ERR_SURROGATE";
    case UTF8CHK_ERR_SURROGATE_LOW:
        return "UTF8CHK_ERR_SURROGATE_LOW";
    case UTF8CHK_ERR_SURROGATE_HIGH:
        return "UTF8CHK_ERR_SURROGATE_HIGH";
    case UTF8CHK_ERR_NULL_BYTE:
        return "UTF8CHK_ERR_NULL_BYTE";
    case UTF8CHK_ERR_EXPECTED_CONT:
        return "UTF8CHK_ERR_EXPECTED_CONT";
    case UTF8CHK_ERR_EXPECTED_CONT2:
        return "UTF8CHK_ERR_EXPECTED_CONT2";
    case UTF8CHK_ERR_EXPECTED_CONT3:
        return "UTF8CHK_ERR_EXPECTED_CONT3";
    case UTF8CHK_ERR_TRUNC:
        return "UTF8CHK_ERR_TRUNC";
    case UTF8CHK_ERR_TRUNC2:
        return "UTF8CHK_ERR_TRUNC2";
    case UTF8CHK_ERR_TRUNC3:
        return "UTF8CHK_ERR_TRUNC3";
    case UTF8CHK_ERR_SURROGATE_TRUNC:
        return "UTF8CHK_ERR_SURROGATE_TRUNC";
    default:
        return "<\?\?\?>";    
    }
}

static int test_case(const char *name, const char *string, size_t length,
              utf8chk_flag_t flags, utf8chk_error_t err,
              size_t expected_error_at_index, size_t expected_error_len) {
    const char *error_at;
    size_t error_len;
    utf8chk_error_t got = utf8chk(string, length, flags, &error_at, &error_len);
            
    printf("Test '%s'... ", name);
    fflush(stdout);
    if (got != err) {
        printf("FAIL (expected err=%s, got err=%s)\n", utf8chk_strerr(err), utf8chk_strerr(got));
        return 1;
    }
    if ((size_t)(error_at - string) != expected_error_at_index) {
        printf("FAIL (expected error_at=%zu, got error_at=%zu)\n", expected_error_at_index, (size_t)(error_at - string));
        return 1;
    }
    if (error_len != expected_error_len) {
        printf("FAIL (expected error_len=%zu, got error_len=%zu)\n", expected_error_len, error_len);
        return 1;
    }
    puts("OK");
    return 0;
}

#define TEST_CASE(name, string, length, flags, expected, error_at, error_len)  \
    if (test_case(name, string, length, flags, expected, error_at, error_len)) \
        ++fail;

static int run_tests(void) {
    unsigned fail = 0;
    TEST_CASE(
        "Empty string with implicit length",
        "",
        UTF8CHK_CSTRING, UTF8CHK_UTF8, UTF8CHK_OK, 0, 0
    );
    TEST_CASE(
        "Empty string with explicit length",
        "",
        0, UTF8CHK_UTF8, UTF8CHK_OK, 0, 0
    );
    TEST_CASE(
        "Valid ASCII string with implicit length",
        "foo",
        UTF8CHK_CSTRING, UTF8CHK_UTF8, UTF8CHK_OK, 3, 0
    );
    TEST_CASE(
        "Valid ASCII string with explicit length",
        "bar",
        3, UTF8CHK_UTF8, UTF8CHK_OK, 3, 0
    );
    TEST_CASE(
        "Valid ASCII string with explicit shorter length",
        "bar",
        2, UTF8CHK_UTF8, UTF8CHK_OK, 2, 0
    );
    TEST_CASE(
        "Valid UTF-8 string containing two-byte sequence with implicit length",
        "\xd2\x91",
        UTF8CHK_CSTRING, UTF8CHK_UTF8, UTF8CHK_OK, 2, 0
    );
    TEST_CASE(
        "Valid UTF-8 string containing two-byte sequence with explicit length",
        "\xd2\x91",
        2, UTF8CHK_UTF8, UTF8CHK_OK, 2, 0
    );
    TEST_CASE(
        "Valid UTF-8 string containing three-byte sequence with implicit length",
        "\xe3\x83\x84",
        UTF8CHK_CSTRING, UTF8CHK_UTF8, UTF8CHK_OK, 3, 0
    );
    TEST_CASE(
        "Valid UTF-8 string containing three-byte sequence with explicit length",
        "\xe3\x83\x84",
        3, UTF8CHK_UTF8, UTF8CHK_OK, 3, 0
    );
    TEST_CASE(
        "Valid UTF-8 string containing four-byte sequence with implicit length",
        "\xf0\x9f\x98\x83",
        UTF8CHK_CSTRING, UTF8CHK_UTF8, UTF8CHK_OK, 4, 0
    );
    TEST_CASE(
        "Valid UTF-8 string containing four-byte sequence with explicit length",
        "\xf0\x9f\x98\x83",
        4, UTF8CHK_UTF8, UTF8CHK_OK, 4, 0
    );
    TEST_CASE(
        "Valid UTF-8 string with implicit length",
        "\xe8\xa9\x9e\xe8\xaa\x9e",
        UTF8CHK_CSTRING, UTF8CHK_UTF8, UTF8CHK_OK, 6, 0
    );
    TEST_CASE(
        "Valid UTF-8 string with explicit length",
        "\xe8\xa9\x9e\xe8\xaa\x9e",
        6, UTF8CHK_UTF8, UTF8CHK_OK, 6, 0
    );
    TEST_CASE(
        "Valid UTF-8 string with explicit shorter length",
        "\xe8\xa9\x9e\xe8\xaa\x9e",
        3, UTF8CHK_UTF8, UTF8CHK_OK, 3, 0
    );
    TEST_CASE(
        "Valid UTF-8 string #2 with explicit length",
        "\xe8\xa9\x9e\xe8\xaa\x9e",
        6, UTF8CHK_UTF8, UTF8CHK_OK, 6, 0
    );
    TEST_CASE(
        "Valid UTF-8 string #2 with explicit shorter length",
        "\xe8\xa9\x9e\xe8\xaa\x9e",
        3, UTF8CHK_UTF8, UTF8CHK_OK, 3, 0
    );
    TEST_CASE(
        "Valid UTF-8 string #3 with implicit length",
        "\x48\x65\x6c\x6c\x6f\x20\x77\x6f\x72\x6c\x64\x2c\x20\xce\x9a\xce\xb1\xce\xbb\xce\xb7\xce\xbc\xe1\xbd\xb3\xcf\x81\xce\xb1\x20\xce\xba\xe1\xbd\xb9\xcf\x83\xce\xbc\xce\xb5\x2c\x20\xe3\x82\xb3\xe3\x83\xb3\xe3\x83\x8b\xe3\x83\x81\xe3\x83\x8f",
        UTF8CHK_CSTRING, UTF8CHK_UTF8, UTF8CHK_OK, 59, 0
    );
    TEST_CASE(
        "Valid UTF-8 string #3 with explicit length",
        "\x48\x65\x6c\x6c\x6f\x20\x77\x6f\x72\x6c\x64\x2c\x20\xce\x9a\xce\xb1\xce\xbb\xce\xb7\xce\xbc\xe1\xbd\xb3\xcf\x81\xce\xb1\x20\xce\xba\xe1\xbd\xb9\xcf\x83\xce\xbc\xce\xb5\x2c\x20\xe3\x82\xb3\xe3\x83\xb3\xe3\x83\x8b\xe3\x83\x81\xe3\x83\x8f",
        59, UTF8CHK_UTF8, UTF8CHK_OK, 59, 0
    );
    TEST_CASE(
        "Last valid UTF-8 one-byte sequence",
        "\x7f",
        1, UTF8CHK_UTF8, UTF8CHK_OK, 1, 0
    );
    TEST_CASE(
        "First valid UTF-8 two-byte sequence",
        "\xc2\x80",
        2, UTF8CHK_UTF8, UTF8CHK_OK, 2, 0
    );
    TEST_CASE(
        "Last valid UTF-8 two-byte sequence",
        "\xdf\xbf",
        2, UTF8CHK_UTF8, UTF8CHK_OK, 2, 0
    );
    TEST_CASE(
        "First valid UTF-8 three-byte sequence",
        "\xe0\xa0\x80",
        3, UTF8CHK_UTF8, UTF8CHK_OK, 3, 0
    );
    TEST_CASE(
        "Last valid UTF-8 three-byte sequence (U+FFFF)",
        "\xef\xbf\xbf",
        3, UTF8CHK_UTF8, UTF8CHK_OK, 3, 0
    );
    TEST_CASE(
        "First valid UTF-8 four-byte sequence",
        "\xf0\x90\x80\x80",
        4, UTF8CHK_UTF8, UTF8CHK_OK, 4, 0
    );
    TEST_CASE(
        "Last valid UTF-8 four-byte sequence",
        "\xf4\x8f\xbf\xbf",
        4, UTF8CHK_UTF8, UTF8CHK_OK, 4, 0
    );
    TEST_CASE(
        "U+FFFD",
        "\xef\xbf\xbd",
        3, UTF8CHK_UTF8, UTF8CHK_OK, 3, 0
    );
    TEST_CASE(
        "Code point out of range (U+110000)",
        "\xf4\x90\x80\x80",
        4, UTF8CHK_UTF8, UTF8CHK_ERR_RANGE, 0, 4
    );
    TEST_CASE(
        "Code point way out of range (U+1FFFFF)",
        "\xf7\xbf\xbf\xbf",
        4, UTF8CHK_UTF8, UTF8CHK_ERR_RANGE, 0, 4
    );
    TEST_CASE(
        "Unexpected continuation character #1",
        "a\x80",
        2, UTF8CHK_UTF8, UTF8CHK_ERR_UNEXPECTED_CONT, 1, 1
    );
    TEST_CASE(
        "Unexpected continuation character #2",
        "\xbf",
        1, UTF8CHK_UTF8, UTF8CHK_ERR_UNEXPECTED_CONT, 0, 1
    );
    TEST_CASE(
        "Two-byte sequence cut short with explicit length",
        "\xc2",
        1, UTF8CHK_UTF8, UTF8CHK_ERR_TRUNC, 0, 1
    );
    TEST_CASE(
        "Three-byte sequence cut short after 1 byte with explicit length",
        "\xe0",
        1, UTF8CHK_UTF8, UTF8CHK_ERR_TRUNC2, 0, 1
    );
    TEST_CASE(
        "Three-byte sequence cut short after 2 bytes with explicit length",
        "\xe0\xa0",
        2, UTF8CHK_UTF8, UTF8CHK_ERR_TRUNC, 0, 2
    );
    TEST_CASE(
        "Four-byte sequence cut short after 1 byte with explicit length",
        "\xf0",
        1, UTF8CHK_UTF8, UTF8CHK_ERR_TRUNC3, 0, 1
    );
    TEST_CASE(
        "Four-byte sequence cut short after 2 bytes with explicit length",
        "\xf0\x90",
        2, UTF8CHK_UTF8, UTF8CHK_ERR_TRUNC2, 0, 2
    );
    TEST_CASE(
        "Four-byte sequence cut short after 3 bytes with explicit length",
        "\xf0\x90\x80",
        3, UTF8CHK_UTF8, UTF8CHK_ERR_TRUNC, 0, 3
    );
    TEST_CASE(
        "Two-byte sequence cut short with implicit length",
        "\xc2",
        UTF8CHK_CSTRING, UTF8CHK_UTF8, UTF8CHK_ERR_TRUNC, 0, 1
    );
    TEST_CASE(
        "Three-byte sequence cut short after 1 byte with implicit length",
        "\xe0",
        UTF8CHK_CSTRING, UTF8CHK_UTF8, UTF8CHK_ERR_TRUNC2, 0, 1
    );
    TEST_CASE(
        "Three-byte sequence cut short after 2 bytes with implicit length",
        "\xe0\xa0",
        UTF8CHK_CSTRING, UTF8CHK_UTF8, UTF8CHK_ERR_TRUNC, 0, 2
    );
    TEST_CASE(
        "Four-byte sequence cut short after 1 byte with implicit length",
        "\xf0",
        UTF8CHK_CSTRING, UTF8CHK_UTF8, UTF8CHK_ERR_TRUNC3, 0, 1
    );
    TEST_CASE(
        "Four-byte sequence cut short after 2 bytes with implicit length",
        "\xf0\x90",
        UTF8CHK_CSTRING, UTF8CHK_UTF8, UTF8CHK_ERR_TRUNC2, 0, 2
    );
    TEST_CASE(
        "Four-byte sequence cut short after 3 bytes with implicit length",
        "\xf0\x90\x80",
        UTF8CHK_CSTRING, UTF8CHK_UTF8, UTF8CHK_ERR_TRUNC, 0, 3
    );
    TEST_CASE(
        "Two-byte sequence cut short by another character",
        "\xc2\x62",
        2, UTF8CHK_UTF8, UTF8CHK_ERR_EXPECTED_CONT, 0, 1
    );
    TEST_CASE(
        "Three-byte sequence cut short by another character after 1 byte",
        "\xe0\x62\x62",
        3, UTF8CHK_UTF8, UTF8CHK_ERR_EXPECTED_CONT2, 0, 1
    );
    TEST_CASE(
        "Three-byte sequence cut short by another character after 2 bytes",
        "\xe0\xa0\x62",
        3, UTF8CHK_UTF8, UTF8CHK_ERR_EXPECTED_CONT, 0, 2
    );
    TEST_CASE(
        "Four-byte sequence cut short by another character after 1 byte",
        "\xf0\x62\x62\x62",
        4, UTF8CHK_UTF8, UTF8CHK_ERR_EXPECTED_CONT3, 0, 1
    );
    TEST_CASE(
        "Four-byte sequence cut short by another character after 2 bytes",
        "\xf0\x90\x62\x62",
        4, UTF8CHK_UTF8, UTF8CHK_ERR_EXPECTED_CONT2, 0, 2
    );
    TEST_CASE(
        "Four-byte sequence cut short by another character after 3 bytes",
        "\xf0\x90\x80\x62",
        4, UTF8CHK_UTF8, UTF8CHK_ERR_EXPECTED_CONT, 0, 3
    );
    TEST_CASE(
        "Invalid start byte #1",
        "\xf8",
        1, UTF8CHK_UTF8, UTF8CHK_ERR_INVALID_START_BYTE, 0, 1
    );
    TEST_CASE(
        "Invalid start byte #2",
        "\xff",
        1, UTF8CHK_UTF8, UTF8CHK_ERR_INVALID_START_BYTE, 0, 1
    );
    TEST_CASE(
        "Noncharacter #1 when allowed",
        "\xef\xbf\xbe",
        3, UTF8CHK_UTF8, UTF8CHK_OK, 3, 0
    );
    TEST_CASE(
        "Noncharacter #2 when allowed",
        "\xef\xb7\x90",
        3, UTF8CHK_UTF8, UTF8CHK_OK, 3, 0
    );
    TEST_CASE(
        "Noncharacter #3 when allowed",
        "\xef\xb7\xaf",
        3, UTF8CHK_UTF8, UTF8CHK_OK, 3, 0
    );
    TEST_CASE(
        "Noncharacter #4 when allowed",
        "\xf3\xbf\xbf\xbe",
        4, UTF8CHK_UTF8, UTF8CHK_OK, 4, 0
    );
    TEST_CASE(
        "Noncharacter #1 when banned",
        "\xef\xbf\xbe",
        3, UTF8CHK_UTF8 | UTF8CHK_BAN_NONCHARACTERS, UTF8CHK_ERR_NONCHARACTER, 0, 3
    );
    TEST_CASE(
        "Noncharacter #2 when banned",
        "\xef\xb7\x90",
        3, UTF8CHK_UTF8 | UTF8CHK_BAN_NONCHARACTERS, UTF8CHK_ERR_NONCHARACTER, 0, 3
    );
    TEST_CASE(
        "Noncharacter #3 when banned",
        "\xef\xb7\xaf",
        3, UTF8CHK_UTF8 | UTF8CHK_BAN_NONCHARACTERS, UTF8CHK_ERR_NONCHARACTER, 0, 3
    );
    TEST_CASE(
        "Noncharacter #4 when banned",
        "\xf3\xbf\xbf\xbe",
        4, UTF8CHK_UTF8 | UTF8CHK_BAN_NONCHARACTERS, UTF8CHK_ERR_NONCHARACTER, 0, 4
    );
    TEST_CASE(
        "Null byte banned with implicit length",
        "b\x00",
        UTF8CHK_CSTRING, UTF8CHK_UTF8 | UTF8CHK_BAN_NULL_BYTE, UTF8CHK_OK, 1, 0
    );
    TEST_CASE(
        "Null byte banned with explicit length",
        "a\x00",
        2, UTF8CHK_UTF8 | UTF8CHK_BAN_NULL_BYTE, UTF8CHK_ERR_NULL_BYTE, 1, 1
    );
    TEST_CASE(
        "Minimum overlong two-byte sequence",
        "\xc0\x80",
        2, UTF8CHK_UTF8, UTF8CHK_ERR_OVERLONG, 0, 2
    );
    TEST_CASE(
        "Maximum overlong two-byte sequence",
        "\xc1\xbf",
        2, UTF8CHK_UTF8, UTF8CHK_ERR_OVERLONG, 0, 2
    );
    TEST_CASE(
        "Minimum overlong three-byte sequence",
        "\xe0\x80\x80",
        3, UTF8CHK_UTF8, UTF8CHK_ERR_OVERLONG, 0, 3
    );
    TEST_CASE(
        "Maximum overlong three-byte sequence",
        "\xe0\x9f\xbf",
        3, UTF8CHK_UTF8, UTF8CHK_ERR_OVERLONG, 0, 3
    );
    TEST_CASE(
        "Minimum overlong four-byte sequence",
        "\xf0\x80\x80\x80",
        4, UTF8CHK_UTF8, UTF8CHK_ERR_OVERLONG, 0, 4
    );
    TEST_CASE(
        "Maximum overlong four-byte sequence",
        "\xf0\x8f\xbf\xbf",
        4, UTF8CHK_UTF8, UTF8CHK_ERR_OVERLONG, 0, 4 
    );
    TEST_CASE(
        "When overlong not banned",
        "\xe0\x9f\xbf",
        3, UTF8CHK_UTF8 & ~UTF8CHK_BAN_OVERLONG, UTF8CHK_OK, 3, 0
    );
    TEST_CASE(
        "C0 80 allowed",
        "\xc0\x80",
        2, UTF8CHK_MUTF8, UTF8CHK_OK, 2, 0
    );
    TEST_CASE(
        "Minimum overlong two-byte sequence with C0 80 allowed",
        "\xc0\x81",
        2, UTF8CHK_MUTF8, UTF8CHK_ERR_OVERLONG, 0, 2
    );
    TEST_CASE(
        "Three-byte null not allowed if C0 80 allowed",
        "\xe0\x80\x80",
        3, UTF8CHK_MUTF8, UTF8CHK_ERR_OVERLONG, 0, 3
    );
    TEST_CASE(
        "Surrogates when banned",
        "\xed\xa0\x81\xed\xb0\x80",
        6, UTF8CHK_UTF8, UTF8CHK_ERR_SURROGATE, 0, 3
    );
    TEST_CASE(
        "Surrogates when allowed",
        "\xed\xa0\x81\xed\xb0\x80",
        6, UTF8CHK_CESU8, UTF8CHK_OK, 6, 0
    );
    TEST_CASE(
        "Surrogate truncated",
        "\xed\xa0\x81",
        3, UTF8CHK_CESU8, UTF8CHK_ERR_SURROGATE_TRUNC, 0, 3
    );
    TEST_CASE(
        "Low surrogate truncated by one byte",
        "\xed\xa0\x81\xed\xb0",
        5, UTF8CHK_CESU8, UTF8CHK_ERR_SURROGATE_TRUNC, 0, 3
    );
    TEST_CASE(
        "Low surrogate truncated by two bytes",
        "\xed\xa0\x81\xed",
        4, UTF8CHK_CESU8, UTF8CHK_ERR_SURROGATE_TRUNC2, 0, 3
    );
    TEST_CASE(
        "Surrogate low before high",
        "\xed\xb0\x80\xed\xa0\x81",
        6, UTF8CHK_CESU8, UTF8CHK_ERR_SURROGATE_LOW, 0, 3
    );
    TEST_CASE(
        "Surrogate high-high",
        "\xed\xa0\x81\xed\xa0\x81",
        6, UTF8CHK_CESU8, UTF8CHK_ERR_SURROGATE_HIGH, 3, 3
    );
    TEST_CASE(
        "Surrogate truncated without validation",
        "\xed\xa0\x81",
        3, UTF8CHK_WTF8, UTF8CHK_OK, 3, 0
    );
    TEST_CASE(
        "Surrogate low before high without validation",
        "\xed\xb0\x80\xed\xa0\x81",
        6, UTF8CHK_WTF8, UTF8CHK_OK, 6, 0
    );
    TEST_CASE(
        "Surrogate high-high without validation",
        "\xed\xa0\x81\xed\xa0\x81",
        6, UTF8CHK_WTF8, UTF8CHK_OK, 6, 0
    );
    if (fail)
        printf("%u tests failed.\n", fail);
    else
        puts("All tests OK.");
    return fail;
}

int main(int argc, char *argv[]) {
    (void)argc, (void)argv;
    return run_tests() ? EXIT_FAILURE : EXIT_SUCCESS;
}
