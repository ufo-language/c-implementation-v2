#include <stdbool.h>

#include "main/typedefs.h"
#include "utils/hash.h"

// Assign one prime number to each type in data/any.h, even if
// that type is not hashable.
HashCode HASH_PRIMES[T_FINAL+1] = {
  5009, 5099, 5189, 5281, 5393, 5449, 5527, 5641,
  5701, 5801, 5861, 5953, 6067, 6143, 6229, 6311,
  6373, 6481, 6577, 6679, 6763, 6841, 6947, 7001,
  7109, 7211, 7307, 7417, 7507, 7573, 7649, 7727,
  7841, 7927, 8039, 8117, 8221, 8293, 8389, 8513,
  8599, 8681, 8747, 8837, 8933, 9013, 9127, 9203,
  9293, 9391, 9461, 9539 /*, 9643, 9739, 9817, 9901 */
  // more primes can be found here
  // https://primes.utm.edu/lists/small/10000.txt
};

static unsigned int highBit = ((unsigned int)-1) / 2 + 1;

HashCode hashRotateLeft(HashCode n) {
  bool g = n >= highBit;
  return (n << 1) + g;
}
