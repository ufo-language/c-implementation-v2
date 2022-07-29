#include <stdbool.h>

#include "utils/utils.h"

bool startsWith(const char *restrict string, const char *restrict prefix) {
    while(*prefix) {
        if (!*string || *prefix++ != *string++) {
            return false;
        }
    }
    return true;
}
