#include "Lexer.h"
#include <cctype> // Para isalpha, isdigit, isalnum

// Inicialización del mapa de palabras clave
const std::map<std::string, TokenType> Lexer::keywords = {
    {"int", TokenType::KEYWORD_INT},
    {"void", TokenType::KEYWORD_VOID},
    {"if", TokenType::KEYWORD_IF},
    {"else", TokenType::KEYWORD_ELSE},
    {"for", TokenType::KEYWORD_FOR},
    {"return", TokenType::KEYWORD_RETURN},
    {"printf", TokenType::KEYWORD_PRINTF}
};

// Constructor
Lexer::Lexer(const std::string& sourceCode)
    : source(sourceCode), currentPos(0), currentLine(1), currentCol(1) {}

// Obtiene el carácter en la posición actual + offset sin avanzar
char Lexer::peek(int offset) {
    if (currentPos + offset >= source.length()) {
        return '\0'; // Carácter nulo para indicar el final
    }
    return source[currentPos + offset];
}

// Obtiene el carácter actual y avanza la posición
char Lexer::advance() {
    char c = peek();
    if (!isAtEnd()) {
        currentPos++;
        currentCol++;
        return c;
    }
    return '\0';
}

// Verifica si se ha llegado al final del código fuente
bool Lexer::isAtEnd() {
    return currentPos >= source.length();
}

// Ignora espacios en blanco, tabs, saltos de línea y comentarios
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
                advance();
                currentLine++;
                currentCol = 1;
                break;
            case '/':
                // Manejar comentarios de una línea //
                if (peek(1) == '/') {
                    while (!isAtEnd() && peek() != '\n') {
                        advance();
                    }
                }
                // Manejar comentarios de múltiples líneas /* */
                else if (peek(1) == '*') {
                    advance(); // Consume '/'
                    advance(); // Consume '*'
                    while (!isAtEnd()) {
                        if (peek() == '*' && peek(1) == '/') {
                            advance(); // Consume '*'
                            advance(); // Consume '/'
                            break;
                        }
                        advance();
                        if (peek() == '\n') {
                            currentLine++;
                            currentCol = 1;
                        }
                    }
                    if (isAtEnd()) {
                        ErrorHandler::reportError("Error: Comentario multi-línea sin cerrar.", currentLine, currentCol);
                    }
                } else {
                    return; // No es un comentario, salir del switch
                }
                break;
            default:
                return; // No es espacio en blanco ni comentario
        }
    }
}

// Escanea y devuelve el siguiente token
Token Lexer::scanToken() {
    skipWhitespace();

    if (isAtEnd()) {
        return Token(TokenType::END_OF_FILE, "", currentLine, currentCol);
    }

    char c = advance();
    int startCol = currentCol - 1; // La columna donde empieza el token

    if (isalpha(c) || c == '_') {
        return identifierOrKeyword();
    }
    if (isdigit(c)) {
        return number();
    }
    if (c == '"') {
        return string();
    }

    switch (c) {
        case '(': return Token(TokenType::LPAREN, "(", currentLine, startCol);
        case ')': return Token(TokenType::RPAREN, ")", currentLine, startCol);
        case '{': return Token(TokenType::LBRACE, "{", currentLine, startCol);
        case '}': return Token(TokenType::RBRACE, "}", currentLine, startCol);
        case ';': return Token(TokenType::SEMICOLON, ";", currentLine, startCol);
        case ',': return Token(TokenType::COMMA, ",", currentLine, startCol);
        case '+': return Token(TokenType::PLUS, "+", currentLine, startCol);
        case '-': return Token(TokenType::MINUS, "-", currentLine, startCol);
        case '*': return Token(TokenType::MULTIPLY, "*", currentLine, startCol);
        case '/': return Token(TokenType::DIVIDE, "/", currentLine, startCol);
        case '=':
            if (peek() == '=') {
                advance();
                return Token(TokenType::EQUAL_EQUAL, "==", currentLine, startCol);
            }
            return Token(TokenType::ASSIGN, "=", currentLine, startCol);
        case '<':
            if (peek() == '=') {
                advance();
                return Token(TokenType::LESS_EQUAL, "<=", currentLine, startCol);
            }
            return Token(TokenType::LESS_THAN, "<", currentLine, startCol);
        case '>':
            if (peek() == '=') {
                advance();
                return Token(TokenType::GREATER_EQUAL, ">=", currentLine, startCol);
            }
            return Token(TokenType::GREATER_THAN, ">", currentLine, startCol);
        case '!':
            if (peek() == '=') {
                advance();
                return Token(TokenType::NOT_EQUAL, "!=", currentLine, startCol);
            }
            // En C, '!' por sí solo puede ser el operador lógico NOT.
            // Dependiendo del alcance de C que quieras soportar, podrías añadirlo.
            // Por ahora, lo dejamos como UNKNOWN si no va con '='.
            break;
        default:
            ErrorHandler::reportError("Carácter inesperado: '" + std::string(1, c) + "'", currentLine, startCol);
            return Token(TokenType::UNKNOWN, std::string(1, c), currentLine, startCol);
    }
    return Token(TokenType::UNKNOWN, std::string(1, c), currentLine, startCol); // En caso de que el switch no maneje un caracter
}

// Maneja identificadores y palabras clave
Token Lexer::identifierOrKeyword() {
    size_t startPos = currentPos - 1; // La posición donde comenzó el identificador/palabra clave
    int startCol = currentCol - 1;

    while (isalnum(peek()) || peek() == '_') {
        advance();
    }
    std::string value = source.substr(startPos, currentPos - startPos);

    // Verificar si es una palabra clave
    auto it = keywords.find(value);
    if (it != keywords.end()) {
        return Token(it->second, value, currentLine, startCol);
    }
    return Token(TokenType::IDENTIFIER, value, currentLine, startCol);
}

// Maneja números enteros
Token Lexer::number() {
    size_t startPos = currentPos - 1;
    int startCol = currentCol - 1;

    while (isdigit(peek())) {
        advance();
    }
    // Si queremos soportar flotantes, aquí habría que añadir lógica para el punto decimal
    // For now, only integers
    std::string value = source.substr(startPos, currentPos - startPos);
    return Token(TokenType::INTEGER_LITERAL, value, currentLine, startCol);
}

// Maneja literales de cadena
Token Lexer::string() {
    size_t startPos = currentPos; // El contenido de la cadena comienza después del '"' inicial
    int startCol = currentCol;

    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') { // Las cadenas no pueden cruzar líneas sin ser escapadas
            ErrorHandler::reportError("Error: Salto de línea inesperado dentro de una cadena.", currentLine, currentCol);
            // Podrías devolver un token de error o intentar recuperarte
            return Token(TokenType::UNKNOWN, "", currentLine, startCol);
        }
        advance();
    }

    if (isAtEnd()) {
        ErrorHandler::reportError("Error: Cadena sin cerrar.", currentLine, startCol);
        return Token(TokenType::UNKNOWN, "", currentLine, startCol);
    }

    std::string value = source.substr(startPos, currentPos - startPos);
    advance(); // Consume el '"' de cierre
    return Token(TokenType::STRING_LITERAL, value, currentLine, startCol);
}

// Método principal para realizar el análisis léxico
std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (!isAtEnd()) {
        Token token = scanToken();
        if (token.type == TokenType::END_OF_FILE) {
            break; // Salir del bucle si se alcanzó el final
        }
        if (token.type != TokenType::UNKNOWN) { // No añadir tokens de error a la lista principal
            tokens.push_back(token);
        }
    }
    // Añadir el token de fin de archivo al final
    tokens.push_back(Token(TokenType::END_OF_FILE, "", currentLine, currentCol));
    return tokens;
}