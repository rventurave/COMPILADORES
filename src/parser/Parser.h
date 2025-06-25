#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <memory>
#include "../lexer/Token.h"
#include "AST.h"
#include "../utils/ErrorHandler.h"

// Clase Parser: Construye un Árbol de Sintaxis Abstracta (AST) a partir de una secuencia de tokens.
class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);

    // Método principal para iniciar el análisis y construir el AST.
    std::unique_ptr<ASTNode> parse();

private:
    const std::vector<Token>& tokens; // Referencia a la lista de tokens.
    size_t currentTokenIndex;          // Índice del token actual que se está procesando.

    // Métodos auxiliares para el análisis sintáctico (gramática descendente recursiva)
    Token peek(int offset = 0); // Mira el token en la posición actual + offset sin avanzar.
    Token consume();            // Consume el token actual y avanza al siguiente.
    bool match(TokenType type); // Consume el token actual si su tipo coincide con 'type'.
    Token expect(TokenType type, const std::string& errorMessage); // Espera un token de un tipo específico, si no lo encuentra, reporta un error.
    bool isAtEnd();             // Verifica si se ha llegado al final de los tokens.

    // Métodos para parsear diferentes construcciones del lenguaje C
    std::unique_ptr<ASTNode> parseProgram();
    std::unique_ptr<ASTNode> parseFunctionDeclaration();
    std::unique_ptr<ASTNode> parseBlockStatement(); // {}
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseDeclarationStatement(); // int x; o int x = 10;
    std::unique_ptr<ASTNode> parseAssignmentStatement(); // x = 10;
    std::unique_ptr<ASTNode> parseIfStatement();
    std::unique_ptr<ASTNode> parseForStatement();
    std::unique_ptr<ASTNode> parseReturnStatement();
    std::unique_ptr<ASTNode> parsePrintStatement(); // printf(...)
    std::unique_ptr<ASTNode> parseFunctionCall();

    // Métodos para parsear expresiones
    std::unique_ptr<ASTNode> parseExpression(); // Punto de entrada para expresiones
    std::unique_ptr<ASTNode> parseEqualityExpression(); // ==, !=
    std::unique_ptr<ASTNode> parseComparisonExpression(); // <, <=, >, >=
    std::unique_ptr<ASTNode> parseAdditiveExpression();   // +, -
    std::unique_ptr<ASTNode> parseMultiplicativeExpression(); // *, /
    std::unique_ptr<ASTNode> parsePrimaryExpression();    // Literales, identificadores, (expresiones)
};

#endif // PARSER_H