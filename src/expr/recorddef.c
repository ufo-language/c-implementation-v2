#include <stdbool.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/array.h"
#include "data/hashtable.h"
#include "data/integer.h"
#include "data/list.h"
#include "data/queue.h"
#include "data/symbol.h"
#include "data/triple.h"
#include "etor/evaluator.h"
#include "expr/identifier.h"
#include "gc/gc.h"
#include "main/globals.h"

struct E_RecordDefinition {
    struct Any obj;
    struct D_Symbol* typeName;
    struct D_HashTable* nameMap;
    struct D_Array* fieldNames;
    struct D_Array* fieldTypes;
    struct D_Array* defaultValues;
};

struct E_RecordDefinition* recordDefinition_new(struct D_Symbol* typeName, struct D_Queue* fieldDefs, struct Evaluator* etor) {
    struct E_RecordDefinition* self = (struct E_RecordDefinition*)gc_alloc(T_RecordDefinition);
    int nFields = queue_count(fieldDefs);
    self->typeName = typeName;
    self->nameMap = hashTable_new();
    self->fieldNames = array_new(nFields);
    self->fieldTypes = array_new(nFields);
    self->defaultValues = array_new(nFields);
    for (int n=0; n<nFields; n++) {
        struct D_Array* fieldDef = (struct D_Array*)queue_deq_unsafe(fieldDefs);
        struct Any* fieldName = array_get_unsafe(fieldDef, 0);
        array_set_unsafe(self->fieldNames, n, fieldName);
        array_set_unsafe(self->fieldTypes, n, array_get_unsafe(fieldDef, 1));
        array_set_unsafe(self->defaultValues, n, array_get_unsafe(fieldDef, 2));
        hashTable_put(self->nameMap, fieldName, (struct Any*)integer_new(n), etor);
    }
    return self;
}

void recordDefinition_free(struct E_RecordDefinition* self) {
    free(self);
}

void recordDefinition_eval(struct E_RecordDefinition* self, struct Evaluator* etor) {
    struct D_HashTable* recordNs = evaluator_getRecordNamespace(etor);
    hashTable_put(recordNs, (struct Any*)self->typeName, (struct Any*)self, etor);
    evaluator_pushObj(etor, (struct Any*)NIL);
}

void recordDefinition_freeVars(struct E_RecordDefinition* self, struct D_Set* freeVars, struct Evaluator* etor) {
    any_freeVars((struct Any*)self->defaultValues, freeVars, etor);
}

struct D_Array* recordDefinition_getFieldNames(struct E_RecordDefinition* self) {
    return self->fieldNames;
}

struct D_Array* recordDefinition_getFieldTypes(struct E_RecordDefinition* self) {
    return self->fieldTypes;
}

struct D_HashTable* recordDefinition_getNameMap(struct E_RecordDefinition* self) {
    return self->nameMap;
}

struct D_Symbol* recordDefinition_getTypeName(struct E_RecordDefinition* self) {
    return self->typeName;
}

void recordDefinition_markChildren(struct E_RecordDefinition* self) {
    any_mark((struct Any*)self->typeName);
    any_mark((struct Any*)self->nameMap);
    any_mark((struct Any*)self->fieldNames);
    any_mark((struct Any*)self->fieldTypes);
    any_mark((struct Any*)self->defaultValues);
}

void recordDefinition_show(struct E_RecordDefinition* self, FILE* fp) {
    fputs("record ", fp);
    symbol_show(self->typeName, fp);
    fputs(" = ", fp);
    int nFields = array_count(self->fieldNames);
    bool firstIter = true;
    for (int n=0; n<nFields; n++) {
        if (firstIter) {
            firstIter = false;
        }
        else {
            fputs(", ", fp);
        }
        identifier_show((struct E_Identifier*)array_get_unsafe(self->fieldNames, n), fp);
        struct Any* fieldType = array_get_unsafe(self->fieldTypes, n);
        if (fieldType != (struct Any*)NIL) {
            fputs(" :: ", fp);
            any_show(fieldType, fp);
        }
        struct Any* defaultValue = array_get_unsafe(self->defaultValues, n);
        if (defaultValue != (struct Any*)NIL) {
            fputs(" = ", fp);
            any_show(defaultValue, fp);
        }
    }
}

size_t recordDefinition_sizeOf(struct E_RecordDefinition* self) {
    return sizeof(self);
}

size_t recordDefinition_structSize(void) {
    return sizeof(struct E_RecordDefinition);
}
