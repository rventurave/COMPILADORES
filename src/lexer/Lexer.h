// src/lexer/Lexer.h
#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <cctype> // Para isalpha, isdigit, isalnum
#include <map>    // Para palabras clave
#include "Token.h" // Incluye la definición de Token y TokenType
#include "../utils/ErrorHandler.h" // Incluye ErrorHandler

// Clase Lexer: Se encarga del análisis léxico (tokenización) del código fuente.
class Lexer {
public:
    // Constructor: Ahora toma una referencia a ErrorHandler
    explicit Lexer(const std::string& sourceCode, ErrorHandler& errorHandler);

    // Método principal para realizar el análisis léxico y devolver una lista de tokens.
    std::vector<Token> tokenize();

private:
    const std::string& sourceCode; // Referencia al código fuente a analizar.
    size_t currentIndex;             // Índice actual en el código fuente.
    int currentLine;               // Número de línea actual.
    int currentCol;                // Número de columna actual.
    ErrorHandler& errorHandler;    // Referencia al manejador de errores.

    std::vector<Token> tokens;     // <--- ¡NUEVO: La lista de tokens generados!

    // Mapa para almacenar las palabras clave y sus TokenType correspondientes.
    std::map<std::string, TokenType> keywords;

    // Métodos auxiliares para el análisis léxico
    char peek(int offset = 0); // Mira el carácter en la posición actual + offset sin avanzar.
    char advance();            // Consume el carácter actual y avanza al siguiente.
    bool isAtEnd();            // Verifica si se ha llegado al final del código fuente.
    void skipWhitespace();     // Salta espacios en blanco y comentarios.
    Token makeToken(TokenType type, const std::string& value); // Crea un token con la información actual.
    void scanToken();          // Escanea el siguiente token y lo añade a 'tokens'.

    // Métodos para escanear tipos específicos de tokens
    Token scanIdentifierOrKeyword();
    Token scanNumber();
    Token scanString();
    // Token scanOperator(); // Ya no es necesario si lo manejas en scanToken switch
};

#endif // LEXER_H