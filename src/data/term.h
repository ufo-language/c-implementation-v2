#if !defined(TERM_H)
#define TERM_H

#include <stdbool.h>
#include <stdio.h>

struct Any;
struct D_Array;
struct D_Set;
struct D_Symbol;
struct D_Term;
struct Evaluator;

struct D_Term* term_new(struct D_Symbol* name, struct D_Array* elems);
void term_free(struct D_Term* self);

bool term_boolValue(struct D_Term* self);
int term_count(struct D_Term* self);
struct D_Term* term_deepCopy(struct D_Term* self);
void term_eval(struct D_Term* self, struct Evaluator* etor);
void term_freeVars(struct D_Term* self, struct D_Set* freeVars, struct Evaluator* etor);
struct Any* term_get_unsafe(struct D_Term* self, int n);
bool term_isEqual(struct D_Term* self, struct D_Term* other);
void term_markChildren(struct D_Term* self);
struct D_Triple* term_match(struct D_Term* self, struct Any* other, struct D_Triple* bindings);
struct D_Symbol* term_name(struct D_Term* self);
void term_set_unsafe(struct D_Term* self, int n, struct Any* elem);
void term_show(struct D_Term* self, FILE* fp);
size_t term_sizeOf(struct D_Term* self);
size_t term_structSize();

#endif
