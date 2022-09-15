#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/array.h"
#include "data/boolean.h"
#include "data/integer.h"
#include "data/iterator.h"
#include "data/sequence.h"
#include "data/symbol.h"
#include "etor/evaluator.h"
#include "main/globals.h"
#include "main/typedefs.h"  // for HashCode
#include "memory/gc.h"
#include "methods/methods.h"
#include "utils/hash.h"

struct D_Sequence {
    struct Any obj;
    int from;
    int to;
    int by;
};

bool sequence_iteratorHasNext(struct D_Iterator* iterator);
struct Any* sequence_iteratorNext(struct D_Iterator* iterator);

struct Methods* sequence_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_boolValue = (bool (*)(struct Any*))sequence_boolValue;
    methods->m_compare = (int (*)(struct Any*, struct Any*, struct Evaluator* etor))sequence_compare;
    methods->m_free = (void (*)(struct Any*))sequence_free;
    methods->m_hashCode = (bool (*)(struct Any*, HashCode*))sequence_hashCode;
    methods->m_isEqual = (bool (*)(struct Any*, struct Any*))sequence_isEqual;
    methods->m_iterator = (struct D_Iterator* (*)(struct Any*))sequence_iterator;
    methods->m_iteratorHasNext = (bool (*)(struct D_Iterator*))sequence_iteratorHasNext;
    methods->m_iteratorNext = (struct Any* (*)(struct D_Iterator*))sequence_iteratorNext;
    methods->m_show = (void (*)(struct Any*, FILE*))sequence_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))sequence_sizeOf;
    methods->m_structSize = sequence_structSize;
    return methods;
}

struct D_Sequence* sequence_new(int from, int to, int by) {
    struct D_Sequence* self = (struct D_Sequence*)gc_alloc(T_Sequence);
    self->from = from;
    self->to = to;
    self->by = by;
    return self;
}

void sequence_free(struct D_Sequence* self) {
    free(self);
}

bool sequence_boolValue(struct D_Sequence* self) {
    return sequence_count(self) > 0;
}

int sequence_compare(struct D_Sequence* self, struct D_Sequence* other, struct Evaluator* etor) {
    (void)etor;
    int diff = self->from - other->from;
    if (diff != 0) {
        return diff < 0 ? -1 : 1;
    }
    diff = self->to - other->to;
    if (diff != 0) {
        return diff < 0 ? -1 : 1;
    }
    diff = self->by - other->by;
    if (diff != 0) {
        return diff < 0 ? -1 : 1;
    }
    return 0;
  }

/*

  1..7 % 3  contains  4 ?
  0..6 % 3  contains  3 ?

*/
bool sequence_contains(struct D_Sequence* self, int n) {
    int from = self->from;
    int to = self->to;
    int by = self->by;
    if (by < 0) {
        to = -to;
        by = -by;
    }
    to -= from;
    n -= from;
    return (n >= 0) && (n <= to) && (n % by == 0);
}

int sequence_count(struct D_Sequence* self) {
    int count = (self->to - self->from) / self->by + 1;
    return count >= 0 ? count : 0;
}

int sequence_getBy(struct D_Sequence* self) {
    return self->by;
}

int sequence_getFrom(struct D_Sequence* self) {
    return self->from;
}

int sequence_getTo(struct D_Sequence* self) {
    return self->to;
}

bool sequence_hashCode(struct D_Sequence* self, HashCode* hashCode) {
    *hashCode = (self->from + self->to + self->by) ^ HASH_PRIMES[T_Sequence];
    return true;
}

bool sequence_isEqual(struct D_Sequence* self, struct D_Sequence* other) {
    return self->from == other->from
        && self->to == other->to
        && self->by == other->by;
}

struct D_Iterator* sequence_iterator(struct D_Sequence* self) {
    struct Any* nObj;
    if (sequence_contains(self, self->from)) {
        nObj = (struct Any*)integer_new(self->from);
    }
    else {
        nObj = (struct Any*)NIL;
        }
    struct D_Array* iterObj = array_newN(2, nObj, self);
    return iterator_new((struct Any*)iterObj, T_Sequence);
}

bool sequence_iteratorHasNext(struct D_Iterator* iterator) {
    struct D_Array* state = (struct D_Array*)iterator_getStateObject(iterator);
    return T_Integer == any_typeId(array_get_unsafe(state, 0));
}

struct Any* sequence_iteratorNext(struct D_Iterator* iterator) {
    struct D_Array* state = (struct D_Array*)iterator_getStateObject(iterator);
    struct Any* nObj = array_get_unsafe(state, 0);
    if (T_Integer != any_typeId(nObj)) {
        return NULL;
    }
    int n = integer_getValue((struct D_Integer*)nObj);
    struct D_Sequence* seq = (struct D_Sequence*)array_get_unsafe(state, 1);
    int nextInt = n + seq->by;
    if (sequence_contains(seq, nextInt)) {
        array_set_unsafe(state, 0, (struct Any*)integer_new(nextInt));
    }
    else {
        array_set_unsafe(state, 0, (struct Any*)NIL);
    }
    return nObj;
}

void sequence_show(struct D_Sequence* self, FILE* fp) {
    fprintf(fp, "%d..%d", self->from, self->to);
    if (self->by != 1) {
        fprintf(fp, " %% %d", self->by);
    }
}

size_t sequence_sizeOf(struct D_Sequence* self) {
    (void)self;
    return sizeof(struct D_Sequence);
}

size_t sequence_structSize(void) {
    return sizeof(struct D_Sequence);
}
