#!/bin/sh

LANG=C
export LANG
LC_ALL=C
export LC_ALL

exec 2>&1

echo '--- log writes every message as one line, long messages are truncated'
# three messages -> three lines; the long one must not exceed the log
# buffer (512 bytes including the newline) and must not split into
# several lines; every line starts with the log prefix
./_tinysshd-test-log 2>&1 | awk '
    { ++lines; if (length($0) > maxlen) maxlen = length($0) }
    /^tinysshd: [a-zA-Z0-9]*: debug: / { ++prefixed }
    END {
        print "lines: " lines
        print "prefixed lines: " prefixed
        print "longest line within 511 bytes: " (maxlen <= 511 ? "yes" : "no")
    }
'
