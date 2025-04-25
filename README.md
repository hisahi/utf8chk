
# utf8chk

utf8chk is a platform-independent single-header C library for
validating UTF-8 strings. It is compatible with ANSI C and above
for platforms with 8-bit bytes (`CHAR_BIT == 8`).

The library can validate UTF-8 strings as well as do full error reporting,
with support for reporting the type, position and length of the error.

`utf8chk.h` does not require any hosted headers to function.

## API

To use utf8chk, include `utf8chk.h` from as many files as needed,
and define `UTF8CHK_IMPL` in exactly one of them before including
the header. The implementation function will be placed in that file.
Alternatively, define `UTF8CHK_STATIC` before you include the header
in every file you use `utf8chk` in.

The main function is simply called `utf8chk`:

```c
utf8chk_error_t utf8chk(const char *string, size_t length,
            utf8chk_flag_t flags, const char **error_at, size_t *error_len);
```

* `string` is the pointer to the string to validate.
* `length` is the number of bytes (`char`) in `string`. If you want to
  have `string` treated as a null-terminated C string, pass `UTF8CHK_CSTRING`
  (= `(size_t)(-1)`) as the `length`.
* `flags` is a set of flags (see Flags below).
* `error_at` is where the error pointer (see Errors below) is placed.
  If it is `NULL`, the error pointer is not stored anywhere.
* `error_len` is where the error length (see Errors below) is placed.
  If it is `NULL`, the error length is not stored anywhere.

The return value is the first error found in the string, with `*error_at`
and `*error_len` set appropriately if the corresponding pointer is not `NULL`.
If there were no errors, the return value is `UTF8CHK_OK` (= 0).

## Flags

The supported flags are as follows:
* `UTF8CHK_BAN_OVERLONG`: Overlong representations are banned and result
  in the error `UTF8CHK_ERR_OVERLONG`.
* `UTF8CHK_BAN_OVERLONG_EXCEPT_NULL`: Overlong representations except for
  `C0 80` for the null terminator are banned and result in
  the error `UTF8CHK_ERR_OVERLONG`.
  `UTF8CHK_BAN_OVERLONG` takes priority over this flag if both are specified.
* `UTF8CHK_BAN_SURROGATES`: Surrogates are banned and result
  in the error `UTF8CHK_ERR_SURROGATE`.
* `UTF8CHK_CHECK_SURROGATES`: Surrogates are validated.
  `UTF8CHK_BAN_SURROGATES` takes priority over this flag if both are specified.
* `UTF8CHK_BAN_NONCHARACTERS`: Unicode 'noncharacters' are banned.
* `UTF8CHK_BAN_NULL_BYTE`: Null terminators as single bytes (`00`) are banned.
  This flag is ignored if `length` is `UTF8CHK_CSTRING`.

Flags can be combined with `|` or `+`.

The builtin supported flag combinations are as follows:
* `UTF8CHK_LAX`: lax validation - no flags enabled.
* `UTF8CHK_STRICT`: strict validation - all flags enabled.
* `UTF8CHK_UTF8`: standard UTF-8. Overlong representations and surrogates are
  banned, but null terminators and noncharacters are permitted.
  Equivalent to `UTF8CHK_BAN_OVERLONG | UTF8CHK_BAN_SURROGATES`.
* `UTF8CHK_CESU8`: CESU-8. Overlong representations are banned, but surrogates,
  null terminators and noncharacters are permitted. Surrogates
  are validated.
  Equivalent to `UTF8CHK_BAN_OVERLONG | UTF8CHK_CHECK_SURROGATES`.
* `UTF8CHK_MUTF8`: Modified UTF-8. Overlong representations are banned except
  for `C0 80` for the null terminator, but surrogates, null terminators
  and noncharacters are permitted. Surrogates are validated.
  Equivalent to `UTF8CHK_BAN_OVERLONG_EXCEPT_NULL | UTF8CHK_CHECK_SURROGATES`.
* `UTF8CHK_WTF8`: WTF-8. Overlong representations are banned, but surrogates,
  null terminators and noncharacters are permitted. No surrogate validation
  is performed.
  Equivalent to `UTF8CHK_BAN_OVERLONG`.

## Errors

Each error code sets the error pointer (`error_at`) and length (`error_len`)
to something appropriate.

* `UTF8CHK_OK`: String is valid with the given flags.
  The error pointer is set to the end of the string,
  and the error length is set to 0.
  `UTF8CHK_OK` is equivalent to zero, and all other error codes are nonzero.
* `UTF8CHK_ERR_UNEXPECTED_CONT`: Unexpected continuation byte not as part
  of a sequence, e.g. at the start of the string. 
  The error pointer is set to point to the continuation byte,
  and the error length is set to 1.
* `UTF8CHK_ERR_INVALID_START_BYTE`: A sequence start byte that does not
  correspond to a valid UTF-8 sequence, e.g. one with more than four
  leading bits set. 
  The error pointer is set to point to the invalid start byte,
  and the error length is set to 1.
* `UTF8CHK_ERR_EXPECTED_CONT`: A code unit sequence is truncated, and
  a continuation byte was not found where one was expected.
  One more continuation byte was expected.
  The error pointer is set to point to the start of the code unit sequence that
  is truncated, and the error length is set to the length of that sequence.
* `UTF8CHK_ERR_EXPECTED_CONT2`: A code unit sequence is truncated, and
  a continuation byte was not found where one was expected.
  Two more continuation bytes were expected.
  The error pointer is set to point to the start of the code unit sequence that
  is truncated, and the error length is set to the length of that sequence.
* `UTF8CHK_ERR_EXPECTED_CONT3`: A code unit sequence is truncated, and
  a continuation byte was not found where one was expected.
  Three more continuation bytes were expected.
  The error pointer is set to point to the start of the code unit sequence that
  is truncated, and the error length is set to the length of that sequence.
* `UTF8CHK_ERR_TRUNC`: The string is truncated.
  At least one more byte is required.
  The error pointer is set to point to the start of the code unit sequence that
  is truncated, and the error length is set to the length of that sequence.
  If more data is provided, validation must continue
  from the start of this sequence.
* `UTF8CHK_ERR_TRUNC2`: The string is truncated.
  At least two more bytes are required.
  The error pointer is set to point to the start of the code unit sequence that
  is truncated, and the error length is set to the length of that sequence.
  If more data is provided, validation must continue
  from the start of this sequence.
* `UTF8CHK_ERR_TRUNC3`: The string is truncated.
  At least three more bytes are required.
  The error pointer is set to point to the start of the code unit sequence that
  is truncated, and the error length is set to the length of that sequence.
  If more data is provided, validation must continue
  from the start of this sequence.
* `UTF8CHK_ERR_RANGE`: A code point that is out of range, i.e. greater than
  U+10FFFF.
  The error pointer is set to point to the start of the code point
  byte sequence, and the error length is set to the length of that sequence.
* `UTF8CHK_ERR_OVERLONG`: A code unit sequence that is not the shortest
  representation of the corresponding code point.
  The error pointer is set to point to the start of the code unit sequence,
  and the error length is set to the length of that sequence.
  Requires either the flag `UTF8CHK_BAN_OVERLONG`
  or `UTF8CHK_BAN_OVERLONG_EXCEPT_NULL`.
* `UTF8CHK_ERR_NONCHARACTER`: A code unit sequence that represents
  a code point corresponding to a Unicode noncharacter.
  The error pointer is set to point to the start of the code unit sequence,
  and the error length is set to the length of that sequence.
  Requires the flag `UTF8CHK_BAN_NONCHARACTERS`.
* `UTF8CHK_ERR_NULL_BYTE`: A single null byte.
  The error pointer is set to point to the null byte,
  and the error length is set to 1.
  Requires the flag `UTF8CHK_BAN_NULL_BYTE`.
* `UTF8CHK_ERR_SURROGATE`: A code unit sequence that represents a surrogate.
  The error pointer is set to point to the start of the code unit sequence,
  and the error length is set to the length of that sequence.
  Requires the flag `UTF8CHK_BAN_SURROGATES`.
* `UTF8CHK_ERR_SURROGATE_LOW`: A code unit sequence that represents a
  low surrogate when not expected, i.e. when not immediately
  preceded by a high surrogate.
  The error pointer is set to point to the start of the code unit sequence,
  and the error length is set to the length of that sequence.
  Requires the flag `UTF8CHK_CHECK_SURROGATES`.
* `UTF8CHK_ERR_SURROGATE_HIGH`: A code unit sequence that represents a
  high surrogate when not expected, i.e. when immediately
  preceded by a high surrogate.
  The error pointer is set to point to the start of the code unit sequence,
  and the error length is set to the length of that sequence.
  Requires the flag `UTF8CHK_CHECK_SURROGATES`.
* `UTF8CHK_ERR_SURROGATE_TRUNC`: The string is truncated after a high
  surrogate. At least one more byte is required.
  The error pointer is set to point to the start of the code unit sequence
  of the high surrogate, and the error length is set to the length
  of that sequence.
  If more data is provided, validation must continue
  from the specified high surrogate.
  Requires the flag `UTF8CHK_CHECK_SURROGATES`.
* `UTF8CHK_ERR_SURROGATE_TRUNC2`: The string is truncated after a high
  surrogate. At least two more bytes are required.
  The error pointer is set to point to the start of the code unit sequence
  of the high surrogate, and the error length is set to the length
  of that sequence.
  If more data is provided, validation must continue
  from the specified high surrogate.
  Requires the flag `UTF8CHK_CHECK_SURROGATES`.
* `UTF8CHK_ERR_SURROGATE_TRUNC3`: The string is truncated after a high
  surrogate. At least three more bytes are required.
  The error pointer is set to point to the start of the code unit sequence
  of the high surrogate, and the error length is set to the length
  of that sequence.
  If more data is provided, validation must continue
  from the specified high surrogate.
  Requires the flag `UTF8CHK_CHECK_SURROGATES`.

## FAQ

* **Q**: What about performance?
    * **A**: utf8chk is designed for correctness, robustness and portability
      over performance. There are e.g. no SIMD optimizations or anything
      of that sort.

## License

utf8chk is dual-licensed under the Unlicense (public domain equivalent)
and the MIT license.

## Examples

A simple validation of a UTF-8 C string with no care for the exact error
code or its location can be done as follows:

```c
if (utf8chk(string, UTF8CHK_CSTRING, UTF8CHK_UTF8, NULL, NULL)) {
    /* string is invalid */
} else {
    /* string is valid */
}
```

This code prints the error code (as an integer), the error index and
its length:

```c
const char *error_at;
size_t error_len;
utf8chk_error_t err = utf8chk(string, UTF8CHK_CSTRING, UTF8CHK_UTF8,
                              &error_at, &error_len);
if (err) {
    printf("UTF-8 validation error: error=%d at=%zu length=%zu\n",
           err, (size_t)(error_at - string), error_len);
}
```

## Tests

The included `utf8chk_test.c` tests utf8chk against a variety of input
strings and makes sure errors, if any, are correctly reported. It should
compile on any platform that has the C standard library available for
use by applications.
