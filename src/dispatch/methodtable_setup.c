#include "data/any.h"
#include "dispatch/methodtable.h"

struct Methods* iterator_methodSetup();
struct Methods* string_methodSetup();

// called from main.c
void methodTable_setupMethods(void) {
    printf("%s got here 1\n", __func__);
    METHOD_TABLE[T_Iterator] = iterator_methodSetup();
    METHOD_TABLE[T_String] = string_methodSetup();
}
