#include <stdbool.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/array.h"
#include "data/binding.h"
#include "data/hashtable.h"
#include "data/integer.h"
#include "data/list.h"
#include "data/queue.h"
#include "data/record.h"
#include "data/symbol.h"
#include "data/triple.h"
#include "dispatch/methodtable.h"
#include "etor/evaluator.h"
#include "expr/continuation.h"
#include "expr/identifier.h"
#include "expr/recorddef.h"
#include "expr/recordspec.h"
#include "gc/gc.h"
#include "main/globals.h"

struct E_RecordSpec {
    struct Any obj;
    struct D_Symbol* recordTypeName;
    struct D_Queue* fieldBindings;
};

struct Methods* recordSpec_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_eval = (void (*)(struct Any*, struct Evaluator*))recordSpec_eval;
    methods->m_free = (void (*)(struct Any*))recordSpec_free;
    methods->m_freeVars = (void (*)(struct Any*, struct D_Set*, struct Evaluator*))recordSpec_freeVars;
    methods->m_markChildren = (void (*)(struct Any* self))recordSpec_markChildren;
    methods->m_match = (struct D_Triple* (*)(struct Any*, struct Any*, struct D_Triple*))recordSpec_match;
    methods->m_show = (void (*)(struct Any*, FILE*))recordSpec_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))recordSpec_sizeOf;
    methods->m_structSize = recordSpec_structSize;
    return methods;
}

struct E_RecordSpec* recordSpec_new(struct D_Symbol* recordTypeName, struct D_Queue* fieldBindings) {
    struct E_RecordSpec* self = (struct E_RecordSpec*)gc_alloc(T_RecordSpec);
    self->recordTypeName = recordTypeName;
    self->fieldBindings = fieldBindings;
    return self;
}

void recordSpec_free(struct E_RecordSpec* self) {
    free(self);
}

static void _contin(struct Evaluator* etor, struct Any* arg) {
    struct E_RecordDefinition* recordDef = (struct E_RecordDefinition*)arg;
    struct D_Array* values = (struct D_Array*)evaluator_popObj(etor);
    struct D_Record* record = record_new(recordDef, values);
    evaluator_pushObj(etor, (struct Any*)record);
}

void recordSpec_eval(struct E_RecordSpec* self, struct Evaluator* etor) {
    struct D_HashTable* recordNs = evaluator_getRecordNamespace(etor);
    int nElems = queue_count(self->fieldBindings);
    struct D_Array* valueAry = array_new(nElems);
    struct D_Symbol* typeName = self->recordTypeName;
    struct Any* recordDefObj = hashTable_get(recordNs, (struct Any*)typeName, etor);
    if (recordDefObj == NULL) {
        evaluator_throwException(
            etor,
            symbol_new("Record"),
            "record type is not defined",
            (struct Any*)self->recordTypeName
            );
    }
    struct E_RecordDefinition* recordDef = (struct E_RecordDefinition*)recordDefObj;
    struct D_List* bindings = queue_asList(self->fieldBindings);
    while (!list_isEmpty(bindings)) {
        struct D_Binding* binding = (struct D_Binding*)list_getFirst(bindings);
        struct Any* key = binding_getKey(binding);
        struct Any* indexObj = hashTable_get_unsafe(recordDefinition_getNameMap(recordDef), key);
        if (indexObj == NULL) {
            evaluator_throwException(
                etor,
                symbol_new("RecordSpec"),
                "field name is not part of record",
                (struct Any*)array_newN(2, (struct Any*)key, (struct Any*)self)
            );
        }
        struct D_Integer* indexInt = (struct D_Integer*)indexObj;
        int index = integer_getValue(indexInt);
        struct Any* value = binding_getValue(binding);
        array_set(valueAry, index, value, etor);
        bindings = (struct D_List*)list_getRest(bindings);
    }
    struct E_Continuation* contin = continuation_new(_contin, "recordSpec", (struct Any*)recordDef);
    evaluator_pushExpr(etor, (struct Any*)contin);
    evaluator_pushExpr(etor, (struct Any*)valueAry);
}

void recordSpec_freeVars(struct E_RecordSpec* self, struct D_Set* freeVars, struct Evaluator* etor) {
    any_freeVars((struct Any*)self->fieldBindings, freeVars, etor);
}

void recordSpec_markChildren(struct E_RecordSpec* self) {
    any_mark((struct Any*)self->recordTypeName);
    any_mark((struct Any*)self->fieldBindings);
}

struct D_Triple* recordSpec_match(struct E_RecordSpec* self, struct Any* other, struct D_Triple* bindings) {
    (void)self;
    (void)other;
    (void)bindings;
    printf("%s is incomplete\n", __func__);
    return NULL;
}

void recordSpec_show(struct E_RecordSpec* self, FILE* fp) {
    fputc('#', fp);
    symbol_show(self->recordTypeName, fp);
    fputc('{', fp);
    struct D_List* bindings = queue_asList(self->fieldBindings);
    bool firstIter = true;
    while (!list_isEmpty(bindings)) {
        if (firstIter) {
            firstIter = false;
        }
        else {
            fputs(", ", fp);
        }
        struct D_Binding* binding = (struct D_Binding*)list_getFirst(bindings);
        binding_show(binding, fp);
        bindings = (struct D_List*)list_getRest(bindings);
    }
    fputc('}', fp);
}

size_t recordSpec_sizeOf(struct E_RecordSpec* self) {
    return sizeof(self);
}

size_t recordSpec_structSize(void) {
    return sizeof(struct E_RecordSpec);
}
