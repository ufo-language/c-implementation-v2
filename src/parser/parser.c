#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>

#include "data/array.h"
#include "data/binding.h"
#include "data/hashtable.h"
#include "data/integer.h"
#include "data/list.h"
#include "data/queue.h"
#include "data/set.h"
#include "data/string.h"
#include "data/symbol.h"
#include "data/term.h"
#include "expr/abstraction.h"
#include "expr/apply.h"
#include "expr/async.h"
#include "expr/binop.h"
#include "expr/bracketexpr.h"
#include "expr/cobegin.h"
#include "expr/do.h"
#include "expr/identifier.h"
#include "expr/if.h"
#include "expr/let.h"
#include "expr/letin.h"
#include "expr/letrec.h"
#include "expr/loop.h"
#include "expr/nondet.h"
#include "expr/recorddef.h"
#include "expr/recordspec.h"
#include "expr/trycatch.h"
#include "main/globals.h"
#include "lexer/lexer.h"

// NOTES
// Write this in the simplest way possible.
// Don't use memoization.
// Memoization can be added later using global variables.
// That's not reentrant, but how often will I actually need a reentrant parser?
// I can just lock the parser using a mutex.

typedef struct Any*          Obj;
typedef struct D_Array*      Array;
typedef struct D_Integer*    Integer;
typedef struct D_List*       List;
typedef struct D_String*     String;
typedef struct D_Symbol*     Symbol;
typedef struct E_Identifier* Identifier;

typedef Obj (*Parser)(List* tokens);

// forward declarations ==============================================

Obj p_any(List* tokens);
Obj p_any_required(List* tokens);
Obj p_array(List* tokens);
Obj p_list(List* tokens);

struct D_Queue* p_many(List* tokens);
static Obj p_special(List* tokens, String str);

// utility functions =================================================

static char* _errMessage = NULL;
static List _errTokens;
static Obj _errObj;
static struct Evaluator* _etor;
static jmp_buf jumpBuf;

struct Any* parser_parse(List* tokens, struct Evaluator* etor) {
    _errTokens = NULL;
    _errObj = NULL;
    _etor = etor;
    int jumpRes = setjmp(jumpBuf);
    Obj parseRes = NULL;
    if (jumpRes == 0) {
        parseRes = p_any(tokens);
        if (parseRes == NULL) {
            fprintf(stderr, "No parse\n");
        }
    }
    else {
        fprintf(stderr, "ERROR: %s", _errMessage);
        if (_errObj != NULL) {
            fputs(": ", stderr);
            any_show(_errObj, stderr);
        }
        fputc('\n', stderr);
        fprintf(stderr, "found ");
        any_show((Obj)_errTokens, stderr);
        fputc('\n', stderr);
    }
    return parseRes;
}

// utility functions =================================================

#if 0
union ParserPointer {
    Parser parser;
    void* pointer;
};

static void printParser(Parser p) {
    union ParserPointer u;
    u.parser = p;
    printf("%p", u.pointer);
}
#endif

static String BAR;
static String BRACE_CLOSE;
static String BRACE_OPEN;
static String BRACKET_CLOSE;
static String BRACKET_OPEN;
static String COMMA;
static String DOLLAR;
static String HASH_MARK;
static String PAREN_CLOSE;
static String PAREN_OPEN;
static String TILDE;
Symbol IGNORE;  // needed by json_parser.c

static String ASYNC;
static String CATCH;
static String COBEGIN;
static String DO;
static String ELSE;
static String END;
static String FUN;
static String IF;
static String IN;
static String LET;
static String LETREC;
static String LOOP;
static String NONDET;
static String RECORD;
static String THEN;
static String TRY;

static Identifier EQUAL_SIGN;
static Identifier COLON;
static Identifier DOUBLE_COLON;

void parser_permanentObjects(void) {
    BAR = string_new("|");
    BRACE_CLOSE = string_new("}");
    BRACE_OPEN = string_new("{");
    BRACKET_CLOSE = string_new("]");
    BRACKET_OPEN = string_new("[");
    COMMA = string_new(",");
    DOLLAR = string_new("$");
    HASH_MARK = string_new("#");
    PAREN_CLOSE = string_new(")");
    PAREN_OPEN = string_new("(");
    TILDE = string_new("~");

    ASYNC = string_new("async");
    CATCH = string_new("catch");
    COBEGIN = string_new("cobegin");
    DO = string_new("do");
    ELSE = string_new("else");
    END = string_new("end");
    FUN = string_new("fun");
    IF = string_new("if");
    IN = string_new("in");
    LET = string_new("let");
    LETREC = string_new("letrec");
    LOOP = string_new("loop");
    NONDET = string_new("nondet");
    RECORD = string_new("record");
    THEN = string_new("then");
    TRY = string_new("try");
}

void parser_rootObjects(void) {
    IGNORE = symbol_new("$__IGNORE__");
    EQUAL_SIGN = identifier_new("=");
    COLON = identifier_new(":");
    DOUBLE_COLON = identifier_new("::");
}

// primitive parsers =================================================

Obj p_spot(List* tokens, Symbol tokenType) {
    Array firstToken = (Array)list_getFirst(*tokens);
    Symbol tokenSym = (Symbol)array_get_unsafe(firstToken, 0);
    if (tokenSym == tokenType) {
        *tokens = (List)list_getRest(*tokens);
        return array_get_unsafe(firstToken, 1);
    }
    return NULL;
}

Obj p_spotValue(List* tokens, Symbol tokenType, Obj value) {
    Array firstToken = (Array)list_getFirst(*tokens);
    Symbol tokenSym = (Symbol)array_get_unsafe(firstToken, 0);
    if (tokenSym == tokenType) {
        Obj value1 = array_get_unsafe(firstToken, 1);
        if (any_isEqual(value, value1)) {
            *tokens = (List)list_getRest(*tokens);
            return value;
        }
    }
    return NULL;
}

void p_fail(List* tokens, char* message, Obj arg) {
    _errTokens = *tokens;
    _errMessage = message;
    _errObj = arg;
    longjmp(jumpBuf, 1);
}

// special characters ================================================

Obj p_bar(List* tokens) {
    return p_special(tokens, BAR) ? (Obj)IGNORE : NULL;
}

Obj p_braceOpen(List* tokens) {
    return p_special(tokens, BRACE_OPEN) ? (Obj)IGNORE : NULL;
}

Obj p_braceClose(List* tokens) {
    if (p_special(tokens, BRACE_CLOSE)) {
        return (Obj)IGNORE;
    }
    p_fail(tokens, "Expected '}' or ','", NULL);
    return  NULL;
}

Obj p_bracketOpen(List* tokens) {
    return p_special(tokens, BRACKET_OPEN) ? (Obj)IGNORE : NULL;
}

Obj p_bracketClose(List* tokens) {
    if (p_special(tokens, BRACKET_CLOSE)) {
        return (Obj)IGNORE;
    }
    p_fail(tokens, "Expected ']'", NULL);
    return  NULL;
}

Obj p_comma(List* tokens) {
    return p_special(tokens, COMMA) ? (Obj)IGNORE : NULL;
}

Obj p_dollar(List* tokens) {
    return p_special(tokens, DOLLAR) ? (Obj)IGNORE : NULL;
}

Obj p_parenOpen(List* tokens) {
    return p_special(tokens, PAREN_OPEN) ? (Obj)IGNORE : NULL;
}

Obj p_parenOpen_required(List* tokens) {
    if (p_special(tokens, PAREN_OPEN)) {
        return (Obj)IGNORE;
    }
    p_fail(tokens, "Expected '('", NULL);
    return NULL;
}

Obj p_parenClose_required(List* tokens) {
    if (p_special(tokens, PAREN_CLOSE)) {
        return (Obj)IGNORE;
    }
    p_fail(tokens, "Expected ')'", NULL);
    return  NULL;
}

Obj p_tilde(List* tokens) {
    return p_special(tokens, TILDE) ? (Obj)IGNORE : NULL;
}

Obj p_equalSign(List* tokens) {
    return p_spotValue(tokens, LEXER_SYMBOLS[LTT_Operator], (Obj)EQUAL_SIGN) ? (Obj)IGNORE : NULL;
}

Obj p_equalSign_required(List* tokens) {
    if (p_spotValue(tokens, LEXER_SYMBOLS[LTT_Operator], (Obj)EQUAL_SIGN)) {
        return (Obj)IGNORE;
    }
    p_fail(tokens, "Expected '='", NULL);
    return NULL;
}

Obj p_hashMark(List* tokens) {
    return p_special(tokens, HASH_MARK) ? (Obj)IGNORE : NULL;
}

// combinators =======================================================

Obj p_ignore(List* tokens, Parser parser) {
    return parser(tokens) ? (Obj)IGNORE : NULL;
}

Obj p_maybe(List* tokens, Parser parser) {
    Obj res = parser(tokens);
    return res ? res : (Obj)NIL;
}

Obj p_oneOf(List* tokens, ...) {
    va_list argList;
    va_start(argList, tokens);
    Parser parser;
    List savedTokens = *tokens;
    Obj value = NULL;
    while ((parser = va_arg(argList, Parser))) {
        value = parser(tokens);
        if (value) {
            break;
        }
        *tokens = savedTokens;
    }
    va_end(argList);
    return value;
}

struct D_Queue* p_sepBy(List* tokens, Parser parser, Parser separator) {
    struct D_Queue* objects = queue_new();
    bool firstIter = true;
    while (true) {
        Obj res = parser(tokens);
        if (res == NULL) {
            if (firstIter) {
                break;
            }
            else {
                p_fail(tokens, "Object expected after separator", NULL);
            }
        }
        queue_enq(objects, res);
        res = separator(tokens);
        if (res == NULL) {
            break;
        }
        firstIter = false;
    }
    return objects;
}

struct D_Queue* p_seq(List* tokens, ...) {
    struct D_Queue* q = queue_new();
    va_list argList;
    va_start(argList, tokens);
    Parser parser;
    while ((parser = va_arg(argList, Parser))) {
        Obj value = parser(tokens);
        if (value == NULL) {
            va_end(argList);
            return NULL;
        }
        if (value != (Obj)IGNORE) {
            queue_enq(q, value);
        }
    }
    va_end(argList);
    return q;
}

Obj p_some(List* tokens, int minCount, Parser parser) {
    struct D_Queue* q = queue_new();
    List tokens1 = *tokens;
    int count = 0;
    while (true) {
        Obj value = parser(&tokens1);
        if (value == NULL) {
            break;
        }
        count++;
        queue_enq(q, value);
    }
    if (count < minCount) {
        return NULL;
    }
    *tokens = tokens1;
    return (Obj)queue_asList(q);
}

struct D_Queue* p_commaSepElems(List* tokens) {
    return p_sepBy(tokens, p_any, p_comma);
}

// literals ==========================================================

Obj p_boolean(List* tokens) {
    return p_spot(tokens, LEXER_SYMBOLS[LTT_Boolean]);
}

Obj p_identifier(List* tokens) {
    return p_spot(tokens, LEXER_SYMBOLS[LTT_Identifier]);
}

Obj p_integer(List* tokens) {
    return p_spot(tokens, LEXER_SYMBOLS[LTT_Integer]);
}

Obj p_nil(List* tokens) {
    return p_spot(tokens, LEXER_SYMBOLS[LTT_Nil]);
}

Obj p_real(List* tokens) {
    return p_spot(tokens, LEXER_SYMBOLS[LTT_Real]);
}

Obj p_reserved(List* tokens, String word) {
    return p_spotValue(tokens, LEXER_SYMBOLS[LTT_Reserved], (Obj)word) ? (Obj)IGNORE : NULL;
}

Obj p_reserved_required(List* tokens, String word) {
    if (p_spotValue(tokens, LEXER_SYMBOLS[LTT_Reserved], (Obj)word)) {
        return (Obj)IGNORE;
    }
    p_fail(tokens, "Expected a reserved word", (Obj)word);
    return NULL;
}

Obj p_special(List* tokens, String str) {
    return p_spotValue(tokens, LEXER_SYMBOLS[LTT_Special], (Obj)str);
}

Obj p_string(List* tokens) {
    return p_spot(tokens, LEXER_SYMBOLS[LTT_String]);
}

Obj p_symbol(List* tokens) {
    return p_spot(tokens, LEXER_SYMBOLS[LTT_Symbol]);
}

Obj p_symbol_required(List* tokens) {
    Obj res = p_spot(tokens, LEXER_SYMBOLS[LTT_Symbol]);
    if (res != NULL) {
        return res;
    }
    p_fail(tokens, "Expected a symbol", NULL);
    return  NULL;
}

// data structures ===================================================

Obj p_array(List* tokens) {
    struct D_Queue* parts = p_seq(tokens, p_braceOpen, p_commaSepElems, p_braceClose, 0);
    if (parts == NULL) {
        return NULL;
    }
    parts = (struct D_Queue*)queue_deq_unsafe(parts);
    int count = queue_count(parts);
    List elems = (List)queue_asList(parts);
    return (Obj)list_toArray(elems, count);
}

Obj p_bindingLhs(List* tokens) {
    return p_oneOf(tokens, p_symbol, p_integer, p_identifier, p_list, p_array, 0);
}

Obj p_binding(List* tokens) {
    List savedTokens = *tokens;
    struct D_Queue* parts = p_seq(tokens, p_bindingLhs, p_equalSign, p_any, 0);
    if (parts == NULL) {
        *tokens = savedTokens;
        return NULL;
    }
    Obj lhs = queue_deq_unsafe(parts);
    Obj rhs = queue_deq_unsafe(parts);
    struct D_Binding* binding = binding_new(lhs, rhs);
    return (Obj)binding;
}

Obj p_binding_required(List* tokens) {
    struct D_Queue* parts = p_seq(tokens, p_bindingLhs, p_equalSign_required, p_any, 0);
    if (parts == NULL) {
        return NULL;
    }
    Obj lhs = queue_deq_unsafe(parts);
    Obj rhs = queue_deq_unsafe(parts);
    struct D_Binding* binding = binding_new(lhs, rhs);
    return (Obj)binding;
}
    
struct D_Queue* p_commaBindings(List* tokens) {
    return p_sepBy(tokens, p_binding_required, p_comma);
}

struct D_Queue* p_commaBindings_required(List* tokens) {
    List savedTokens = *tokens;
    struct D_Queue* elems = p_sepBy(tokens, p_binding_required, p_comma);
    if (queue_isEmpty(elems)) {
        *tokens = savedTokens;
        p_fail(tokens, "list of bindings exepected", NULL);
    }
    return elems;
}

Obj p_hashTable(List* tokens) {
    struct D_Queue* bindings = p_seq(tokens, p_hashMark, p_braceOpen, p_commaBindings, p_braceClose, 0);
    if (bindings == NULL) {
        return NULL;
    }
    bindings = (struct D_Queue*)queue_deq_unsafe(bindings);
    struct D_HashTable* hash = hashTable_new();
    while (!queue_isEmpty(bindings)) {
        struct D_Binding* binding = (struct D_Binding*)queue_deq_unsafe(bindings);
        hashTable_put(hash, binding_getKey(binding), binding_getValue(binding), _etor);
    }
    return (Obj)hash;
}

// list parsers
Obj p_improper(List* tokens) {
    Obj res = (Obj)p_seq(tokens, p_bar, p_any, 0);
    return res;
}

Obj p_maybeImproper(List* tokens) {
    return p_maybe(tokens, p_improper);
}

Obj p_list(List* tokens) {
    struct D_Queue* elems = p_seq(tokens, p_bracketOpen, p_commaSepElems, p_maybeImproper, p_bracketClose, 0);
    if (elems == NULL) {
        return NULL;
    }
    struct D_Queue* elems1 = (struct D_Queue*)queue_deq_unsafe(elems);
    if (elems1 == NULL) {
        return NULL;
    }
    Obj rest = queue_deq_unsafe(elems);
    if (rest == (Obj)NIL) {
        return (Obj)queue_asList(elems1);
    }
    struct D_Queue* finalElemQueue = (struct D_Queue*)rest;
    struct Any* finalElem = queue_deq_unsafe(finalElemQueue);
    struct D_List* queueTail = queue_getTail(elems1);
    list_setRest(queueTail, finalElem);
    return (Obj)queue_asList(elems1);
}

// queue parser
Obj p_queue(List* tokens) {
    Obj res = p_tilde(tokens);
    if (res == NULL) {
        return NULL;
    }
    Obj listObj = p_list(tokens);
    if (listObj == NULL) {
        p_fail(tokens, "elements expected after '~'", NULL);
    }
    struct D_List* list = (struct D_List*)listObj;
    struct D_Queue* queue = queue_new();
    while (!list_isEmpty(list)) {
        Obj elem = list_getFirst(list);
        queue_enq(queue, elem);
        list = (struct D_List*)list_getRest(list);
    }
    return (struct Any*)queue;
}

// set parser
Obj p_set(List* tokens) {
    Obj res = p_dollar(tokens);
    if (res == NULL) {
        return NULL;
    }
    Obj aryObj = p_array(tokens);
    if (aryObj == NULL) {
        p_fail(tokens, "elements expected after '$'", NULL);
    }
    struct D_Array* ary = (struct D_Array*)aryObj;
    struct D_Set* set = set_new();
    for (int n=0; n<array_count(ary); n++) {
        set_add(set, array_get_unsafe(ary, n), _etor);
    }
    return (struct Any*)set;
}

// term parser
Obj p_term(List* tokens) {
    struct D_List* savedTokens = *tokens;
    struct D_Queue* parts = p_seq(tokens, p_symbol, p_array, 0);
    if (parts == NULL) {
        *tokens = savedTokens;
        return NULL;
    }
    struct D_Symbol* sym = (struct D_Symbol*)queue_deq_unsafe(parts);
    struct D_Array* elems = (struct D_Array*)queue_deq_unsafe(parts);
    struct D_Term* term = term_new(sym, elems);
    return (struct Any*)term;
}

// reserved words ====================================================

Obj p_ASYNC(List* tokens) {
    return p_reserved(tokens, ASYNC);
}

Obj p_CATCH(List* tokens) {
    return p_reserved_required(tokens, CATCH);
}

Obj p_COBEGIN(List* tokens) {
    return p_reserved(tokens, COBEGIN);
}

Obj p_DO(List* tokens) {
    return p_reserved(tokens, DO);
}

Obj p_ELSE(List* tokens) {
    return p_reserved_required(tokens, ELSE);
}

Obj p_END(List* tokens) {
    return p_reserved_required(tokens, END);
}

Obj p_FUN(List* tokens) {
    return p_reserved(tokens, FUN);
}

Obj p_IF(List* tokens) {
    return p_reserved(tokens, IF);
}

Obj p_IN(List* tokens) {
    return p_reserved(tokens, IN);
}

Obj p_LET(List* tokens) {
    return p_reserved(tokens, LET);
}

Obj p_LETREC(List* tokens) {
    return p_reserved(tokens, LETREC);
}

Obj p_LOOP(List* tokens) {
    return p_reserved(tokens, LOOP);
}

Obj p_NONDET(List* tokens) {
    return p_reserved(tokens, NONDET);
}

Obj p_RECORD(List* tokens) {
    return p_reserved(tokens, RECORD);
}

Obj p_THEN(List* tokens) {
    return p_reserved_required(tokens, THEN);
}

Obj p_TRY(List* tokens) {
    return p_reserved(tokens, TRY);
}

// expressions =======================================================

Obj p_colonOperator(List* tokens) {
    return p_spotValue(tokens, LEXER_SYMBOLS[LTT_Operator], (Obj)COLON);
}

Obj p_colonRhs(List* tokens) {
    return p_oneOf(tokens, p_identifier, p_symbol, p_integer, 0);
}

struct E_BinOp* p_colonExpr(List* tokens) {
    List savedTokens = *tokens;
    Obj lhs = p_identifier(tokens);
    if (lhs == NULL) {
        *tokens = savedTokens;
        return NULL;
    }
    Obj colonOp = p_colonOperator(tokens);
    if (colonOp == NULL) {
        *tokens = savedTokens;
        return NULL;
    }
    Obj rhs = p_colonRhs(tokens);
    if (rhs == NULL) {
        p_fail(tokens, "Expression expected after ':'", NULL);
    }
    struct E_BinOp* binOp = binOp_new(lhs, (Identifier)colonOp, rhs);
    return binOp;
}

Obj p_apply(List* tokens) {
    struct D_List* savedTokens = *tokens;
    Obj abstr = p_oneOf(tokens, p_colonExpr, p_identifier, 0);
    if (!abstr) {
        struct D_Queue* abstrParts = p_seq(tokens, p_parenOpen, p_any_required, p_parenClose_required, 0);
        if (!abstrParts) {
            *tokens = savedTokens;
            return NULL;
        }
        abstr = queue_deq_unsafe(abstrParts);
    }
    struct D_Queue* argParts = p_seq(tokens, p_parenOpen, p_commaSepElems, p_parenClose_required, 0);
    if (!argParts) {
        *tokens = savedTokens;
        return NULL;
    }
    struct D_List* args = queue_asList((struct D_Queue*)queue_deq_unsafe(argParts));
    return (Obj)apply_new(abstr, args);
}

struct E_Abstraction* p_funRule(List* tokens) {
    struct D_Queue* params = p_seq(tokens, p_parenOpen_required, p_commaSepElems, p_parenClose_required, 0);
    params = (struct D_Queue*)queue_deq_unsafe(params);
    p_equalSign_required(tokens);
    Obj body = p_any(tokens);
    return abstraction_new(queue_asList(params), (struct Any*)body);
}

struct E_Abstraction* p_funRules(List* tokens) {
    struct E_Abstraction* firstRule = NULL;
    struct E_Abstraction* lastRule = NULL;
    struct E_Abstraction* rule = NULL;
    while (true) {
        rule = p_funRule(tokens);
        if (rule == NULL) {
            p_fail(tokens, "ERROR: function rule expected", NULL);
        }
        if (firstRule == NULL) {
            firstRule = rule;
            lastRule = rule;
        }
        else {
            abstraction_setNext(lastRule, rule);
            lastRule = rule;
        }
        if (!p_bar(tokens)) {
            break;
        }
    }
    return firstRule;
}

Obj p_abstraction(List* tokens) {
    if (!p_FUN(tokens)) {
        return NULL;
    }
    Obj name = p_identifier(tokens);
    Obj abstr = (Obj)p_funRules(tokens);
    if (name != NULL) {
        struct D_Binding* binding = binding_new((struct Any*)name, abstr);
        struct D_List* bindings = list_new((struct Any*)binding, (struct Any*)EMPTY_LIST);
        struct E_LetRec* letRec = letRec_new(bindings, 1);
        return (Obj)letRec;
    }
    return abstr;
}

Obj p_block(Parser reservedWord, Obj (*constructor)(struct D_Array*), List* tokens) {
    struct D_Queue* exprs = (struct D_Queue*)p_seq(tokens, reservedWord, p_many, p_END, 0);
    if (exprs == NULL) {
        return NULL;
    }
    exprs = (struct D_Queue*)queue_deq_unsafe(exprs);
    int count = queue_count(exprs);
    Array ary = array_new(count);
    for (int n=0; n<count; n++) {
        array_set_unsafe(ary, n, queue_deq_unsafe(exprs));
    }
    return constructor(ary);
}

Obj p_async(List* tokens) {
    return p_block(p_ASYNC, (struct Any* (*)(struct D_Array*))async_new, tokens);
}

Obj p_bracketExpr(List* tokens) {
    List savedTokens = *tokens;
    Obj lhs = p_identifier(tokens);
    if (lhs == NULL) {
        *tokens = savedTokens;
        return NULL;
    }
    Obj lbracket = p_bracketOpen(tokens);
    if (lbracket == NULL) {
        return NULL;
    }
    Obj indexer = p_any_required(tokens);
    p_bracketClose(tokens);
    return (Obj)bracketExpr_new(lhs, indexer);
}

Obj p_cobegin(List* tokens) {
    return p_block(p_COBEGIN, (struct Any* (*)(struct D_Array*))cobegin_new, tokens);
}

Obj p_do(List* tokens) {
    return p_block(p_DO, (struct Any* (*)(struct D_Array*))do_new, tokens);
}

Obj p_if(List* tokens) {
    struct D_Queue* parts = p_seq(tokens, p_IF, p_any_required, p_THEN, p_any_required, p_ELSE, p_any_required, 0);
    if (parts == NULL) {
        return NULL;
    }
    Obj cond = queue_deq_unsafe(parts);
    Obj conseq = queue_deq_unsafe(parts);
    Obj alt = queue_deq_unsafe(parts);
    struct E_If* ifExpr = if_new(cond, conseq, alt);
    return (Obj)ifExpr;
}

Obj p_let(List* tokens) {
    struct D_Queue* parts = p_seq(tokens, p_LET, p_commaBindings_required, 0);
    if (parts == NULL) {
        return NULL;
    }
    struct D_Queue* bindings = (struct D_Queue*)queue_deq_unsafe(parts);
    return (Obj)let_new(queue_asList(bindings), queue_count(bindings));
}

Obj p_letIn(List* tokens) {
    struct D_Queue* parts = p_seq(tokens, p_LET, p_commaBindings_required, p_IN, p_any_required, 0);
    if (parts == NULL) {
        return NULL;
    }
    struct D_Queue* bindings = (struct D_Queue*)queue_deq_unsafe(parts);
    struct Any* expr = queue_deq_unsafe(parts);
    return (Obj)letIn_new(queue_asList(bindings), queue_count(bindings), expr);
}

Obj p_letRec(List* tokens) {
    struct D_Queue* parts = p_seq(tokens, p_LETREC, p_commaBindings_required, 0);
    if (parts == NULL) {
        return NULL;
    }
    struct D_Queue* bindings = (struct D_Queue*)queue_deq_unsafe(parts);
    return (Obj)letRec_new(queue_asList(bindings), queue_count(bindings));
}

Obj p_loop(List* tokens) {
    struct D_Queue* parts = p_seq(tokens, p_LOOP, p_any_required, p_any_required, 0);
    if (parts == NULL) {
        return NULL;
    }
    struct Any* iterExpr = queue_deq_unsafe(parts);
    struct Any* body = queue_deq_unsafe(parts);
    return (Obj)loop_new(iterExpr, body);
}

Obj p_nondet(List* tokens) {
    return p_block(p_NONDET, (struct Any* (*)(struct D_Array*))nondet_new, tokens);
}

Obj p_parenExpr(List* tokens) {
    struct D_Queue* parts = p_seq(tokens, p_parenOpen, p_any_required, p_parenClose_required, 0);
    if (parts == NULL) {
        return NULL;
    }
    return (Obj)queue_deq_unsafe(parts);
}

// for record definition
Obj p_doubleColon(List* tokens) {
    return p_spotValue(tokens, LEXER_SYMBOLS[LTT_Operator], (Obj)DOUBLE_COLON) ? (Obj)IGNORE : NULL;
}

Obj p_typeSpec(List* tokens) {
    struct D_Queue* parts = p_seq(tokens, p_doubleColon, p_symbol, 0);
    if (parts == NULL) {
        return NULL;
    }
    return queue_deq_unsafe(parts);
}

Obj p_recordField(List* tokens) {
    Obj ident = p_identifier(tokens);
    if (ident == NULL) {
        p_fail(tokens, "Expected identifier after 'record'", NULL);
    }
    Obj typeSpec = p_typeSpec(tokens);
    if (typeSpec == NULL) {
        typeSpec = (Obj)NIL;
    }
    Obj initialValue = NULL;
    if (p_equalSign(tokens)) {
        initialValue = p_any(tokens);
        if (initialValue == NULL) {
            p_fail(tokens, "Expected expression after '='", NULL);
        }
    }
    else {
        initialValue = (Obj)NIL;
    }
    Array recordFieldAry = array_newN(3, ident, typeSpec, initialValue);
    return (Obj)recordFieldAry;
}

struct D_Queue* p_recordFields(List* tokens) {
    return p_sepBy(tokens, p_recordField, p_comma);
}

Obj p_recordDef(List* tokens) {
    List savedTokens = *tokens;
    struct D_Queue* parts = p_seq(tokens, p_RECORD, p_symbol_required, p_equalSign_required, p_recordFields, 0);
    if (parts == NULL) {
        *tokens = savedTokens;
        return NULL;
    }
    struct D_Symbol* recordName = (struct D_Symbol*)queue_deq_unsafe(parts);
    struct D_Queue* fieldQueue = (struct D_Queue*)queue_deq_unsafe(parts);
    return (Obj)recordDefinition_new(recordName, fieldQueue, _etor);
}

// for record specification
Obj p_recordSpec(List* tokens) {
    List savedTokens = *tokens;
    struct D_Queue* parts = p_seq(tokens, p_hashMark, p_symbol, p_braceOpen, p_commaBindings, p_braceClose, 0);
    if (parts == NULL) {
        *tokens = savedTokens;
        return NULL;
    }
    struct D_Symbol* recordName = (struct D_Symbol*)queue_deq_unsafe(parts);
    struct D_Queue* fields = (struct D_Queue*)queue_deq_unsafe(parts);
    return (Obj)recordSpec_new(recordName, fields);
}

Obj p_tryCatch(List* tokens) {
    struct D_Queue* parts = p_seq(tokens, p_TRY, p_any_required, p_CATCH, p_funRules, 0);
    if (parts == NULL) {
        return NULL;
    }
    Obj tryExpr = queue_deq_unsafe(parts);
    struct E_Abstraction* catchExpr = (struct E_Abstraction*)queue_deq_unsafe(parts);
    return (Obj)tryCatch_new(tryExpr, catchExpr);
}

// Any expression ====================================================

Obj p_any_required(List* tokens) {
    Obj res = p_any(tokens);
    if (res) {
        return res;
    }
    p_fail(tokens, "Expected expression", NULL);
    return NULL;
}

Obj p_any(List* tokens) {
    struct Any* res = p_oneOf(
        tokens,
        p_parenExpr,
        p_term,
        p_tryCatch,
        p_recordSpec,
        p_recordDef,
        p_bracketExpr,
        p_apply,  // TODO this doesn't work right if it's lower in the list
        p_abstraction,
        p_letIn,  // must precede p_let
        p_let,
        p_letRec,
        p_loop,
        p_if,
        p_do,
        p_async,
        p_cobegin,
        p_nondet,
        p_binding,
        p_integer,
        p_real,
        p_list,
        p_array,
        p_set,
        p_queue,
        p_hashTable,
        p_symbol,
        p_identifier,
        p_nil,
        p_boolean,
        p_string,
        0
    );
    // check for an operator and a RHS expression
    struct D_List* savedTokens = *tokens;
    Obj oper = p_spot(tokens, LEXER_SYMBOLS[LTT_Operator]);
    if (oper == NULL) {
        // this expression is not a binary operator, just return the original result
        *tokens = savedTokens;
        return res;
    }
    Identifier operIdent = (Identifier)oper;
    Obj rhs = p_any_required(tokens);
    return (struct Any*)binOp_new(res, operIdent, rhs);
}

struct D_Queue* p_many(List* tokens) {
    struct D_Queue* exprs = queue_new();
    Obj expr;
    while ((expr = p_any(tokens))) {
        queue_enq(exprs, expr);
    }
    return exprs;
}
