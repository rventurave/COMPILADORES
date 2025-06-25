#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include <memory>
#include <string>
#include "AST.h" // Incluir el AST
#include "SymbolTable.h"
#include "../utils/ErrorHandler.h"

// Clase SemanticAnalyzer: Realiza comprobaciones semánticas en el AST.
class SemanticAnalyzer {
public:
    explicit SemanticAnalyzer(SymbolTable& symbolTable);

    // Método principal para iniciar el análisis semántico.
    bool analyze(ASTNode* root);

private:
    SymbolTable& symbolTable; // Referencia a la tabla de símbolos.
    bool hasErrors;           // Bandera para indicar si se encontraron errores semánticos.

    // Métodos para recorrer el AST y realizar comprobaciones semánticas.
    void visit(ASTNode* node);
    void visitProgramNode(ProgramNode* node);
    void visitFunctionDeclarationNode(FunctionDeclarationNode* node);
    void visitVariableDeclarationNode(VariableDeclarationNode* node);
    void visitAssignmentStatementNode(AssignmentStatementNode* node);
    void visitBinaryExpressionNode(BinaryExpressionNode* node);
    void visitLiteralNode(LiteralNode* node);
    void visitIdentifierNode(IdentifierNode* node);
    void visitIfStatementNode(IfStatementNode* node);
    void visitForStatementNode(ForStatementNode* node);
    void visitReturnStatementNode(ReturnStatementNode* node);
    void visitFunctionCallNode(FunctionCallNode* node);
    void visitPrintStatementNode(PrintStatementNode* node);
    void visitBlockStatementNode(BlockStatementNode* node);

    // Método auxiliar para determinar el tipo de una expresión
    std::string getType(ASTNode* node);
};

#endif // SEMANTIC_ANALYZER_H