#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data/any.h"
#include "data/array.h"
#include "data/integer.h"
#include "data/queue.h"
#include "data/real.h"
#include "data/string.h"
#include "data/symbol.h"
#include "expr/identifier.h"
#include "lexer/lexer.h"
#include "main/globals.h"

struct D_Symbol* LEXER_SYMBOLS[LT_FINAL];

static int nilLen;
static int trueLen;
static int falseLen;

void parser_initialize(void) {
    nilLen = strlen("nil");
    trueLen = strlen("true");
    falseLen = strlen("false");
}

void lexObj_rootObjects(void) {
    LEXER_SYMBOLS[LT_None]       = symbol_new("None");
    LEXER_SYMBOLS[LT_Boolean]    = symbol_new("Boolean");
    LEXER_SYMBOLS[LT_Nil]        = symbol_new("Nil");
    LEXER_SYMBOLS[LT_Identifier] = symbol_new("Identifier");
    LEXER_SYMBOLS[LT_Integer]    = symbol_new("Integer");
    LEXER_SYMBOLS[LT_Operator]   = symbol_new("Operator");
    LEXER_SYMBOLS[LT_Real]       = symbol_new("Real");
    LEXER_SYMBOLS[LT_Reserved]   = symbol_new("Reserved");
    LEXER_SYMBOLS[LT_Special]    = symbol_new("Special");
    LEXER_SYMBOLS[LT_String]     = symbol_new("String");
    LEXER_SYMBOLS[LT_Symbol]     = symbol_new("Symbol");
    LEXER_SYMBOLS[LT_EOI]        = symbol_new("EOI");
}

struct D_List* lexObj_string(struct D_String* string) {
    struct D_Queue* tokenQueue = queue_new();
    struct LexerState lexerState = {
        /* inputString */ string_getChars(string),
        /* pos         */ 0,
        /* col         */ 0,
        /* line        */ 1,
        /* prevChar    */ 0
    };
    struct ResultToken token = {
        /* tokenType   */ LT_None,
        /* error       */ NULL,
        /* pos         */ 0,
        /* col         */ 0,
        /* line        */ 1,
        /* lexeme      */ lexerState.inputString,
        /* lexemeLen   */ 0,
        /* intValue    */ 0,
        /* charValue   */ 0,
        /* doubleValue */ 0.0
    };
    bool contin = true;
    while (contin) {
        token.tokenType = LT_None;
        token.error = NULL;
        token.lexeme = lexerState.inputString;
        token.lexemeLen = 0;
        token.intValue = 0;
        token.charValue = '\0';
        token.doubleValue = 0.0;
        lex_nextToken(&lexerState, &token);
        if (token.error) {
            fprintf(stderr, "ERROR: %s\n", token.error);
            return NULL;
        }
        struct Any* lexemeObj;
        switch (token.tokenType) {
            case LT_EOI:
                lexemeObj = (struct Any*)NIL;
                contin = false;
                break;
            case LT_Boolean:
                lexemeObj = (struct Any*)(strncmp("true", token.lexeme, 4) ? FALSE : TRUE);
                break;
            case LT_Nil:
                lexemeObj = (struct Any*)NIL;
                break;
            case LT_Operator: {
                    int len = token.lexemeLen;
                    char* lexeme = (char*)malloc(len + 1);
                    strncpy(lexeme, token.lexeme, len);
                    lexeme[len] = 0;
                    lexemeObj = (struct Any*)identifier_new_move(lexeme, len);
                }
                break;
            case LT_Identifier: {
                    int len = token.lexemeLen;
                    char* lexeme = (char*)malloc(len + 1);
                    strncpy(lexeme, token.lexeme, len);
                    lexeme[len] = 0;
                    lexemeObj = (struct Any*)identifier_new_move(lexeme, len);
                }
                break;
            case LT_Integer:
                lexemeObj = (struct Any*)integer_new(token.intValue);
                break;
            case LT_Real:
                lexemeObj = (struct Any*)real_new(token.doubleValue);
                break;
            case LT_String:
            case LT_Special:
            case LT_Reserved: {
                    int len = token.lexemeLen;
                    char* lexeme = (char*)malloc(len + 1);
                    strncpy(lexeme, token.lexeme, len);
                    lexeme[len] = 0;
                    lexemeObj = (struct Any*)string_new_move(lexeme, len);
                }
                break;
            case LT_Symbol: {
                    int len = token.lexemeLen;
                    char* lexeme = (char*)malloc(len + 1);
                    strncpy(lexeme, token.lexeme, len);
                    lexeme[len] = 0;
                    lexemeObj = (struct Any*)symbol_new_move(lexeme, len);
                }
                break;
            case LT_None:  // should never happen
            case LT_FINAL:
            default:
                fprintf(stderr, "%s got unhandled case %d\n", __func__, token.tokenType);
                exit(1);
        }

        struct D_Symbol* tokenType = LEXER_SYMBOLS[token.tokenType];
        struct D_Integer* pos = integer_new(token.pos);
        struct D_Integer* col = integer_new(token.col);
        struct D_Integer* line = integer_new(token.line);
        struct D_Array* tokenArray = array_newN(5, tokenType, lexemeObj, pos, col, line);
        queue_enq(tokenQueue, (struct Any*)tokenArray);
    }
    return queue_asList(tokenQueue);
}
