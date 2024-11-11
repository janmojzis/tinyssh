/*
20150114
Jan Mojzis
Public domain.
*/

#include "str.h"
#include "log.h"
#include "e.h"
#include "channel.h"

static long long channel_subsystem_pos = 0;
static char *channel_subsystem[CHANNEL_SUBSYSTEM_MAX] = {0};

int channel_subsystem_add(const char *x) {

    if (channel_subsystem_pos >= CHANNEL_SUBSYSTEM_MAX) return 0;
    channel_subsystem[channel_subsystem_pos++] = (char *)x;
    return 1;
}

char *channel_subsystem_get(const char *x) {

    long long len, i;

    if (!x) return 0;

    len = str_len(x);
    for (i = 0; i < CHANNEL_SUBSYSTEM_MAX; ++i) {
        if (!channel_subsystem[i]) break;
        if (str_start(channel_subsystem[i], x) && (channel_subsystem[i][len] == '='))
            return channel_subsystem[i] + len + 1;
    }
    return 0;
}

void channel_subsystem_log(void) {

    long long i;

    for (i = 0; i < CHANNEL_SUBSYSTEM_MAX; ++i) {
        if (!channel_subsystem[i]) break;
        log_d3("subsystem: ", channel_subsystem[i], " registered");
    }
}
