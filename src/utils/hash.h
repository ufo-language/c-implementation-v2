#if !defined(HASH_H)
#define HASH_H

#include "data/any.h"
#include "main/typedefs.h"

extern HashCode HASH_PRIMES[T_FINAL+1];

HashCode hashRotateLeft(HashCode n);

#endif
