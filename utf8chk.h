/*******************************************************************************
*                                                                              *
*   UTF8CHK -- PORTABLE, SINGLE-HEADER, UTF-8 VALIDATION LIBRARY FOR C         *
*   DEVELOPED BY SAMPO HIPPELAINEN (HISAHI)                                    *
*   VERSION   2025-04-25                                                       *
*                                                                              *
*   THIS LIBRARY IS DUAL-LICENSED UNDER THE UNLICENSE PUBLIC-DOMAIN            *
*   EQUIVALENT LICENSE AND THE MIT LICENSE.                                    *
*                                                                              *
*******************************************************************************/

/* To use this library:
    1. define UTF8CHK_STATIC before every #include "utf8chk.h" to define all 
       needed functions statically, or
    2. include this in any number of compilation units, but define UTF8CHK_IMPL 
       before #include "utf8chk.h" in exactly one of them. */

#ifndef UTF8CHK_H
#define UTF8CHK_H

#include <limits.h>
#include <stddef.h>

#if CHAR_BIT != 8
#error CHAR_BIT must be 8 for utf8chk
#endif

typedef enum utf8chk_error {
    /* Continuation byte found when not expected.
       The error pointer points to the continuation byte.
       The error length will be set to 1. */
    UTF8CHK_ERR_UNEXPECTED_CONT = 1,

    /* An invalid multibyte sequence start byte (F8-FF).
       The error pointer points to the invalid byte.
       The error length will be set to 1. */
    UTF8CHK_ERR_INVALID_START_BYTE = 2,

    /* A sequence encoding a code point that is out of the Unicode range
       (U+0000 - U+10FFFF).
       The error pointer points to the start of the byte sequence
       that represents an out-of-range code point.
       The error length will be set to the length of the byte sequence. */
    UTF8CHK_ERR_RANGE = 3,

    /* Found an overlong representation.
       The error pointer points to the start of the byte sequence
       that is an overlong representation.
       The error length will be set to the length of the byte sequence.
       Requires the validation flag UTF8CHK_BAN_OVERLONG
       or UTF8CHK_BAN_OVERLONG_EXCEPT_NULL. */
    UTF8CHK_ERR_OVERLONG = 8,

    /* Found a Unicode noncharacter.
       The error pointer points to the start of the byte sequence
       encoding a noncharacter.
       The error length will be set to the length of the byte sequence.
       Requires the validation flag UTF8CHK_BAN_NONCHARACTERS. */
    UTF8CHK_ERR_NONCHARACTER = 9,

    /* A null byte was found.
       The error pointer points to the null byte found.
       The error length will be set to 1.
       Requires the validation flag UTF8CHK_BAN_NULL_BYTE. */
    UTF8CHK_ERR_NULL_BYTE = 10,

    /* Found a UTF-16 surrogate (U+D800 - U+DFFF).
       The error pointer points to the start of the byte sequence
       encoding a surrogate.
       The error length will be set to the length of the
       surrogate byte sequence.
       Requires the validation flag UTF8CHK_BAN_SURROGATES. */
    UTF8CHK_ERR_SURROGATE = 12,

    /* Found a low surrogate (U+DC00 - U+DFFF) not immediately
       preceded by a high surrogate (U+D800 - U+DBFF).
       The error pointer points to the start of the byte sequence
       encoding a surrogate.
       The error length will be set to the length of the
       surrogate byte sequence.
       Requires the validation flag UTF8CHK_CHECK_SURROGATES. */
    UTF8CHK_ERR_SURROGATE_LOW = 13,

    /* Found a high surrogate (U+D800 - U+DBFF)
       preceded by another high surrogate (U+D800 - U+DBFF).
       The error pointer points to the start of the byte sequence
       encoding the latter surrogate.
       The error length will be set to the length of the
       surrogate byte sequence.
       Requires the validation flag UTF8CHK_CHECK_SURROGATES. */
    UTF8CHK_ERR_SURROGATE_HIGH = 14,

    /* Expected one more continuation byte, but found something else.
       The error pointer points to the first byte of the sequence.
       The error length will be set to how many bytes of the sequence
       were found. */
    UTF8CHK_ERR_EXPECTED_CONT = 16,

    /* Expected two more continuation bytes, but found something else.
       The error pointer points to the first byte of the sequence.
       The error length will be set to how many bytes of the sequence
       were found. */
    UTF8CHK_ERR_EXPECTED_CONT2 = 17,

    /* Expected three more continuation bytes, but found something else.
       The error pointer points to the first byte of the sequence.
       The error length will be set to how many bytes of the sequence
       were found. */
    UTF8CHK_ERR_EXPECTED_CONT3 = 18,

    /* Expected one more continuation byte, but the string is truncated.
       The error pointer points to the start of the sequence.
       The error length will be set to how many bytes of the sequence
       were found.
       To continue validation, more text must be buffered, and the validation
       must restart from the byte pointed at by the error pointer. */
    UTF8CHK_ERR_TRUNC = 32,

    /* Expected two more continuation bytes, but the string is truncated.
       The error pointer points to the start of the sequence.
       The error length will be set to how many bytes of the sequence
       were found.
       To continue validation, more text must be buffered, and the validation
       must restart from the byte pointed at by the error pointer. */
    UTF8CHK_ERR_TRUNC2 = 33,

    /* Expected three more continuation bytes, but the string is truncated.
       The error pointer points to the start of the sequence.
       The error length will be set to how many bytes of the sequence
       were found.
       To continue validation, more text must be buffered, and the validation
       must restart from the byte pointed at by the error pointer. */
    UTF8CHK_ERR_TRUNC3 = 34,

    /* Found a high surrogate, but no low surrogate follows, because
       the string is truncated, and at least one more byte is needed.
       The error pointer points to the high surrogate.
       The error length will be set to the length of that high
       surrogate sequence.
       To continue validation, more text must be buffered, and the validation
       must restart from the byte pointed at by the error pointer
       (the high surrogate).
       Requires the validation flag UTF8CHK_CHECK_SURROGATES. */
    UTF8CHK_ERR_SURROGATE_TRUNC = 48,

    /* Found a high surrogate, but no low surrogate follows, because
       the string is truncated, and at least two more bytes is needed.
       The error pointer points to the high surrogate.
       The error length will be set to the length of that high
       surrogate sequence.
       To continue validation, more text must be buffered, and the validation
       must restart from the byte pointed at by the error pointer
       (the high surrogate).
       Requires the validation flag UTF8CHK_CHECK_SURROGATES. */
    UTF8CHK_ERR_SURROGATE_TRUNC2 = 49,

    /* Found a high surrogate, but no low surrogate follows, because
       the string is truncated, and at least three more bytes is needed.
       The error pointer points to the high surrogate.
       The error length will be set to the length of that high
       surrogate sequence.
       To continue validation, more text must be buffered, and the validation
       must restart from the byte pointed at by the error pointer
       (the high surrogate).
       Requires the validation flag UTF8CHK_CHECK_SURROGATES. */
    UTF8CHK_ERR_SURROGATE_TRUNC3 = 50,

    /* No error.
       The error pointer is set to the end of the string, either
       due to length or due to a null terminator (depending on the
       input length specified). The error length will be set to 0. */
    UTF8CHK_OK = 0
} utf8chk_error_t;

typedef enum utf8chk_flag {
    /* Bans overlong representations with the
       error code UTF8CHK_ERR_OVERLONG. */
    UTF8CHK_BAN_OVERLONG = 1,

    /* Ban all surrogates with the error code UTF8CHK_ERR_SURROGATE. */
    UTF8CHK_BAN_SURROGATES = 2,

    /* Bans overlong representations except for C0 80 (2-byte for U+0000)
       with the error code UTF8CHK_ERR_OVERLONG.
       Overruled by UTF8CHK_BAN_OVERLONG. */
    UTF8CHK_BAN_OVERLONG_EXCEPT_NULL = 4,

    /* Allows surrogates, but checks that they are valid (high-low),
       as expected in UTF-16.
       Overruled by UTF8CHK_BAN_SURROGATES. */
    UTF8CHK_CHECK_SURROGATES = 8,

    /* Bans Unicode noncharacters. */
    UTF8CHK_BAN_NONCHARACTERS = 16,

    /* Bans the single-byte null terminator (zero byte).
       Only matters if length is not UTF8CHK_CSTRING = (size_t)(-1);
       if the input string is treated as null-terminated, this flag is
       effectively ignored. */
    UTF8CHK_BAN_NULL_BYTE = 32,

    /* Lax validation - enable no extra validation. */
    UTF8CHK_LAX = 0,

    /* Strict validation - enable all extra validation. */
    UTF8CHK_STRICT = 32767
} utf8chk_flag_t;

/* Standard UTF-8 validation
        All overlong representations banned.
        All surrogates banned.
        Noncharacters not banned.
        Null terminators not banned. */
static const utf8chk_flag_t UTF8CHK_UTF8 = (utf8chk_flag_t)(
        UTF8CHK_BAN_OVERLONG | UTF8CHK_BAN_SURROGATES);

/* MUTF-8 validation
        All overlong representations banned except for C0 80 for U+0000.
        Surrogates not banned, but validated.
        Noncharacters not banned.
        Null terminators not banned. */
static const utf8chk_flag_t UTF8CHK_MUTF8 = (utf8chk_flag_t)(
        UTF8CHK_BAN_OVERLONG_EXCEPT_NULL | UTF8CHK_CHECK_SURROGATES);

/* CESU-8 validation
        All overlong representations banned.
        Surrogates not banned, but validated.
        Noncharacters not banned.
        Null terminators not banned. */
static const utf8chk_flag_t UTF8CHK_CESU8 = (utf8chk_flag_t)(
        UTF8CHK_BAN_OVERLONG | UTF8CHK_CHECK_SURROGATES);

/* WTF-8 validation
        All overlong representations banned.
        Surrogates not banned nor validated.
        Noncharacters not banned.
        Null terminators not banned. */
static const utf8chk_flag_t UTF8CHK_WTF8 = (utf8chk_flag_t)(
        UTF8CHK_BAN_OVERLONG);

#define UTF8CHK_CSTRING ((size_t)(-1))

/** Validates that the string in a buffer is valid UTF-8.
    Returns UTF8CHK_OK = 0 if valid, otherwise returns one of the values
    of enum utf8chk_error (UTF*CHK_ERR_*).
    
    If a length is given, it is assumed that it is the length of the
    string. If string is null-terminated, pass UTF8CHK_CSTRING = (size_t)(-1)
    as the length.
    
    If error_at is not NULL, the error position will be stored
    in that pointer. Its value depends on the return value;
    see utf8chk_error.
    
    If error_len is not NULL, the error length will be stored
    in that pointer. Its value depends on the return value;
    see utf8chk_error.
    
    A conforming UTF-8 decoder implementation should use the appropriate flags
    and replace errors with U+FFFD instead of removing or ignoring error
    sequences. The error may be replaced by a single U+FFFD, or it may
    be replaced with as many U+FFFD code points as there were bytes in
    the error. The former is recommended by modern conventions. */
#ifndef UTF8CHK_STATIC
extern utf8chk_error_t utf8chk(const char *string, size_t length,
            utf8chk_flag_t flags, const char **error_at, size_t *error_len);
#endif

#if UINT_MAX / 10000 > 10000
typedef unsigned int utf8chk_uchar_t;
#else
typedef unsigned long utf8chk_uchar_t;
#endif

#define UTF8CHK_UCHAR(u) (utf8chk_uchar_t)(u##UL)

#if defined(UTF8CHK_IMPL) || defined(UTF8CHK_STATIC)

#define UTF8CHK_SET_ERROR_AT_LEN(p, l) do {                                    \
                    if (error_at) *error_at = (const char *)(p);               \
                    if (error_len) *error_len = (size_t)(l);                   \
                } while (0)
#define UTF8CHK_RETURN_ERROR(err, p, l) do {                                   \
                    UTF8CHK_SET_ERROR_AT_LEN(p, l); return err;                \
                } while (0)
#define UTF8CHK_RETURN_ERROR_N(err, sel)                                       \
                switch (sel) {                                                 \
                default:                                                       \
                case 1:                                                        \
                    return err;                                                \
                case 2:                                                        \
                    return err##2;                                             \
                case 3:                                                        \
                    return err##3;                                             \
                }

/** Validates that the string in a buffer is valid UTF-8.
    Returns UTF8CHK_OK = 0 if valid, otherwise returns one of the values
    of enum utf8chk_error (UTF*CHK_ERR_*).
    
    If a length is given, it is assumed that it is the length of the
    string. If string is null-terminated, pass UTF8CHK_CSTRING = (size_t)(-1)
    as the length.
    
    If error_at is not NULL, the error position will be stored
    in that pointer. Its value depends on the return value;
    see utf8chk_error.
    
    If error_len is not NULL, the error length will be stored
    in that pointer. Its value depends on the return value;
    see utf8chk_error.
    
    A conforming UTF-8 decoder implementation should use the appropriate flags
    and replace errors with U+FFFD instead of removing or ignoring error
    sequences. The error may be replaced by a single U+FFFD, or it may
    be replaced with as many U+FFFD code points as there were bytes in
    the error. The former is recommended by modern conventions. */
#ifdef UTF8CHK_STATIC
static
#endif
utf8chk_error_t utf8chk(const char *string, size_t length,
    utf8chk_flag_t flags, const char **error_at, size_t *error_len) {
    /* maximum codepoint allowed. */
    static const utf8chk_uchar_t UNICODE_MAX = UTF8CHK_UCHAR(0x10FFFF);

    /* pointer to read bytes from. */
    const unsigned char *p = (const unsigned char *)string;

    /* whether the string is treated as null-terminated. */
    int null_terminated = length == UTF8CHK_CSTRING;

    /* whether to allow low surrogates. set if and only if the
       preceding code point was a low surrogate. */
    int expect_low_surrogate = 0;

    /* cached codepoint from high surrogate. */
    utf8chk_uchar_t u_cache = 0;

    /* expected or read length of sequence. */
    unsigned n = 0;

    /* length of the last sequence. */
    unsigned n_prev = 0;

    while (length) {
        /* byte read. */
        unsigned char c = *p;
        /* decoded codepoint. */
        utf8chk_uchar_t u;
        /* minimum allowed codepoint (overlong detection). */
        utf8chk_uchar_t u_min;
        /* iteration variable used when reading sequences. */
        unsigned i;

        if (!c) {
            /* Terminate if string is null-terminated
               and null terminator found. */
            if (null_terminated) break;
            /* If nulls are banned, return an error. */
            if (flags & UTF8CHK_BAN_NULL_BYTE)
                UTF8CHK_RETURN_ERROR(UTF8CHK_ERR_NULL_BYTE, p, 1);
        }

        if (c < 0x80U) {
            /* one byte.    0xxxxxxx
               advance pointer and decrease length. */
            n = 1;
            u = c;
            /* single-byte code points need no further checks
               (they cannot be surrogates, noncharacters, overlong
                or truncated) */
            goto skip_checks;
        } else if (c < 0xC0U) {
            /* continuation byte when one was not expected. */
            UTF8CHK_RETURN_ERROR(UTF8CHK_ERR_UNEXPECTED_CONT, p, 1);
        } else if (c < 0xE0U) {
            /* two bytes.   110xxxxx */
            n = 2;
            u = c & 0x1F;
            u_min = UTF8CHK_UCHAR(0x0080);
        } else if (c < 0xF0U) {
            /* three bytes. 1110xxxx */
            n = 3;
            u = c & 0x0F;
            u_min = UTF8CHK_UCHAR(0x0800);
        } else if (c < 0xF8U) {
            /* four bytes.  11110xxx */
            n = 4;
            u = c & 0x07;
            u_min = UTF8CHK_UCHAR(0x10000);
        } else {
            /* invalid start byte (overlong or out of range). */
            UTF8CHK_RETURN_ERROR(UTF8CHK_ERR_INVALID_START_BYTE, p, 1);
        }

        if (length < n) {
            /* truncated. return the appropriate error code. */
            if (expect_low_surrogate) {
                UTF8CHK_SET_ERROR_AT_LEN(p - n_prev, n_prev);
                UTF8CHK_RETURN_ERROR_N(UTF8CHK_ERR_SURROGATE_TRUNC, 
                                       n - (unsigned)length);
            }
            UTF8CHK_SET_ERROR_AT_LEN(p, length);
            UTF8CHK_RETURN_ERROR_N(UTF8CHK_ERR_TRUNC, n - (unsigned)length);
        }

        for (i = 1; i < n; ++i) {
            c = p[i];
            /* continuation bytes: high two bits must be 10xxxxxx */
            if ((c & 0xC0U) != 0x80U) {
                UTF8CHK_SET_ERROR_AT_LEN(p, i);

                /* expected continuation byte, saw something else. */
                if (!c && null_terminated) {
                    /* treat as truncated. */
                    if (expect_low_surrogate) {
                        UTF8CHK_SET_ERROR_AT_LEN(p - n_prev, n_prev);
                        UTF8CHK_RETURN_ERROR_N(UTF8CHK_ERR_SURROGATE_TRUNC, 
                                               n - i);
                    }
                    UTF8CHK_RETURN_ERROR_N(UTF8CHK_ERR_TRUNC, n - i);
                }
                UTF8CHK_RETURN_ERROR_N(UTF8CHK_ERR_EXPECTED_CONT, n - i);
            }
            u = (u << 6) | (c & 0x3FU);
        }

        /* check code point range. */
        if (u > UNICODE_MAX)
            UTF8CHK_RETURN_ERROR(UTF8CHK_ERR_RANGE, p, n);

        /* check for overlong representations. */
        if ((flags & (UTF8CHK_BAN_OVERLONG
                    | UTF8CHK_BAN_OVERLONG_EXCEPT_NULL)) && u < u_min) {
            /* possibly allow C0 80. */
            if ((flags & UTF8CHK_BAN_OVERLONG) || u || length != 2)
                UTF8CHK_RETURN_ERROR(UTF8CHK_ERR_OVERLONG, p, n);
        }

        /* check for surrogates. */
        if (UTF8CHK_UCHAR(0xD800) <= u && u <= UTF8CHK_UCHAR(0xDFFF)) {
            /* U+DC00 - U+DFFF are low surrogates. */
            int is_low = (int)(u & UTF8CHK_UCHAR(0x400));

            /* if all surrogates are banned, report error. */
            if (flags & UTF8CHK_BAN_SURROGATES)
                UTF8CHK_RETURN_ERROR(UTF8CHK_ERR_SURROGATE, p, n);

            if (flags & UTF8CHK_CHECK_SURROGATES) {
                /* check that the surrogate is low/high as specified. */
                if (is_low && !expect_low_surrogate)
                    UTF8CHK_RETURN_ERROR(UTF8CHK_ERR_SURROGATE_LOW, p, n);
                else if (!is_low && expect_low_surrogate)
                    UTF8CHK_RETURN_ERROR(UTF8CHK_ERR_SURROGATE_HIGH, p, n);

                /* next surrogate may be low only if this one is high. */
                expect_low_surrogate = !is_low;

                if (!is_low) {
                    /* cache the codepoint from the high surrogate.
                       U+D800 -> U+10000 + (low) 
                       U+D801 -> U+10400 + (low) 
                       ...
                       U+DBFE -> U+10F800 + (low) 
                       U+DBFF -> U+10FC00 + (low) */
                    u_cache = UTF8CHK_UCHAR(0x10000) + (
                            (u & UTF8CHK_UCHAR(0x3FF)) << 10U);
                    /* no code point 'to output' here - continue. */
                    goto no_output;
                }
                /* set up u for noncharacter check. */
                u = u_cache | (u & UTF8CHK_UCHAR(0x3FF));
            }
        } else {
            expect_low_surrogate = 0;
        }

        if (flags & UTF8CHK_BAN_NONCHARACTERS) {
            /* check for Unicode noncharacters.
               any nFFFE and nFFFF is a noncharacter. */
            if ((u & UTF8CHK_UCHAR(0xFFFE)) == UTF8CHK_UCHAR(0xFFFE))
                UTF8CHK_RETURN_ERROR(UTF8CHK_ERR_NONCHARACTER, p, n);

            /* U+FDD0 - U+FDEF are also noncharacters. */
            if (UTF8CHK_UCHAR(0xFDD0) <= u && u <= UTF8CHK_UCHAR(0xFDEF))
                UTF8CHK_RETURN_ERROR(UTF8CHK_ERR_NONCHARACTER, p, n);
        }

skip_checks:
        /* should you wish to decode the string, u is the code point
           decoded once code reaches this point. */
        ;

no_output:
        /* advance pointer and decrease length. */
        p += n, length -= n;
        n_prev = n;
    }

    /* end of string and no low surrogate found.
       shift back to the high surrogate. */
    if ((flags & UTF8CHK_CHECK_SURROGATES) && expect_low_surrogate)
        UTF8CHK_RETURN_ERROR(UTF8CHK_ERR_SURROGATE_TRUNC, p - n, n);

    UTF8CHK_RETURN_ERROR(UTF8CHK_OK, p, 0);
}

#endif /* UTF8CHK_IMPL */

#endif /* UTF8CHK_H */
