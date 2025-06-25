// src/semantic_analyzer/SemanticAnalyzer.h
#ifndef SEMANTICANALYZER_H
#define SEMANTICANALYZER_H

#include "../parser/AST.h" // Incluye todas las definiciones de nodos AST
#include "SymbolTable.h"   // Incluye la tabla de símbolos
#include "../utils/ErrorHandler.h" // Incluye ErrorHandler
#include <string>
#include <vector>
#include <map>

// Forward declarations de los nodos AST para los métodos visit
class ProgramNode;
class FunctionDeclarationNode;
class VariableDeclarationNode;
class AssignmentStatementNode;
class FunctionCallNode;
class ReturnStatementNode;
class IfStatementNode;
class ForStatementNode;
class PrintStatementNode;
class BlockStatementNode;
class IdentifierNode;
class LiteralNode;
class BinaryExpressionNode;
class UnaryExpressionNode;


class SemanticAnalyzer {
public:
    // Constructor: Ahora toma una referencia a ErrorHandler
    explicit SemanticAnalyzer(ErrorHandler& errorHandler); // <--- ¡CONSTRUCTOR MODIFICADO!

    // Método principal para iniciar el análisis semántico.
    void analyze(ProgramNode* program);

    // Métodos para visitar cada tipo de nodo AST y realizar el análisis semántico
    void visit(ASTNode* node);
    void visitProgramNode(ProgramNode* node);
    void visitFunctionDeclarationNode(FunctionDeclarationNode* node);
    void visitVariableDeclarationNode(VariableDeclarationNode* node);
    void visitAssignmentStatementNode(AssignmentStatementNode* node);
    void visitFunctionCallNode(FunctionCallNode* node);
    void visitReturnStatementNode(ReturnStatementNode* node);
    void visitIfStatementNode(IfStatementNode* node);
    void visitForStatementNode(ForStatementNode* node);
    void visitPrintStatementNode(PrintStatementNode* node);
    void visitBlockStatementNode(BlockStatementNode* node);

    // Métodos para analizar expresiones y verificar tipos
    // Estos métodos deberían devolver el tipo del resultado de la expresión (ej. "int", "void")
    // o un tipo especial para errores. Por ahora, solo devolverán bool (true si es válido).
    bool analyzeExpression(ASTNode* node);
    bool visitIdentifierNode(IdentifierNode* node);
    bool visitLiteralNode(LiteralNode* node);
    bool visitBinaryExpressionNode(BinaryExpressionNode* node);
    bool visitUnaryExpressionNode(UnaryExpressionNode* node);

private:
    SymbolTable symbolTable;       // La tabla de símbolos para gestionar el ámbito.
    ErrorHandler& errorHandler;    // Referencia al manejador de errores. // <--- ¡MIEMBRO NUEVO!

    // Para mantener un registro del tipo de retorno de la función actual.
    std::string currentFunctionReturnType;
};

#endif // SEMANTICANALYZER_H