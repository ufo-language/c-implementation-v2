#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/binding.h"
#include "data/integer.h"
#include "data/list.h"
#include "etor/evaluator.h"
#include "expr/continuation.h"
#include "expr/protocol.h"
#include "gc/gc.h"
#include "main/globals.h"

struct E_Protocol {
    struct Any obj;
};

struct E_Protocol* protocol_new() {
    struct E_Protocol* self = (struct E_Protocol*)gc_alloc(T_Protocol);
    return self;
}

void protocol_free(struct E_Protocol* self) {
    free(self);
}

void protocol_freeVars(struct E_Protocol* self, struct D_Set* freeVars, struct Evaluator* etor) {
    (void)self;
    (void)freeVars;
    (void)etor;
    // TODO
}

void protocol_eval(struct E_Protocol* self, struct Evaluator* etor) {
#if 0
    struct D_Integer* nBindings = integer_new(self->nBindings);
    struct E_Continuation* contin = continuation_new(protocol_continuation, "protocol", (struct Any*)nBindings);
    evaluator_pushExpr(etor, (struct Any*)contin);
    struct D_List* bindings = self->bindings;
    while (!list_isEmpty(bindings)) {
        struct D_Binding* binding = (struct D_Binding*)list_getFirst(bindings);
        evaluator_pushObj(etor, (struct Any*)binding_getKey(binding));
        evaluator_pushExpr(etor, (struct Any*)binding_getValue(binding));
        bindings = (struct D_List*)list_getRest(bindings);
    }
#endif
    printf("%s is incomplete\n", __func__);
    evaluator_pushObj(etor, (struct Any*)self);
}

void protocol_markChildren(struct E_Protocol* self) {
    (void)self;
    // TODO
}

void protocol_show(struct E_Protocol* self, FILE* fp) {
    (void)self;
    fprintf(fp, "Protocol{incomplete}");
}

size_t protocol_sizeOf(struct E_Protocol* self) {
    return sizeof(self);
}

size_t protocol_structSize() {
    return sizeof(struct E_Protocol);
}
