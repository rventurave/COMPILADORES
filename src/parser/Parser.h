// src/parser/Parser.h
#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <memory> // Para std::unique_ptr
#include "../lexer/Token.h" // Incluye la definición de Token
#include "AST.h" // Incluye la definición de ASTNode, ProgramNode, etc.
#include "../utils/ErrorHandler.h" // Incluye ErrorHandler

// Clase Parser: Construye un Árbol de Sintaxis Abstracta (AST) a partir de una secuencia de tokens.
class Parser {
public:
    // Constructor modificado para recibir una referencia a ErrorHandler
    explicit Parser(const std::vector<Token>& tokens, ErrorHandler& errorHandler);

    // Método principal para iniciar el análisis y construir el AST.
    std::unique_ptr<ASTNode> parse();

private:
    const std::vector<Token>& tokens; // Referencia a la lista de tokens.
    size_t currentTokenIndex;          // Índice del token actual que se está procesando.
    ErrorHandler& errorHandler;        // Referencia al manejador de errores.

    // Métodos auxiliares para el análisis sintáctico (gramática descendente recursiva)
    Token peek(int offset = 0); // Mira el token en la posición actual + offset sin avanzar.
    Token consume();            // Consume el token actual y avanza al siguiente.
    bool match(TokenType type); // Consume el token actual si su tipo coincide con 'type'.
    // expect ahora usa el miembro errorHandler para reportar errores
    Token expect(TokenType type, const std::string& errorMessage);
    bool isAtEnd();             // Verifica si se ha llegado al final de los tokens.

    // Métodos para parsear diferentes construcciones del lenguaje C (Devuelven unique_ptr<ASTNode>)
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

    // Métodos para parsear expresiones (Devuelven unique_ptr<ASTNode>)
    std::unique_ptr<ASTNode> parseExpression(); // Punto de entrada para expresiones
    std::unique_ptr<ASTNode> parseEqualityExpression(); // ==, !=
    std::unique_ptr<ASTNode> parseComparisonExpression(); // <, <=, >, >=
    std::unique_ptr<ASTNode> parseAdditiveExpression();   // +, -
    std::unique_ptr<ASTNode> parseMultiplicativeExpression(); // *, /
    std::unique_ptr<ASTNode> parsePrimaryExpression();    // Literales, identificadores, (expresiones)
};

#endif // PARSER_H