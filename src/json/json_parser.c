#include <stdlib.h>

#include "data/any.h"
#include "data/binding.h"
#include "data/hashtable.h"
#include "data/list.h"
#include "data/queue.h"
#include "data/string.h"
#include "expr/identifier.h"
#include "lexer/lexer.h"
#include "main/globals.h"

struct Any;
struct D_List;
struct D_Symbol;

typedef struct Any* (*Parser)(struct D_List** tokens);

extern struct D_Symbol* IGNORE;

// functions from parser.c
struct Any* p_boolean(struct D_List** tokens);
struct Any* p_braceClose(struct D_List** tokens);
struct Any* p_braceOpen(struct D_List** tokens);
struct Any* p_bracketClose(struct D_List** tokens);
struct Any* p_bracketOpen(struct D_List** tokens);
struct Any* p_comma(struct D_List** tokens);
struct Any* p_integer(struct D_List** tokens);
struct Any* p_oneOf(struct D_List** tokens, ...);
struct Any* p_real(struct D_List** tokens);
struct Any* p_spotValue(struct D_List** tokens, struct D_Symbol* tokenType, struct Any* value);
struct Any* p_string(struct D_List** tokens);
struct D_Queue* p_sepBy(struct D_List** tokens, Parser parser, Parser separator);
struct D_Queue* p_seq(struct D_List** tokens, ...);
void p_fail(struct D_List** tokens, char* message, struct Any* arg);

// local functions
static struct Any* p_json(struct D_List** tokens);

static struct Any* p_colon(struct D_List** tokens) {
    static struct E_Identifier* colon = NULL;
    if (colon == NULL) {
        colon = identifier_new(":");
    }
    if (p_spotValue(tokens, LEXER_SYMBOLS[LTT_Operator], (struct Any*)colon)) {
        return (struct Any*)IGNORE;
    }
    p_fail(tokens, "Expected ':'", NULL);
    return NULL;
}

static struct Any* p_null(struct D_List** tokens) {
    static struct E_Identifier* nullWord = NULL;
    if (nullWord == NULL) {
        nullWord = identifier_new("null");
    }
    if (p_spotValue(tokens, LEXER_SYMBOLS[LTT_Identifier], (struct Any*)nullWord)) {
        return (struct Any*)NIL;
    }
    return NULL;
}

static struct Any* p_literal(struct D_List** tokens) {
    struct Any* res = p_oneOf(
        tokens,
        p_null,
        p_boolean,
        p_integer,
        p_real,
        p_string,
        0
    );
    return res;
}

static struct D_Queue* p_commaSepElems(struct D_List** tokens) {
    return p_sepBy(tokens, p_json, p_comma);
}

static struct Any* p_array(struct D_List** tokens) {
    struct D_Queue* parts = p_seq(tokens, p_bracketOpen, p_commaSepElems, p_bracketClose, 0);
    if (parts == NULL) {
        return NULL;
    }
    parts = (struct D_Queue*)queue_deq_unsafe(parts);
    int count = queue_count(parts);
    struct D_List* elems = queue_asList(parts);
    return (struct Any*)list_toArray(elems, count);
}

static struct Any* p_string_(struct D_List** tokens) {
  struct Any* res = p_string(tokens);
  return res;
}

static struct Any* p_objBinding_required(struct D_List** tokens) {
    struct D_Queue* parts = p_seq(tokens, p_string_, p_colon, p_json, 0);
    if (parts == NULL) {
        return NULL;
    }
    struct Any* lhs = queue_deq_unsafe(parts);
    struct Any* rhs = queue_deq_unsafe(parts);
    struct D_Binding* binding = binding_new(lhs, rhs);
    return (struct Any*)binding;
}
    
static struct D_Queue* p_objCommaBindings(struct D_List** tokens) {
    return p_sepBy(tokens, p_objBinding_required, p_comma);
}

static struct Any* p_object(struct D_List** tokens) {
    struct D_Queue* bindings = p_seq(tokens, p_braceOpen, p_objCommaBindings, p_braceClose, 0);
    if (bindings == NULL) {
        return NULL;
    }
    bindings = (struct D_Queue*)queue_deq_unsafe(bindings);
    struct D_HashTable* hash = hashTable_new();
    while (!queue_isEmpty(bindings)) {
        struct D_Binding* binding = (struct D_Binding*)queue_deq_unsafe(bindings);
        hashTable_put_unsafe(hash, binding_getKey(binding), binding_getValue(binding));
    }
    return (struct Any*)hash;
}

static struct Any* p_json(struct D_List** tokens) {
    struct Any* res = p_oneOf(
        tokens,
        p_literal,
        p_array,
        p_object,
        0
    );
    return res;
}

struct Any* json_parse(struct D_String* string) {
    struct Any* res = lexer_tokenize(string);
    if (res->typeId != T_List) {
        return res;
    }
    struct D_List* tokens = (struct D_List*)res;
    struct Any* jsonRes = p_json(&tokens);
    if (jsonRes == NULL) {
        jsonRes = (struct Any*)NIL;
    }
    return jsonRes;
}
