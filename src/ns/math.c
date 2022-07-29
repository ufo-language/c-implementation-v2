#include "data/hashtable.h"
#include "data/real.h"
#include "expr/identifier.h"

double PI = 3.141592653589793;
double TAU = 2.0 * 3.141592653589793;

void ns_math_defineAll(struct D_HashTable* env) {
    hashTable_put_unsafe(env, (struct Any*)identifier_new("pi"), (struct Any*)real_new(PI));
    hashTable_put_unsafe(env, (struct Any*)identifier_new("tau"), (struct Any*)real_new(TAU));
}
