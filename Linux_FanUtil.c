#include <stdio.h>
#include "Linux_FanUtil.h"

static FILE *of = NULL;

void log_msg(char const *msg) {
    if (!of) {
        if (!(of = fopen("/tmp/Linux_FanProvider.log", "a"))) return;
    }
    fprintf(of, "%s\n", msg);
    fflush(of);
}

void util_cleanup() {
    if (of) {
        fclose(of);
        of = NULL;
    }
}
