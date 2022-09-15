#include <stdbool.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/array.h"
#include "data/hashtable.h"
#include "data/integer.h"
#include "data/list.h"
#include "data/queue.h"
#include "data/record.h"
#include "data/symbol.h"
#include "data/term.h"
#include "data/triple.h"
#include "etor/evaluator.h"
#include "expr/continuation.h"
#include "expr/identifier.h"
#include "expr/recorddef.h"
#include "main/globals.h"
#include "memory/gc.h"
#include "methods/methods.h"

struct D_Record {
    struct Any obj;
    struct E_RecordDefinition* recordDefinition;
    struct D_Array* values;
};

struct Methods* record_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_deepCopy = (struct Any* (*)(struct Any*))record_deepCopy;
    methods->m_free = (void (*)(struct Any*))record_free;
    methods->m_freeVars = (void (*)(struct Any*, struct D_Set*, struct Evaluator*))record_freeVars;
    methods->m_isEqual = (bool (*)(struct Any*, struct Any*))record_isEqual;
    methods->m_markChildren = (void (*)(struct Any* self))record_markChildren;
    methods->m_match = (struct D_Triple* (*)(struct Any*, struct Any*, struct D_Triple*))record_match;
    methods->m_show = (void (*)(struct Any*, FILE*))record_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))record_sizeOf;
    methods->m_structSize = record_structSize;
    return methods;
}

struct D_Record* record_new(struct E_RecordDefinition* recordDefinition, struct D_Array* values) {
    struct D_Record* self = (struct D_Record*)gc_alloc(T_Record);
    self->recordDefinition = recordDefinition;
    self->values = values;
    return self;
}

void record_free(struct D_Record* self) {
    free(self);
}

struct D_Record* record_deepCopy(struct D_Record* self) {
    return record_new(self->recordDefinition, array_deepCopy(self->values));
}

void record_freeVars(struct D_Record* self, struct D_Set* freeVars, struct Evaluator* etor) {
    any_freeVars((struct Any*)self->values, freeVars, etor);
}

struct E_RecordDefinition* record_getDefinition(struct D_Record* self) {
    return self->recordDefinition;
}

struct Any* record_getFieldValue(struct D_Record* self, struct Any* fieldName, struct Evaluator* etor) {
    struct D_HashTable* nameMap = recordDefinition_getNameMap(self->recordDefinition);
    struct Any* indexObj = hashTable_get(nameMap, fieldName);
    if (indexObj == NULL) {
        evaluator_throwException(
            etor,
            symbol_new("Record"),
            "requested field not found in record",
            (struct Any*)array_newN(2, (struct Any*)fieldName, (struct Any*)self)
        );
    }
    struct D_Integer* indexInt = (struct D_Integer*)indexObj;
    int index = integer_getValue(indexInt);
    return array_get_unsafe(self->values, index);
}

bool record_isEqual(struct D_Record* self, struct D_Record* other) {
    (void)self;
    (void)other;
    printf("%s is incomplete\n", __func__);
    return false;
}

void record_markChildren(struct D_Record* self) {
    any_mark((struct Any*)self->recordDefinition);
    any_mark((struct Any*)self->values);
}

struct D_Triple* record_match(struct D_Record* self, struct Any* other, struct D_Triple* bindings) {
    (void)self;
    (void)other;
    (void)bindings;
    printf("%s is incomplete\n", __func__);
    return NULL;
}

void record_setFieldValue(struct D_Record* self, struct Any* fieldName, struct Any* value, struct Evaluator* etor) {
    struct D_HashTable* nameMap = recordDefinition_getNameMap(self->recordDefinition);
    struct Any* indexObj = hashTable_get(nameMap, fieldName);
    if (indexObj == NULL) {
        evaluator_throwException(
            etor,
            symbol_new("Record"),
            "field not found in record",
            (struct Any*)array_newN(2, fieldName, (struct Any*)self)
        );
    }
    struct D_Integer* indexInt = (struct D_Integer*)indexObj;
    int index = integer_getValue(indexInt);
    struct Any* fieldType = array_get_unsafe(recordDefinition_getFieldTypes(self->recordDefinition), index);
    if (fieldType != (struct Any*)NIL && !any_hasType(value, fieldType)) {
        evaluator_throwException(
            etor,
            symbol_new("Record"),
            "field type violation",
            (struct Any*)array_newN(3, (struct Any*)fieldName, fieldType, (struct Any*)self)
        );        
    }
    array_set_unsafe(self->values, index, value);
}

void record_show(struct D_Record* self, FILE* fp) {
    fputc('#', fp);
    struct D_Symbol* recordTypeName = recordDefinition_getTypeName(self->recordDefinition);
    struct D_Array* fieldNames = recordDefinition_getFieldNames(self->recordDefinition);
    symbol_show(recordTypeName, fp);
    fputc('{', fp);
    bool firstIter = true;
    for (int n=0; n<array_count(fieldNames); n++) {
        if (firstIter) {
            firstIter = false;
        }
        else {
            fputs(", ", fp);
        }
        identifier_show((struct E_Identifier*)array_get_unsafe(fieldNames, n), fp);
        fputc('=', fp);
        any_show((struct Any*)array_get_unsafe(self->values, n), fp);
    }
    fputc('}', fp);
}

size_t record_sizeOf(struct D_Record* self) {
    return sizeof(self);
}

size_t record_structSize(void) {
    return sizeof(struct D_Record);
}

struct Any* record_typeOf(struct D_Record* self) {
    struct D_Array* fieldTypes = array_copy(recordDefinition_getFieldTypes(self->recordDefinition));
    struct D_Term* type = term_new(any_typeSymbol((struct Any*)self), fieldTypes);
    return (struct Any*)type;
}
