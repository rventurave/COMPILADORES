#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <iostream>

// Enumeración de los tipos de tokens que puede reconocer el lexer
enum class TokenType {
    // Palabras clave
    KEYWORD_INT, KEYWORD_VOID, KEYWORD_IF, KEYWORD_ELSE, KEYWORD_FOR, KEYWORD_RETURN, KEYWORD_PRINTF,

    // Identificadores
    IDENTIFIER,

    // Literales
    INTEGER_LITERAL, STRING_LITERAL,

    //
    AMPERSAND,
    // Operadores
    PLUS, MINUS, MULTIPLY, DIVIDE, ASSIGN,
    LESS_THAN, GREATER_THAN, EQUAL_EQUAL, LESS_EQUAL, GREATER_EQUAL, NOT_EQUAL,

    // Delimitadores y puntuación
    LPAREN, RPAREN, LBRACE, RBRACE, SEMICOLON, COMMA,

    // Fin de archivo
    END_OF_FILE,

    // Error
    UNKNOWN
};

// Estructura que representa un token léxico
struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;

    // Constructor
    Token(TokenType type, std::string value, int line, int column)
        : type(type), value(std::move(value)), line(line), column(column) {}

    // Constructor por defecto
    Token() : type(TokenType::UNKNOWN), value(""), line(0), column(0) {}
};

#endif // TOKEN_H