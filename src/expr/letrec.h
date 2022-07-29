#if !defined(LETREC_H)
#define LETREC_H

#include <stddef.h>
#include <stdio.h>

struct D_Array;
struct D_List;
struct D_Set;
struct E_LetRec;
struct Evaluator;

struct E_LetRec* letRec_new(struct D_List* bindings, int nBindings);
void letRec_free(struct E_LetRec* self);

struct E_LetRec* letRec_deepCopy(struct E_LetRec* self);
void letRec_eval(struct E_LetRec* self, struct Evaluator* etor);
void letRec_freeVars(struct E_LetRec* self, struct D_Set* freeVars, struct Evaluator* etor);
void letRec_markChildren(struct E_LetRec* self);
void letRec_show(struct E_LetRec* self, FILE* fp);
size_t letRec_sizeOf(struct E_LetRec* self);
size_t letRec_structSize();

#endif
