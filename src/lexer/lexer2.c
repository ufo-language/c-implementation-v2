#include <stdlib.h>
#include <string.h>

#include "data/any.h"
#include "data/array.h"
#include "data/integer.h"
#include "data/queue.h"
#include "data/real.h"
#include "data/string.h"
#include "data/stringbuffer.h"
#include "data/stringstream.h"
#include "data/symbol.h"
#include "expr/identifier.h"
#include "main/globals.h"

static bool contains(char* str, char c);
static bool isReserved(char* word);
static struct D_Queue* tokenize_aux(struct D_StringStream* inputString);

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

enum TokenType {
    TT_None,
    TT_Integer,
    TT_Operator,
    TT_Real,
    TT_Special,
    TT_String,
    TT_Symbol,
    TT_Word,
    TT_EOI
};

struct D_Array* makeToken(enum TokenType tokenType, struct D_StringBuffer* lexeme, int pos, int col, int line) {
    struct D_String* lexemeString = stringBuffer_asString(lexeme);
    stringBuffer_clear(lexeme);
    char* lexemeChars = string_getChars(lexemeString);
    //int lexemeLen = string_count(lexemeString);
    char* tokenTypeName = NULL;
    struct Any* token = NULL;
    switch (tokenType) {
        case TT_Integer:
            tokenTypeName = "Integer";
            {    
                int intValue = atoi(lexemeChars);
                token = (struct Any*)integer_new(intValue);
            }
            break;
        case TT_Real:
            tokenTypeName = "Real";
            {
                double doubleValue = atof(lexemeChars);
                token = (struct Any*)real_new(doubleValue);
            }
            break;
        case TT_Special:
            tokenTypeName = "Special";
            token = (struct Any*)lexemeString;
            break;
        case TT_String:
            tokenTypeName = "String";
            token = (struct Any*)string_new(lexemeChars);
            break;
        case TT_Symbol:
            tokenTypeName = "Symbol";
            token = (struct Any*)symbol_new(lexemeChars);
            break;
        case TT_Operator:
            tokenTypeName = "Operator";
            token = (struct Any*)identifier_new(lexemeChars);
            break;
        case TT_Word:
            if (isReserved(lexemeChars)) {
                tokenTypeName = "Reserved";
                token = (struct Any*)lexemeString;
            }
            else {
                tokenTypeName = "Identifier";
                token = (struct Any*)identifier_new(lexemeChars);
            }
            break;
        case TT_EOI:
            tokenTypeName = "EOI";
            token = (struct Any*)NIL;
            break;
        case TT_None:
            break;
        default:
            printf("%s got unhandled token type %d\n", __func__, tokenType);
    }
    return array_newN(5, symbol_new(tokenTypeName), token, integer_new(pos), integer_new(col), integer_new(line));
}

struct D_List* lexer2_tokenize(struct D_String* inputString) {
    char* chars = string_getChars(inputString);
    struct D_StringStream* inputStream = stringStream_new(chars);
    struct D_Queue* tokenQueue = tokenize_aux(inputStream);
    return queue_asList(tokenQueue);
}

static struct D_Queue* tokenize_aux(struct D_StringStream* inputString) {
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
        enum TokenType tokenType = TT_None;
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
                    tokenType = TT_Special;
                    accumulateChar = true;
                }
                break;

            case S_Word:  // 1
                if      (chr >= 'A' && chr <= 'Z') { accumulateChar = true; }
                else if (chr >= 'a' && chr <= 'z') { accumulateChar = true; }
                else if (chr >= '0' && chr <= '9') { accumulateChar = true; }
                else if (chr == '_')               { accumulateChar = true; }
                else {
                    tokenType = TT_Word;
                    ungetChar = chr;
                }
                break;

            case S_Number:  // 2
                if      (chr >= '0' && chr <= '9') { accumulateChar = true; }
                else if (chr == '.') { accumulateChar = true; state = S_Dot; }
                else {
                    tokenType = TT_Integer;
                    ungetChar = chr;
                }
                break;

            case S_Symbol:  // 3
                if      (chr >= 'A' && chr <= 'Z') { accumulateChar = true; }
                else if (chr >= 'a' && chr <= 'z') { accumulateChar = true; }
                else if (chr >= '0' && chr <= '9') { accumulateChar = true; }
                else if (chr == '_')               { accumulateChar = true; }
                else {
                    tokenType = TT_Symbol;
                    ungetChar = chr;
                }
                break;

            case S_String:  // 4
                if      (chr == '\\') { state = S_EscapedChar; }
                else if (chr == '"')  { tokenType = TT_String; }
                else if (chr == '\0') {
                    printf("lexer.c string error starting at pos %d: col %d, line %d\n", pos, col, line);
                    //tokenType = TT_Error;
                }
                else { accumulateChar = true; }
                break;

            case S_EscapedChar:  // 5
                accumulateChar = true;
                state = S_String;
                break;

            case S_Operator:  // 6
                if (contains(operChars, chr)) { accumulateChar = true; }
                else {
                    tokenType = TT_Operator;
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
                    tokenType = TT_Real;
                    stringBuffer_writeChar(lexeme, '0');
                    ungetChar = chr;
                }
                break;

            case S_Real:  // 10
                if (chr >= '0' && chr <= '9')  { accumulateChar = true; }
                else {
                    tokenType = TT_Real;
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
        if (tokenType != TT_None) {
            struct D_Array* token = makeToken(tokenType, lexeme, pos, col, line);
            queue_enq(tokens, (struct Any*)token);
            state = S_Initial;
        }
    }
    struct D_Array* token = makeToken(TT_EOI, lexeme, pos, col, line);
    queue_enq(tokens, (struct Any*)token);
    return tokens;
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
