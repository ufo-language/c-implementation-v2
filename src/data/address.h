#if !defined(ADDRESS_H)
#define ADDRESS_H

#include <stdbool.h>
#include <stdio.h>

#include "data/any.h"
#include "main/typedefs.h"  // for HashCode

struct D_Address {
    struct Any obj;
    void* value;
};

struct D_Address* address_new(void* value);
void address_free(struct D_Address* self);

bool address_boolValue(struct D_Address* self);
int address_compare(struct D_Address* self, struct D_Address* other, struct Evaluator* etor);
bool address_isEqual(struct D_Address* self, struct D_Address* other);
void* address_getValue(struct D_Address* self);
bool address_hashCode(struct D_Address* self, HashCode* hashCode);
void address_setValue(struct D_Address* self, void* value);
void address_show(struct D_Address* self, FILE* fp);
size_t address_sizeOf(struct D_Address* self);
size_t address_structSize(void);

#endif
