#if !defined(RECORDDEF_H)
#define RECORDDEF_H

#include <stdio.h>

struct D_Queue;
struct D_Set;
struct D_Symbol;
struct E_RecordDefinition;
struct Evaluator;

struct E_RecordDefinition* recordDefinition_new(struct D_Symbol* recordName, struct D_Queue* fieldDefs, struct Evaluator* etor);
void recordDefinition_free(struct E_RecordDefinition* self);

void recordDefinition_eval(struct E_RecordDefinition* self, struct Evaluator* etor);
void recordDefinition_freeVars(struct E_RecordDefinition* self, struct D_Set* freeVars, struct Evaluator* etor);
struct D_Symbol* recordDefinition_getTypeName(struct E_RecordDefinition* self);
struct D_Array* recordDefinition_getFieldNames(struct E_RecordDefinition* self);
struct D_Array* recordDefinition_getFieldTypes(struct E_RecordDefinition* self);
struct D_HashTable* recordDefinition_getNameMap(struct E_RecordDefinition* self);
void recordDefinition_markChildren(struct E_RecordDefinition* self);
void recordDefinition_show(struct E_RecordDefinition* self, FILE* fp);
size_t recordDefinition_sizeOf(struct E_RecordDefinition* self);
size_t recordDefinition_structSize(void);

#endif
