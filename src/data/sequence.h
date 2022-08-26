#if !defined(SEQUENCE_H)
#define SEQUENCE_H

#include <stdbool.h>
#include <stdio.h>

#include "main/typedefs.h"  // for HashCode

struct D_Iterator;
struct D_Sequence;
struct Evaluator;

struct D_Sequence* sequence_new(int from, int to, int by);
void sequence_free(struct D_Sequence* self);

bool sequence_boolValue(struct D_Sequence* self);
int sequence_compare(struct D_Sequence* self, struct D_Sequence* other, struct Evaluator* etor);
bool sequence_contains(struct D_Sequence* self, int n);
int sequence_count(struct D_Sequence* self);
int sequence_getBy(struct D_Sequence* self);
int sequence_getFrom(struct D_Sequence* self);
int sequence_getTo(struct D_Sequence* self);
bool sequence_hashCode(struct D_Sequence* self, HashCode* hashCode);
bool sequence_isEqual(struct D_Sequence* self, struct D_Sequence* other);
struct D_Iterator* sequence_iterator(struct D_Sequence* self);
void sequence_show(struct D_Sequence* self, FILE* fp);
size_t sequence_sizeOf(struct D_Sequence* self);
size_t sequence_structSize(void);

#endif
