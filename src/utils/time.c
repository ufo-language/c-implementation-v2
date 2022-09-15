#include <sys/time.h>
#include <time.h>

double timeNow_double() {
    struct timeval te;
    gettimeofday(&te, NULL);
    double timeNow = te.tv_sec + te.tv_usec / 1E6;
    return timeNow;
 }
