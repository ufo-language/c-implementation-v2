#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/array.h"
#include "data/integer.h"
#include "data/sequence.h"
#include "data/symbol.h"
#include "dispatch/methodtable.h"
#include "etor/evaluator.h"
#include "gc/gc.h"
#include "main/globals.h"
#include "main/typedefs.h"
#include "utils/hash.h"

struct D_Sequence {
    struct Any obj;
    int from;
    int to;
    int by;
};

struct Methods* sequence_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_boolValue = (bool (*)(struct Any*))sequence_boolValue;
    methods->m_compare = (int (*)(struct Any*, struct Any*, struct Evaluator* etor))sequence_compare;
    methods->m_free = (void (*)(struct Any*))sequence_free;
    methods->m_hashCode = (HashCode (*)(struct Any*, struct Evaluator*))sequence_hashCode;
    methods->m_isEqual = (bool (*)(struct Any*, struct Any*))sequence_isEqual;
    methods->m_show = (void (*)(struct Any*, FILE*))sequence_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))sequence_sizeOf;
    methods->m_structSize = sequence_structSize;
    return methods;
}

struct D_Sequence* sequence_new(int from, int to, int by, struct Evaluator* etor) {
    if (by == 0) {
        evaluator_throwException(
            etor,
            symbol_new("Sequence"),
            "'by' component can't be 0",
            (struct Any*)array_newN(3, (struct Any*)integer_new(from),
                                       (struct Any*)integer_new(to),
                                       (struct Any*)integer_new(by))
        );
    }
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

HashCode sequence_hashCode(struct D_Sequence* self, struct Evaluator* etor) {
    (void)etor;
    return (self->from + self->to + self->by) ^ HASH_PRIMES[T_Sequence];
}

bool sequence_isEqual(struct D_Sequence* self, struct D_Sequence* other) {
    return self->from == other->from
        && self->to == other->to
        && self->by == other->by;
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
