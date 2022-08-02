// This is an example of how to create a .so file.
// Compile with:
// gcc -shared -o libhello.so -fPIC hello.c

#include <stdio.h>

void __attribute__ ((constructor)) fileStartup(void) {
    printf("%s called\n", __func__);
}

void __attribute__ ((constructor)) fileShutdown(void) {
    printf("%s called\n", __func__);
}

void hello(void) {
    printf("Hello, world! from %s\n", __func__);
}

