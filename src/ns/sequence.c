#include "data/address.h"
#include "data/any.h"
#include "data/hashtable.h"
#include "data/integer.h"
#include "data/primitive.h"
#include "data/sequence.h"
#include "etor/evaluator.h"
#include "expr/identifier.h"

#define NS_NAME "sequence"

static void _by(struct Evaluator* etor, struct D_List* args);
static void _compare(struct Evaluator* etor, struct D_List* args);
static void _count(struct Evaluator* etor, struct D_List* args);
static void _from(struct Evaluator* etor, struct D_List* args);
static void _iterator(struct Evaluator* etor, struct D_List* args);
static void _new(struct Evaluator* etor, struct D_List* args);
static void _to(struct Evaluator* etor, struct D_List* args);

void ns_sequence_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    primitive_define(nsHash, "by", _by);
    primitive_define(nsHash, "compare", _compare);
    primitive_define(nsHash, "count", _count);
    primitive_define(nsHash, "from", _from);
    primitive_define(nsHash, "iterator", _iterator);
    primitive_define(nsHash, "new", _new);
    primitive_define(nsHash, "to", _to);
}

static void _by(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Sequence};
    struct Any* seqObj;
    struct Any** paramVars[] = {&seqObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_Sequence* seq = (struct D_Sequence*)seqObj;
    struct D_Integer* byInt = integer_new(sequence_getBy(seq));
    evaluator_pushObj(etor, (struct Any*)byInt);
}

static void _compare(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Sequence, T_Sequence};
    struct Any* seqObj1;
    struct Any* seqObj2;
    struct Any** paramVars[] = {&seqObj1, &seqObj2};
    primitive_checkArgs(2, paramTypes, args, paramVars, etor);
    struct D_Sequence* seq1 = (struct D_Sequence*)seqObj1;
    struct D_Sequence* seq2 = (struct D_Sequence*)seqObj2;
    struct D_Integer* compInt = integer_new(sequence_compare(seq1, seq2, etor));
    evaluator_pushObj(etor, (struct Any*)compInt);
}

static void _count(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Sequence};
    struct Any* seqObj;
    struct Any** paramVars[] = {&seqObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_Sequence* seq = (struct D_Sequence*)seqObj;
    struct D_Integer* countInt = integer_new(sequence_count(seq));
    evaluator_pushObj(etor, (struct Any*)countInt);
}

static void _from(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Sequence};
    struct Any* seqObj;
    struct Any** paramVars[] = {&seqObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_Sequence* seq = (struct D_Sequence*)seqObj;
    struct D_Integer* fromInt = integer_new(sequence_getFrom(seq));
    evaluator_pushObj(etor, (struct Any*)fromInt);
}

static void _iterator(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Sequence};
    struct Any* seqObj;
    struct Any** paramVars[] = {&seqObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_Sequence* seq = (struct D_Sequence*)seqObj;
    evaluator_pushObj(etor, (struct Any*)sequence_iterator(seq));
}

static void _new(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Integer, T_Integer, T_Integer};
    struct Any* fromObj = NULL;
    struct Any* toObj = NULL;
    struct Any* byObj = NULL;
    struct Any** paramVars[] = {&fromObj, &toObj, &byObj};
    int nArgs = primitive_checkArgs2(1, 3, paramTypes, args, paramVars, etor);
    int from = 1;
    int to = 0;
    int by = 1;
    switch (nArgs) {
        case 1:
            // if there's only one number, then it's the 'to' value
            to = integer_getValue((struct D_Integer*)fromObj);
            break;
        case 2:
            from = integer_getValue((struct D_Integer*)fromObj);
            to = integer_getValue((struct D_Integer*)toObj);
            break;
        case 3:
            from = integer_getValue((struct D_Integer*)fromObj);
            to = integer_getValue((struct D_Integer*)toObj);
            by = integer_getValue((struct D_Integer*)byObj);
            break;
    }
    struct D_Sequence* sequence = sequence_new(from, to, by);
    evaluator_pushObj(etor, (struct Any*)sequence);
}

static void _to(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Sequence};
    struct Any* seqObj;
    struct Any** paramVars[] = {&seqObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_Sequence* seq = (struct D_Sequence*)seqObj;
    struct D_Integer* toInt = integer_new(sequence_getTo(seq));
    evaluator_pushObj(etor, (struct Any*)toInt);
}
