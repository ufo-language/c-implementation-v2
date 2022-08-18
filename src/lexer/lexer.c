#include <stdlib.h>
#include <string.h>

#include "data/any.h"
#include "data/array.h"
#include "data/binding.h"
#include "data/boolean.h"
#include "data/integer.h"
#include "data/queue.h"
#include "data/real.h"
#include "data/string.h"
#include "data/stringbuffer.h"
#include "data/stringstream.h"
#include "data/symbol.h"
#include "expr/identifier.h"
#include "lexer/lexer.h"
#include "main/globals.h"

static bool contains(char* str, char c);
static bool isReserved(char* word);
static struct Any* tokenize_aux(struct D_StringStream* inputString);

static char* operChars = ".:+-*/=%";
static char* ignoreChars = " \n\r\t";
static char* reservedWords[] = {
    "catch",
    "do",
    "else",
    "end",
    "fun",
    "if",
    "in",
    "let",
    "letrec",
    "macro",
    "record",
    "then",
    "try",
    0
};

struct D_Symbol* LEXER_SYMBOLS[LTT_FINAL];

static char* TRUE_WORD;
static char* FALSE_WORD;
static char* NIL_WORD;

void lexer_rootObjects(void) {
    LEXER_SYMBOLS[LTT_None]       = symbol_new("None");
    LEXER_SYMBOLS[LTT_Boolean]    = symbol_new("Boolean");
    LEXER_SYMBOLS[LTT_Nil]        = symbol_new("Nil");
    LEXER_SYMBOLS[LTT_Identifier] = symbol_new("Identifier");
    LEXER_SYMBOLS[LTT_Integer]    = symbol_new("Integer");
    LEXER_SYMBOLS[LTT_Operator]   = symbol_new("Operator");
    LEXER_SYMBOLS[LTT_Real]       = symbol_new("Real");
    LEXER_SYMBOLS[LTT_Reserved]   = symbol_new("Reserved");
    LEXER_SYMBOLS[LTT_Special]    = symbol_new("Special");
    LEXER_SYMBOLS[LTT_String]     = symbol_new("String");
    LEXER_SYMBOLS[LTT_Symbol]     = symbol_new("Symbol");
    LEXER_SYMBOLS[LTT_Word]       = symbol_new("Word");
    LEXER_SYMBOLS[LTT_EOI]        = symbol_new("EOI");
}

void lexer_initialize(void) {
    NIL_WORD = "nil";
    TRUE_WORD = "true";
    FALSE_WORD = "false";
}

struct D_Array* makeToken(enum LexerTokenType tokenType, struct D_StringBuffer* lexeme, int pos, int col, int line) {
    struct D_String* lexemeString = stringBuffer_asString(lexeme);
    stringBuffer_clear(lexeme);
    char* lexemeChars = string_getChars(lexemeString);
    struct D_Symbol* tokenTypeSym = NULL;
    struct Any* token = NULL;
    switch (tokenType) {
        case LTT_Integer:
            tokenTypeSym = LEXER_SYMBOLS[LTT_Integer];
            {    
                int intValue = atoi(lexemeChars);
                token = (struct Any*)integer_new(intValue);
            }
            break;
        case LTT_Real:
            tokenTypeSym = LEXER_SYMBOLS[LTT_Real];
            {
                double doubleValue = atof(lexemeChars);
                token = (struct Any*)real_new(doubleValue);
            }
            break;
        case LTT_Special:
            tokenTypeSym = LEXER_SYMBOLS[LTT_Special];
            token = (struct Any*)lexemeString;
            break;
        case LTT_String:
            tokenTypeSym = LEXER_SYMBOLS[LTT_String];
            token = (struct Any*)string_new(lexemeChars);
            break;
        case LTT_Symbol:
            tokenTypeSym = LEXER_SYMBOLS[LTT_Symbol];            
            token = (struct Any*)symbol_new(lexemeChars);
            break;
        case LTT_Operator:
            tokenTypeSym = LEXER_SYMBOLS[LTT_Operator];
            token = (struct Any*)identifier_new(lexemeChars);
            break;
        case LTT_Word:
            if (!strcmp(TRUE_WORD, lexemeChars)) {
                tokenTypeSym = LEXER_SYMBOLS[LTT_Boolean];
                token = (struct Any*)boolean_from(true);
            }
            else if (!strcmp(FALSE_WORD, lexemeChars)) {
                tokenTypeSym = LEXER_SYMBOLS[LTT_Boolean];
                token = (struct Any*)boolean_from(false);
            }
            else if (!strcmp(NIL_WORD, lexemeChars)) {
                tokenTypeSym = LEXER_SYMBOLS[LTT_Nil];
                token = (struct Any*)NIL;
            }
            else if (isReserved(lexemeChars)) {
                tokenTypeSym = LEXER_SYMBOLS[LTT_Reserved];
                token = (struct Any*)lexemeString;
            }
            else {
                tokenTypeSym = LEXER_SYMBOLS[LTT_Identifier];
                token = (struct Any*)identifier_new(lexemeChars);
            }
            break;
        case LTT_EOI:
            tokenTypeSym = LEXER_SYMBOLS[LTT_EOI];
            token = (struct Any*)NIL;
            break;
        case LTT_None:
        case LTT_Boolean:
        case LTT_Nil:
        case LTT_Identifier:
        case LTT_Reserved:
        case LTT_FINAL:
            printf("%s got unhandled token type %d\n", __func__, tokenType);
    }
    return array_newN(5, tokenTypeSym, token, integer_new(pos), integer_new(col), integer_new(line));
}

struct Any* lexer_tokenize(struct D_String* inputString) {
    char* chars = string_getChars(inputString);
    struct D_StringStream* inputStream = stringStream_new(chars);
    struct Any* res = tokenize_aux(inputStream);
    if (res->typeId == T_Queue) {
        return (struct Any*)queue_asList((struct D_Queue*)res);
    }
    return res;
}

// Returns a D_Queue on success, a D_Array on error
static struct Any* tokenize_aux(struct D_StringStream* inputString) {
    enum StateId {
        S_Initial,     // 0
        S_Word,        // 1
        S_Number,      // 2
        S_Symbol,      // 3
        S_String,      // 4
        S_EscapedChar, // 5
        S_Operator,    // 6
        S_Plus,        // 7
        S_Minus,       // 8
        S_Dot,         // 9
        S_Real         // 10
    };
    enum StateId state = S_Initial;
    struct D_Queue* tokens = queue_new();
    struct D_StringBuffer* lexeme = stringBuffer_new();
    int pos, col, line;
    bool continueLoop = true;
    int chr;
    int ungetChar = -1;
    while (continueLoop) {
        // get the next character, or the last un-got character
        if (ungetChar > -1) {
            chr = ungetChar;
            ungetChar = -1;
        }
        else {
            chr = stringStream_get(inputString);
        }
        bool accumulateChar = false;
        enum LexerTokenType tokenType = LTT_None;
        // handle the character
        switch (state) {
            case S_Initial:  // 0
                stringStream_getPos(inputString, &pos, &col, &line);
                if      (chr >= 'A' && chr <= 'Z')   { accumulateChar = true; state = S_Symbol; }
                else if (chr >= 'a' && chr <= 'z')   { accumulateChar = true; state = S_Word; }
                else if (chr >= '0' && chr <= '9')   { accumulateChar = true; state = S_Number; }
                else if (chr == '+')                 { state = S_Plus; }
                else if (chr == '-')                 { state = S_Minus; }
                else if (chr == '"')                 { state = S_String; }
                else if (contains(operChars, chr))   { accumulateChar = true; state = S_Operator; }
                else if (contains(ignoreChars, chr)) { }
                else if (chr == '\0')                { continueLoop = false; }
                else {
                    tokenType = LTT_Special;
                    accumulateChar = true;
                }
                break;

            case S_Word:  // 1
                if      (chr >= 'A' && chr <= 'Z') { accumulateChar = true; }
                else if (chr >= 'a' && chr <= 'z') { accumulateChar = true; }
                else if (chr >= '0' && chr <= '9') { accumulateChar = true; }
                else if (chr == '_')               { accumulateChar = true; }
                else {
                    tokenType = LTT_Word;
                    ungetChar = chr;
                }
                break;

            case S_Number:  // 2
                if      (chr >= '0' && chr <= '9') { accumulateChar = true; }
                else if (chr == '.') { accumulateChar = true; state = S_Dot; }
                else {
                    tokenType = LTT_Integer;
                    ungetChar = chr;
                }
                break;

            case S_Symbol:  // 3
                if      (chr >= 'A' && chr <= 'Z') { accumulateChar = true; }
                else if (chr >= 'a' && chr <= 'z') { accumulateChar = true; }
                else if (chr >= '0' && chr <= '9') { accumulateChar = true; }
                else if (chr == '_')               { accumulateChar = true; }
                else {
                    tokenType = LTT_Symbol;
                    ungetChar = chr;
                }
                break;

            case S_String:  // 4
                if      (chr == '\\') { state = S_EscapedChar; }
                else if (chr == '"')  { tokenType = LTT_String; }
                else if (chr == '\0') {
                    struct D_Binding* posBinding = binding_new((struct Any*)symbol_new("Pos"), (struct Any*)integer_new(pos));
                    struct D_Binding* lineBinding = binding_new((struct Any*)symbol_new("Line"), (struct Any*)integer_new(line));
                    struct D_Binding* colBinding = binding_new((struct Any*)symbol_new("Col"), (struct Any*)integer_new(col));
                    struct D_Array* exn = array_newN(3,
                                                     (struct Any*)symbol_new("Lexer"),
                                                     (struct Any*)string_new("unterminated string"),
                                                     (struct Any*)array_newN(3,
                                                                             (struct Any*)posBinding,
                                                                             (struct Any*)lineBinding,
                                                                             (struct Any*)colBinding));
                    return (struct Any*)exn;
                }
                else { accumulateChar = true; }
                break;

            case S_EscapedChar:  // 5
                switch (chr) {
                    case 'n': chr = '\n'; break;
                    case 'r': chr = '\r'; break;
                    case 't': chr = '\t'; break;
                    case '0': chr = '\0'; break;
                    default:
                        break;
                }
                accumulateChar = true;
                state = S_String;
                break;

            case S_Operator:  // 6
                if (contains(operChars, chr)) { accumulateChar = true; }
                else {
                    tokenType = LTT_Operator;
                    ungetChar = chr;
                }
                break;

            case S_Plus:  // 7
                if (chr >= '0' && chr <= '9') { accumulateChar = true; state = S_Number; }
                else {
                    stringBuffer_writeChar(lexeme, '+');
                    ungetChar = chr;
                    state = S_Operator;
                }
                break;

            case S_Minus:  // 8
                if (chr >= '0' && chr <= '9') { stringBuffer_writeChar(lexeme, '-'); accumulateChar = true; state = S_Number; }
                else {
                    stringBuffer_writeChar(lexeme, '-');
                    ungetChar = chr;
                    state = S_Operator;
                }
                break;

            case S_Dot:  // 9
                if (chr >= '0' && chr <= '9')  { accumulateChar = true; state = S_Real; }
                else {
                    tokenType = LTT_Real;
                    stringBuffer_writeChar(lexeme, '0');
                    ungetChar = chr;
                }
                break;

            case S_Real:  // 10
                if (chr >= '0' && chr <= '9')  { accumulateChar = true; }
                else {
                    tokenType = LTT_Real;
                    ungetChar = chr;
                }
                break;

            default:
                printf("%s: illegal state %d\n", __func__, state);
                continueLoop = false;
        }
        if (accumulateChar) {
            stringBuffer_writeChar(lexeme, chr);
        }
        if (tokenType != LTT_None) {
            struct D_Array* token = makeToken(tokenType, lexeme, pos, col, line);
            queue_enq(tokens, (struct Any*)token);
            state = S_Initial;
        }
    }
    struct D_Array* token = makeToken(LTT_EOI, lexeme, pos, col, line);
    queue_enq(tokens, (struct Any*)token);
    return (struct Any*)tokens;
}

// support functions =================================================

// determines if a character is in a string
static bool contains(char* str, char c) {
    // The strchr function will return a positive result if c == '\0'
    // because it will match the null terminator of the string;
    if (c == '\0') {
        return false;
    }
    return strchr(str, c) != NULL;
}

static bool isReserved(char* word) {
    for (int n=0; reservedWords[n]; n++) {
        char* reservedWord = reservedWords[n];
        if (!strcmp(reservedWord, word)) {
            return true;
        }
    }
    return false;
}
