// src/lexer/Lexer.cpp
#include "Lexer.h"
#include <iostream> // Para depuración, si es necesario

// Constructor
Lexer::Lexer(const std::string& sourceCode, ErrorHandler& errorHandler)
    : sourceCode(sourceCode), currentIndex(0), currentLine(1), currentCol(1), errorHandler(errorHandler) {
    // Inicializar palabras clave
    keywords["int"] = TokenType::KEYWORD_INT;
    keywords["void"] = TokenType::KEYWORD_VOID;
    keywords["if"] = TokenType::KEYWORD_IF;
    keywords["else"] = TokenType::KEYWORD_ELSE;
    keywords["for"] = TokenType::KEYWORD_FOR;
    keywords["return"] = TokenType::KEYWORD_RETURN;
    keywords["printf"] = TokenType::KEYWORD_PRINTF;
    // Añade aquí más palabras clave si tu lenguaje las tiene (ej. while, break, continue)
}

// Mira el carácter en la posición actual + offset sin avanzar.
char Lexer::peek(int offset) {
    if (currentIndex + offset >= sourceCode.length()) {
        return '\0'; // Carácter nulo para indicar fin de archivo
    }
    return sourceCode[currentIndex + offset];
}

// Consume el carácter actual y avanza al siguiente.
char Lexer::advance() {
    char c = peek();
    if (c != '\0') {
        currentIndex++;
        currentCol++;
        if (c == '\n') {
            currentLine++;
            currentCol = 1;
        }
    }
    return c;
}

// Verifica si se ha llegado al final del código fuente.
bool Lexer::isAtEnd() {
    return peek() == '\0';
}

// Salta espacios en blanco y comentarios.
void Lexer::skipWhitespace() {
    while (!isAtEnd()) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                advance(); // advance ya maneja la línea y columna
                break;
            case '/':
                if (peek(1) == '/') { // Comentario de una línea //
                    while (peek() != '\n' && !isAtEnd()) {
                        advance();
                    }
                } else if (peek(1) == '*') { // Comentario de múltiples líneas /* */
                    advance(); // Consume '/'
                    advance(); // Consume '*'
                    while (!isAtEnd()) {
                        if (peek() == '*' && peek(1) == '/') {
                            advance(); // Consume '*'
                            advance(); // Consume '/'
                            break;
                        }
                        advance();
                    }
                    if (isAtEnd()) {
                        // Reportar error de comentario no cerrado
                        errorHandler.reportError("Comentario de múltiples líneas no cerrado.", currentLine, currentCol); // <--- ¡ORDEN CORREGIDO!
                    }
                } else {
                    return; // No es un comentario, es solo '/'
                }
                break;
            default:
                return; // No es espacio en blanco ni comentario
        }
    }
}

// Crea un token con la información actual.
Token Lexer::makeToken(TokenType type, const std::string& value) {
    return Token(type, value, currentLine, currentCol - value.length()); // Ajustar columna
}

// Escanea el siguiente token.
void Lexer::scanToken() {
    skipWhitespace(); // Saltar cualquier espacio en blanco o comentario antes de escanear

    if (isAtEnd()) {
        // Al final del archivo, agrega el token END_OF_FILE con la posición actual.
        // Se resta 1 a currentCol si es el final de línea para apuntar al final del archivo.
        int finalCol = currentCol;
        if (sourceCode.empty() || sourceCode.back() == '\n') {
            finalCol = 0; // Si el archivo termina en newline, la columna es 0 para EOF
        }
        tokens.emplace_back(TokenType::END_OF_FILE, "", currentLine, finalCol); // <--- ¡'tokens' ahora es miembro!
        return;
    }

    char c = advance(); // Consume el primer carácter del token

    // Determinar el tipo de token según el carácter actual
    if (isalpha(c) || c == '_') {
        tokens.push_back(scanIdentifierOrKeyword()); // <--- ¡'tokens' ahora es miembro!
    } else if (isdigit(c)) {
        tokens.push_back(scanNumber()); // <--- ¡'tokens' ahora es miembro!
    } else if (c == '"') {
        tokens.push_back(scanString()); // <--- ¡'tokens' ahora es miembro!
    } else {
        // Manejar operadores y puntuación
        switch (c) {
            case '+': tokens.push_back(makeToken(TokenType::PLUS, "+")); break;
            case '-': tokens.push_back(makeToken(TokenType::MINUS, "-")); break;
            case '*': tokens.push_back(makeToken(TokenType::MULTIPLY, "*")); break;
            case '/': tokens.push_back(makeToken(TokenType::DIVIDE, "/")); break;
            case '&': tokens.push_back(makeToken(TokenType::AMPERSAND, "&")); break;
            case '=':
                if (peek() == '=') {
                    advance();
                    tokens.push_back(makeToken(TokenType::EQUAL_EQUAL, "=="));
                } else {
                    tokens.push_back(makeToken(TokenType::ASSIGN, "="));
                }
                break;
            case '<':
                if (peek() == '=') {
                    advance();
                    tokens.push_back(makeToken(TokenType::LESS_EQUAL, "<="));
                } else {
                    tokens.push_back(makeToken(TokenType::LESS_THAN, "<"));
                }
                break;
            case '>':
                if (peek() == '=') {
                    advance();
                    tokens.push_back(makeToken(TokenType::GREATER_EQUAL, ">="));
                } else {
                    tokens.push_back(makeToken(TokenType::GREATER_THAN, ">"));
                }
                break;
            case '!':
                if (peek() == '=') {
                    advance();
                    tokens.push_back(makeToken(TokenType::NOT_EQUAL, "!="));
                } else {
                    // Si tienes un operador de negación lógica '!', podrías manejarlo aquí.
                    errorHandler.reportError("Operador '!' no esperado sin '='.", currentLine, currentCol); // <--- ¡ORDEN CORREGIDO!
                    tokens.push_back(makeToken(TokenType::UNKNOWN, "!"));
                }
                break;
            case '(': tokens.push_back(makeToken(TokenType::LPAREN, "(")); break;
            case ')': tokens.push_back(makeToken(TokenType::RPAREN, ")")); break;
            case '{': tokens.push_back(makeToken(TokenType::LBRACE, "{")); break;
            case '}': tokens.push_back(makeToken(TokenType::RBRACE, "}")); break;
            case ';': tokens.push_back(makeToken(TokenType::SEMICOLON, ";")); break;
            case ',': tokens.push_back(makeToken(TokenType::COMMA, ",")); break;
            default:
                // Carácter desconocido
                std::string unknownChar(1, c);
                errorHandler.reportError("Carácter desconocido: '" + unknownChar + "'", currentLine, currentCol); // <--- ¡ORDEN CORREGIDO!
                tokens.push_back(makeToken(TokenType::UNKNOWN, unknownChar));
                break;
        }
    }
}

// Escanea un identificador o una palabra clave.
Token Lexer::scanIdentifierOrKeyword() {
    size_t start = currentIndex - 1; // El 'c' inicial ya fue avanzado
    while (isalnum(peek()) || peek() == '_') {
        advance();
    }
    std::string value = sourceCode.substr(start, currentIndex - start);

    // Verificar si es una palabra clave
    auto it = keywords.find(value);
    if (it != keywords.end()) {
        return makeToken(it->second, value);
    }
    return makeToken(TokenType::IDENTIFIER, value);
}

// Escanea un número (literal entero).
Token Lexer::scanNumber() {
    size_t start = currentIndex - 1; // El 'c' inicial ya fue avanzado
    while (isdigit(peek())) {
        advance();
    }
    // Si necesitas flotantes, añadirías lógica para '.' aquí.
    std::string value = sourceCode.substr(start, currentIndex - start);
    return makeToken(TokenType::INTEGER_LITERAL, value);
}

// Escanea una cadena (literal de cadena).
Token Lexer::scanString() {
    size_t start = currentIndex; // Empezar después de la comilla de apertura
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') { // Las cadenas multilínea son un error en C estándar sin '\'
            errorHandler.reportError("Saltos de línea no permitidos dentro de literales de cadena.", currentLine, currentCol); // <--- ¡ORDEN CORREGIDO!
        }
        advance();
    }

    if (isAtEnd()) {
        errorHandler.reportError("Cadena no terminada.", currentLine, currentCol); // <--- ¡ORDEN CORREGIDO!
        return makeToken(TokenType::UNKNOWN, sourceCode.substr(start -1, currentIndex - (start-1))); // Incluir la comilla de apertura
    }

    advance(); // Consume la comilla de cierre '"'
    std::string value = sourceCode.substr(start, currentIndex - 1 - start); // Excluir las comillas
    return makeToken(TokenType::STRING_LITERAL, value);
}

// Método principal para realizar el análisis léxico y devolver una lista de tokens.
std::vector<Token> Lexer::tokenize() {
    tokens.clear(); // Limpiar tokens de un posible análisis previo
    currentIndex = 0;
    currentLine = 1;
    currentCol = 1;

    while (!isAtEnd()) {
        scanToken();
    }
    scanToken(); // Llamada final para agregar el token END_OF_FILE
    return tokens;
}