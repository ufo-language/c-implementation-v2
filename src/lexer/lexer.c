#include <setjmp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer/lexer.h"

static char* operChars = ".:+-*/=";

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

static int nothingLen;
static int trueLen;
static int falseLen;

static bool isReserved(struct ResultToken* token);

static char getChar(struct LexerState2* lexerState, struct ResultToken* token);
static void ungetChar(struct LexerState2* lexerState, struct ResultToken* token);
static void ignoreChar(struct LexerState2* lexerState, struct ResultToken* token);
static void nextLine(struct LexerState2* lexerState, struct ResultToken* token);

static void makeEOI(struct ResultToken* token);
static void makeInteger(struct ResultToken* token);
static void makeOperator(struct ResultToken* token);
static void makeReal(struct ResultToken* token);
static void makeSpecial(struct ResultToken* token);
static void makeString(struct ResultToken* token);
static void makeSymbol(struct ResultToken* token);
static void makeWord(struct ResultToken* token);

static double strToRealN(char* str, int nChars);
static int strToIntN(char* str, int nChars);

static jmp_buf jumpBuf;

char* NOTHING_WORD;
char* TRUE_WORD;
char* FALSE_WORD;

void lexer_initialize(void) {
    NOTHING_WORD = "nothing";
    TRUE_WORD = "true";
    FALSE_WORD = "false";
    nothingLen = strlen(NOTHING_WORD);
    trueLen = strlen(TRUE_WORD);
    falseLen = strlen(FALSE_WORD);
}

// determines if a character is in a string
bool isIn(char c, char* chars) {
    // The strchr function will return a positive result if c == '\0'
    // because it will match the null terminator of the string;
    if (c == '\0') {
        return false;
    }
    return strchr(chars, c) != NULL;
}

void lex_nextToken(struct LexerState2* lexerState, struct ResultToken* token) {
    int jumpRes = setjmp(jumpBuf);
    if (jumpRes > 0) {
        return;
    }
    enum StateId {
        S_Initial,
        S_Word,
        S_Number,
        S_Symbol,
        S_String,
        S_Operator,
        S_PlusMinus,
        S_Dot
    };
    enum StateId state = S_Initial;
    while (true) {
        if (state == S_Initial) {
            // this must be done before the call to getChar()
            token->pos = lexerState->pos;
            token->col = lexerState->col;
            token->line = lexerState->line;
        }
        char c = getChar(lexerState, token);
        switch(state) {
        case S_Initial:
            if      (c >= 'A' && c <= 'Z')  { state = S_Symbol; }
            else if (c >= 'a' && c <= 'z')  { state = S_Word; }
            else if (c == '_')              { state = S_Word; }
            else if (c == '-' || c == '+')  { state = S_PlusMinus; }
            else if (c >= '0' && c <= '9')  { state = S_Number; }
            else if (c == ' ' || c == '\t') { ignoreChar(lexerState, token); }
            else if (c == '\0')             { makeEOI(token); return; }
            else if (strchr(operChars, c))  { state = S_Operator; }
            else if (c == '"')              { state = S_String; }
            else if (c == '\n')             { ignoreChar(lexerState, token); nextLine(lexerState, token); }
            else {
                makeSpecial(token);
                return;
            }
            break;
        case S_Symbol:
            if      (c >= 'A' && c <= 'Z')  {}
            else if (c >= 'a' && c <= 'z')  {}
            else if (c >= '0' && c <= '9')  {}
            else if (c == '_')              {}
            else {
                ungetChar(lexerState, token);
                makeSymbol(token);
                return;
            }
            break;
        case S_Word:
            if      (c >= 'A' && c <= 'Z')  {}
            else if (c >= 'a' && c <= 'z')  {}
            else if (c >= '0' && c <= '9')  {}
            else if (c == '_')              {}
            else {
                ungetChar(lexerState, token);
                makeWord(token);
                return;
            }
            break;
        case S_Number:
            if      (c >= '0' && c <= '9')  {}
            else if (c == '.')              { state = S_Dot; }
            else {
                ungetChar(lexerState, token);
                makeInteger(token);
                return;
            }
            break;
        case S_Dot:
            if      (c >= '0' && c <= '9')  {}
            else {
                ungetChar(lexerState, token);
                makeReal(token);
                return;
            }
            break;
        case S_Operator:
            if (isIn(c, operChars)) {}
            else {
                ungetChar(lexerState, token);
                makeOperator(token);
                return;
            }
            break;
        case S_String:
            if (c == '"') {
                // terminate the string
                // do not unget (consumes closing quote)
                makeString(token);
                return;
            }
            else if (c == '\0') {
                token->error = "string error";
                longjmp(jumpBuf, 1);
            }
            else {
                // accept any character
            }
            break;
        case S_PlusMinus:
            if (c >= '0' && c <= '9') { state = S_Number; }
            if (c == '\0') {
                ungetChar(lexerState, token);
                state = S_Initial;
            }
            else {
                ungetChar(lexerState, token);
                state = S_Operator;
            }
            break;
        default:
            fprintf(stderr, "%s unhandled case %d\n", __func__, state);
            exit(1);
        }
    }
}

static char getChar(struct LexerState2* lexerState, struct ResultToken* token) {
    token->strLen++;
    char c = *lexerState->inputString++;
    token->charValue = c;
    // increment character position
    lexerState->pos++;
    lexerState->col++;
    return c;
}

static void ungetChar(struct LexerState2* lexerState, struct ResultToken* token) {
    lexerState->inputString--;
    lexerState->pos--;
    lexerState->col--;
    token->strLen--;
    if ('\n' == *lexerState->inputString) {
        lexerState->line--;
    }
}

static void nextLine(struct LexerState2* lexerState, struct ResultToken* token) {
    (void)lexerState;
    (void)token;
    lexerState->line++;
    lexerState->col = 0;
}

static void ignoreChar(struct LexerState2* lexerState, struct ResultToken* token) {
    token->strValue = lexerState->inputString;
    token->strLen = 0;
}

static bool isReserved(struct ResultToken* token) {
    char* word = token->strValue;
    int count  = token->strLen;
    for (int n=0; reservedWords[n]; n++) {
        char* reservedWord = reservedWords[n];
        if (!strncmp(reservedWord, word, count) && reservedWord[count] == 0) {
            return true;
        }
    }
    return false;
}

static void makeEOI(struct ResultToken* token) {
    token->tokenType = LT_EOI;
    token->strValue = NULL;
    token->strLen = 0;
}

static void makeOperator(struct ResultToken* token) {
    token->tokenType = LT_Operator;
}

static void makeWord(struct ResultToken* token) {
    char* word = token->strValue;
    int count  = token->strLen;
    if (count == nothingLen && !strncmp(NOTHING_WORD, word, count)) {
        token->tokenType = LT_Nothing;
    }
    else if (count == trueLen && !strncmp(TRUE_WORD, word, count)) {
        token->tokenType = LT_Boolean;
    }
    else if (count == falseLen && !strncmp(FALSE_WORD, word, count)) {
        token->tokenType = LT_Boolean;
    }
    else if (isReserved(token)) {
        token->tokenType = LT_Reserved;
    }
    else {
        token->tokenType = LT_Identifier;
    }
}

static void makeInteger(struct ResultToken* token) {
    token->tokenType = LT_Integer;
    token->intValue = strToIntN(token->strValue, token->strLen);;
}

static void makeReal(struct ResultToken* token) {
    token->tokenType = LT_Real;
    token->doubleValue = strToRealN(token->strValue, token->strLen);;
}

static void makeSpecial(struct ResultToken* token) {
    token->tokenType = LT_Special;
}

static void makeString(struct ResultToken* token) {
    // skip over leading quote
    token->strValue++;
    // ignore leading and trailing quotes
    token->strLen -= 2;
    token->tokenType = LT_String;
}

static void makeSymbol(struct ResultToken* token) {
    token->tokenType = LT_Symbol;
}

// This converts the first nChars characters in str into an int.
static int strToIntN(char* str, int nChars) {
    int num = 0;
    for (int n=0; n<nChars; n++) {
        num = num * 10 + (str[n] - '0');
    }
    return num;
}

// This converts the first nChars characters in str into a double.
static double strToRealN(char* str, int nChars) {
    double num = 0;
    char c;
    int index = 0;
    while ((c = str[index]) != '.') {
        num = num * 10.0 + (c - '0');
        nChars--;
        index++;
    }
    nChars--;
    index++;
    double place = 0.1;
    while (nChars > 0) {
        c = str[index];
        num += place * (c - '0');
        place /= 10.0;
        nChars--;
        index++;
    }
    return num;
}
