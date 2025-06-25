#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <map>
#include "Token.h"
#include "../utils/ErrorHandler.h"

// Clase Lexer: Encargada de convertir el código fuente en una secuencia de tokens.
class Lexer {
public:
    // Constructor que toma el código fuente como entrada.
    explicit Lexer(const std::string& sourceCode);

    // Método principal para realizar el análisis léxico y devolver una lista de tokens.
    std::vector<Token> tokenize();

private:
    std::string source;       // El código fuente a analizar.
    size_t currentPos;        // Posición actual en el código fuente.
    int currentLine;          // Línea actual en el código fuente.
    int currentCol;           // Columna actual en la línea.

    // Mapa para identificar palabras clave.
    static const std::map<std::string, TokenType> keywords;

    // Métodos auxiliares para el análisis léxico.
    char peek(int offset = 0);           // Obtiene el carácter en la posición actual + offset sin avanzar.
    char advance();                      // Obtiene el carácter actual y avanza la posición.
    bool isAtEnd();                      // Verifica si se ha llegado al final del código fuente.
    void skipWhitespace();               // Ignora espacios en blanco, tabs y saltos de línea.
    Token scanToken();                   // Escanea y devuelve el siguiente token.
    Token identifierOrKeyword();         // Maneja identificadores y palabras clave.
    Token number();                      // Maneja números enteros.
    Token string();                      // Maneja literales de cadena.
};

#endif // LEXER_H