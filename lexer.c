#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

// Token types
typedef enum
{
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MUL,
    TOKEN_DIV,
    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_STRING,
    TOKEN_PRINT,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_WHILE,
    TOKEN_FSTRING_START,
    TOKEN_FSTRING_END,
    TOKEN_FSTRING_VAR,
    TOKEN_IDENTIFIER,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_EQ,
    TOKEN_EQEQ,
    TOKEN_NEQ,
    TOKEN_LT,
    TOKEN_LTE,
    TOKEN_GT,
    TOKEN_GTE,
    TOKEN_EOF
} TokenType;
const char *keywords[] = {"if", "else", "while", "for", "def", "return"};

// Token structure
typedef struct
{
    TokenType type;
    char *value;
} Token;

// Lexer structure
typedef struct
{
    char *src;
    size_t srcLen;
    size_t pos;
    char currentChar;
} Lexer;

// Lexer functions
Lexer *initLexer(char *source);
void advance(Lexer *lexer);
Token *getNextToken(Lexer *lexer);
void lexError(const char *message);
Token *makeToken(TokenType type, char *value);
char *getFStringVar(Lexer *lexer);
void skipWhitespace(Lexer *lexer);
bool isIdentifierChar(char c);
Token *getIdentifier(Lexer *lexer);
Token *getNumber(Lexer *lexer);

Lexer *initLexer(char *source)
{
    Lexer *lexer = (Lexer *)malloc(sizeof(Lexer));
    lexer->src = source;
    lexer->srcLen = strlen(source);
    lexer->pos = 0;
    lexer->currentChar = source[lexer->pos];
    return lexer;
}

void advance(Lexer *lexer)
{
    if (lexer->pos < lexer->srcLen && lexer->currentChar != '\0')
    {
        lexer->pos++;
        lexer->currentChar = lexer->src[lexer->pos];
    }
    else
    {
        lexer->currentChar = '\0';
    }
}

void skipWhitespace(Lexer *lexer)
{
    while (isspace(lexer->currentChar))
    {
        advance(lexer);
    }
}

Token *makeToken(TokenType type, char *value)
{
    Token *token = (Token *)malloc(sizeof(Token));
    token->type = type;
    token->value = value;
    return token;
}

void lexError(const char *message)
{
    printf("Lexer error: %s\n", message);
    exit(1);
}

bool isIdentifierChar(char c)
{
    return isalnum(c) || c == '_';
}

Token *getIdentifier(Lexer *lexer)
{
    char *start = lexer->src + lexer->pos;
    while (isIdentifierChar(lexer->currentChar))
    {
        advance(lexer);
    }
    size_t length = (size_t)(lexer->src + lexer->pos - start);
    char *value = (char *)malloc(length + 1);
    strncpy(value, start, length);
    value[length] = '\0';

    if (strcmp(value, "if") == 0)
        return makeToken(TOKEN_IF, value);
    if (strcmp(value, "else") == 0)
        return makeToken(TOKEN_ELSE, value);
    if (strcmp(value, "while") == 0)
        return makeToken(TOKEN_WHILE, value);
    if (strcmp(value, "print") == 0)
        return makeToken(TOKEN_PRINT, value);

    return makeToken(TOKEN_IDENTIFIER, value);
}

Token *getNumber(Lexer *lexer)
{
    char *start = lexer->src + lexer->pos;
    bool hasDot = false;

    while (isdigit(lexer->currentChar) || lexer->currentChar == '.')
    {
        if (lexer->currentChar == '.')
        {
            if (hasDot)
                break;
            hasDot = true;
        }
        advance(lexer);
    }

    size_t length = (size_t)(lexer->src + lexer->pos - start);
    char *value = (char *)malloc(length + 1);
    strncpy(value, start, length);
    value[length] = '\0';

    if (hasDot)
        return makeToken(TOKEN_FLOAT, value);
    else
        return makeToken(TOKEN_INT, value);
}

Token *getNextToken(Lexer *lexer)
{
    while (lexer->currentChar != '\0')
    {
        if (isspace(lexer->currentChar))
        {
            skipWhitespace(lexer);
            continue;
        }

        if (isalpha(lexer->currentChar))
        {
            return getIdentifier(lexer);
        }

        if (isdigit(lexer->currentChar))
        {
            return getNumber(lexer);
        }

        switch (lexer->currentChar)
        {
        case '+':
            advance(lexer);
            return makeToken(TOKEN_PLUS, strdup("+")); // plus 
        case '-':
            advance(lexer);
            return makeToken(TOKEN_MINUS, strdup("-")); // minus
        case '*':
            advance(lexer);
            return makeToken(TOKEN_MUL, strdup("*")); // multiply
        case '/':
            advance(lexer);
            return makeToken(TOKEN_DIV, strdup("/"));  // divide 
        case '(':
            advance(lexer);
            return makeToken(TOKEN_LPAREN, strdup("("));  // left bracket 
        case ')':
            advance(lexer);
            return makeToken(TOKEN_RPAREN, strdup(")"));  // right bracker
        case '{':
            advance(lexer);
            return makeToken(TOKEN_LBRACE, strdup("{"));  // left brace
        case '}':
            advance(lexer);
            return makeToken(TOKEN_RBRACE, strdup("}")); // right brace
        case '<':
            advance(lexer);
            return makeToken(TOKEN_LTE, strdup("<"));  // less than 
        case '>':
            advance(lexer);
            return makeToken(TOKEN_GTE, strdup(">"));  // greater than
        case '=':
            advance(lexer);
            if (lexer->currentChar == '=')
            {
                advance(lexer);
                return makeToken(TOKEN_EQEQ, strdup("=="));
            }
            return makeToken(TOKEN_EQ, strdup("="));
        case '!':
            advance(lexer);
            if (lexer->currentChar == '=')
            {
                advance(lexer);
                return makeToken(TOKEN_NEQ, strdup("!="));
            }
            lexError("Unexpected character '!' without '='");
            break;
            // Additional cases as needed...
        }

        lexError("Unknown token");
    }

    return makeToken(TOKEN_EOF, "\0");
}

void skipComment(Lexer *lexer)
{
    if (lexer->currentChar == '/')
    {
        advance(lexer);
        if (lexer->currentChar == '/')
        {
            while (lexer->currentChar != '\n' && lexer->currentChar != '\0')
            {
                advance(lexer);
            }
        }
    }
}

Token *getString(Lexer *lexer)
{
    advance(lexer); // Skip the initial quote
    char *start = lexer->src + lexer->pos;
    while (lexer->currentChar != '"' && lexer->currentChar != '\0')
    {  
        // Handle escape sequences here if needed
        advance(lexer);
    }
    if (lexer->currentChar == '\0')
    {
        lexError("Unterminated string literal");
    }
    size_t length = (size_t)(lexer->src + lexer->pos - start);
    char *value = (char *)malloc(length + 1);
    strncpy(value, start, length);
    value[length] = '\0';
    advance(lexer); // Skip the closing quote

    return makeToken(TOKEN_STRING, value);
}

// testing example 1

int main()
{
    char *source = "if x < 10 { print \"x is less than 10\" }";
    Lexer *lexer = initLexer(source);
    Token *token;

    while ((token = getNextToken(lexer))->type != TOKEN_EOF)
    {
        printf("Token: Type: %d, Value: %s\n", token->type, token->value);
        free(token->value); // Remember to free the allocated string
        free(token);        // Free the token itself
    }

    free(lexer); // Cleanup the lexer
    return 0;
}


// examople 2 for testing

// int main()
// {
//     char *text = "var_name = 100 > 20;";
//     Lexer *lexer = initLexer(text);

//     Token *token = NULL;
//     do
//     {
//         token = getNextToken(lexer);
//         printf("Token: Type: %d, Value: %s\n", token->type, token->value);
//         free(token->value);
//     } while (token->type != TOKEN_EOF);

//     free(lexer);

//     return 0;
// }