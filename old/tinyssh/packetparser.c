/*
20140210
Jan Mojzis
Public domain.

The 'packetparser' library is used to parse SSH packets.
The 'packetparser_*' function has always information about buffer, buffer length
and position in the buffer. Before parsing checks if the position
is not exceeding the buffer length. If the position exceeds the buffer length,
packetparser_* function immediately exits with 111 status code.
*/

#include "e.h"
#include "uint32_unpack_big.h"
#include "bug.h"
#include "byte.h"
#include "packetparser.h"

/*
Parse one-byte character.
*/
long long packetparser_uint8_(const char *fn, unsigned long long line,
                         const unsigned char *buf, long long len, long long pos, crypto_uint8 *out) {

    if (!buf || len < 0 || len > 1073741824 || pos < 0 || pos > 1073741824 || !out) bug_inval_(fn, line);
    if (pos + 1 > len) bug_proto_(fn, line);
    *out = buf[pos];
    return pos + 1;
}

/*
Parse the unsigned 32-bit number.
*/
long long packetparser_uint32_(const char *fn, unsigned long long line,
                          const unsigned char *buf, long long len, long long pos, crypto_uint32 *out) {

    if (!buf || len < 0 || len > 1073741824 || pos < 0 || pos > 1073741824 || !out) bug_inval_(fn, line);
    if (pos + 4 > len) bug_proto_(fn, line);
    *out = uint32_unpack_big(buf + pos);
    return pos + 4;

}

/*
Parse the string and copy it.
*/
long long packetparser_copy_(const char *fn, unsigned long long line,
                        const unsigned char *buf, long long len, long long pos, unsigned char *out, long long outlen) {

    if (!buf || len < 0 || len > 1073741824 || pos < 0 || pos > 1073741824 || !out || outlen < 0 || outlen > 1073741824) bug_inval_(fn, line);
    if (pos + outlen > len) bug_proto_(fn, line);
    byte_copy(out, outlen, buf + pos);
    return pos + outlen;
}

/*
Skip 'len' bytes.
*/
long long packetparser_skip_(const char *fn, unsigned long long line,
                        const unsigned char *buf, long long len, long long pos, long long skip) {

    if (!buf || len < 0 || len > 1073741824 || pos < 0 || pos > 1073741824 || skip < 0 || skip > 1073741824) bug_inval_(fn, line);
    if (pos + skip > len) bug_proto_(fn, line);
    return pos + skip;
}

/*
Check if the position is end-position.
*/
long long packetparser_end_(const char *fn, unsigned long long line,
                        const unsigned char *buf, long long len, long long pos) {

    if (!buf || len < 0 || len > 1073741824 || pos < 0 || pos > 1073741824) bug_inval_(fn, line);
    if (pos != len) bug_proto_(fn, line);
    return pos;
}
