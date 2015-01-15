/*
20150113
Jan Mojzis
Public domain.
*/

#include <string.h>
#include "fail.h"
#include "numtostr.h"
#include "channel.h"

char *vector[] = {
    "0=0",
    "1=1",
    "2=2",
    "3=3",
    "4=4",
    "5=5",
    "6=6",
    "7=7",
    "8=8",
    "9=9",
    "10=10",
    "11=11",
    "12=12",
    "13=13",
    "14=14",
    "15=15",
    "16=16",
    "17=17",
    "18=18",
    "19=19",
    "20=20",
    "21=21",
    "22=22",
    "23=23",
    "24=24",
    "25=25",
    "26=26",
    "27=27",
    "28=28",
    "29=29",
    "30=30",
    "31=31",
    "32=32",
    "33=33",
    "34=34",
    "35=35",
    "36=36",
    "37=37",
    "38=38",
    "39=39",
    "40=40",
    "41=41",
    "42=42",
    "43=43",
    "44=44",
    "45=45",
    "46=46",
    "47=47",
    "48=48",
    "49=49",
    "50=50",
    "51=51",
    "52=52",
    "53=53",
    "54=54",
    "55=55",
    "56=56",
    "57=57",
    "58=58",
    "59=59",
    "60=60",
    "61=61",
    "62x=62",
    "63=63x",
    "64=64"
};

#if CHANNEL_SUBSYSTEM_MAX != 64
error !
#endif


static void test1(void) {

    long long i;

    for (i = 0; i < 64; ++i) {
        if (!channel_subsystem_add(vector[i])) fail("failure");
    }
    if (channel_subsystem_add(vector[i])) fail("failure");
}

static void test2(void) {

    long long i;
    char *x, *n;

    for (i = 0; i < 62; ++i) {
        n = numtostr(0, i);
        x = channel_subsystem_get(n);
        if (!x) fail("failure");
        if (strcmp(x, n)) fail("failure");
    }
    n = numtostr(0, 62);
    x = channel_subsystem_get(n);
    if (x) fail("failure");

    n = numtostr(0, 63);
    x = channel_subsystem_get(n);
    if (!x) fail("failure");
    if (!strcmp(x, n)) fail("failure");

    n = numtostr(0, 64);
    x = channel_subsystem_get(n);
    if (x) fail("failure");
}

int main(void) {
    test1();
    test2();
    _exit(0);
}
