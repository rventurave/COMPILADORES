#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include <string>
#include <memory>
#include <vector>
#include <sstream>
#include "AST.h" // Incluir el AST
#include "SFMLTranslator.h" // Incluir el traductor SFML

// Clase CodeGenerator: Recorre el AST y genera código SFML.
class CodeGenerator {
public:
    explicit CodeGenerator(SFMLTranslator& translator);

    // Método principal para iniciar la generación de código.
    std::string generate(ASTNode* root);

private:
    SFMLTranslator& translator; // Referencia al traductor de SFML

    // Métodos para visitar y generar código para diferentes tipos de nodos AST.
    std::string visit(ASTNode* node);
    std::string visitProgramNode(ProgramNode* node);
    std::string visitFunctionDeclarationNode(FunctionDeclarationNode* node);
    std::string visitVariableDeclarationNode(VariableDeclarationNode* node);
    std::string visitAssignmentStatementNode(AssignmentStatementNode* node);
    std::string visitBinaryExpressionNode(BinaryExpressionNode* node);
    std::string visitLiteralNode(LiteralNode* node);
    std::string visitIdentifierNode(IdentifierNode* node);
    std::string visitIfStatementNode(IfStatementNode* node);
    std::string visitForStatementNode(ForStatementNode* node);
    std::string visitReturnStatementNode(ReturnStatementNode* node);
    std::string visitFunctionCallNode(FunctionCallNode* node);
    std::string visitPrintStatementNode(PrintStatementNode* node);
    std::string visitBlockStatementNode(BlockStatementNode* node);

    // Métodos auxiliares para construir expresiones
    std::string generateExpression(ASTNode* node);
};

#endif // CODE_GENERATOR_H