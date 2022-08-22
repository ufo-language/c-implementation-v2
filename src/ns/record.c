#include "data/any.h"
#include "data/array.h"
#include "data/boolean.h"
#include "data/hashtable.h"
#include "data/list.h"
#include "data/primitive.h"
#include "data/record.h"
#include "data/symbol.h"
#include "etor/evaluator.h"
#include "expr/identifier.h"
#include "expr/recorddef.h"
#include "main/globals.h"

#define NS_NAME "record"

static void _definition(struct Evaluator* etor, struct D_List* args);
static void _fieldNames(struct Evaluator* etor, struct D_List* args);
static void _fieldTypes(struct Evaluator* etor, struct D_List* args);
static void _get(struct Evaluator* etor, struct D_List* args);
static void _set(struct Evaluator* etor, struct D_List* args);

void ns_record_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    primitive_define(nsHash, "definition", _definition);
    primitive_define(nsHash, "fieldNames", _fieldNames);
    primitive_define(nsHash, "fieldTypes", _fieldTypes);
    primitive_define(nsHash, "get", _get);
    primitive_define(nsHash, "set", _set);
}

static struct E_RecordDefinition* _definition_aux(struct Evaluator* etor, struct Any* arg) {
    enum TypeId typeId = any_typeId(arg);
    if (typeId != T_Record && typeId != T_Symbol) {
        primitive_argTypeException(T_Record, any_typeId(arg), arg, etor);
    }
    struct E_RecordDefinition* recordDef;
    if (typeId == T_Record) {
        struct D_Record* record = (struct D_Record*)arg;
        recordDef = record_getDefinition(record);
    }
    else {
        struct D_Symbol* recordTypeName = (struct D_Symbol*)arg;
        struct D_HashTable* recordNs = evaluator_getRecordNamespace(etor);
        recordDef = (struct E_RecordDefinition*)hashTable_get(recordNs, (struct Any*)recordTypeName, etor);
        if (recordDef == NULL) {
            evaluator_throwException(
                etor,
                symbol_new("Record"),
                "record type is not defined",
                (struct Any*)recordTypeName
            );
        }
    }
    return recordDef;
}

static void _definition(struct Evaluator* etor, struct D_List* args) {
    int nArgs = list_count(args);
    if (nArgs != 1) {
        primitive_argCountException(1, args, etor);
    }
    struct Any* arg = list_getFirst(args);
    struct E_RecordDefinition* recordDef = _definition_aux(etor, arg);
    evaluator_pushObj(etor, (struct Any*)recordDef);
}

static void _fieldNames(struct Evaluator* etor, struct D_List* args) {
    int nArgs = list_count(args);
    if (nArgs != 1) {
        primitive_argCountException(1, args, etor);
    }
    struct Any* arg = list_getFirst(args);
    struct E_RecordDefinition* recordDef = _definition_aux(etor, arg);
    struct D_Array* fieldNames = recordDefinition_getFieldNames(recordDef);
    evaluator_pushObj(etor, (struct Any*)fieldNames);
}

static void _fieldTypes(struct Evaluator* etor, struct D_List* args) {
    int nArgs = list_count(args);
    if (nArgs != 1) {
        primitive_argCountException(1, args, etor);
    }
    struct Any* arg = list_getFirst(args);
    struct E_RecordDefinition* recordDef = _definition_aux(etor, arg);
    struct D_Array* fieldTypes = recordDefinition_getFieldTypes(recordDef);
    evaluator_pushObj(etor, (struct Any*)fieldTypes);
}

static void _get(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Record, T_NULL};
    struct Any* recordObj;
    struct Any* fieldName;
    struct Any** paramVars[] = {&recordObj, &fieldName};
    primitive_checkArgs(2, paramTypes, args, paramVars, etor);
    struct D_Record* record = (struct D_Record*)recordObj;
    struct Any* value = record_getFieldValue(record, fieldName, etor);
    evaluator_pushObj(etor, value);
}

static void _set(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Record, T_NULL, T_NULL};
    struct Any* recordObj;
    struct Any* fieldName;
    struct Any* value;
    struct Any** paramVars[] = {&recordObj, &fieldName, &value};
    primitive_checkArgs(3, paramTypes, args, paramVars, etor);
    struct D_Record* record = (struct D_Record*)recordObj;
    record_setFieldValue(record, fieldName, value, etor);
    evaluator_pushObj(etor, (struct Any*)NIL);
}
