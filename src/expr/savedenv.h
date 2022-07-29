#if !defined(SAVEDENV_H)
#define SAVEDENV_H

#include <stdio.h>

struct Any;
struct D_Triple;
struct E_SavedEnv;
struct Evaluator;

struct E_SavedEnv* savedEnv_new(struct D_Triple* env);
void savedEnv_free(struct E_SavedEnv* self);

void savedEnv_eval(struct E_SavedEnv* self, struct Evaluator* etor);
void savedEnv_markChildren(struct E_SavedEnv* self);
void savedEnv_show(struct E_SavedEnv* self, FILE* fp);
size_t savedEnv_sizeOf(struct E_SavedEnv* self);
size_t savedEnv_structSize();

#endif
