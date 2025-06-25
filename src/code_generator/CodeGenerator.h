// src/code_generator/CodeGenerator.h
#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H

#include "SFMLTranslator.h"
#include "../parser/AST.h" // Incluye el AST.h para todas las definiciones
#include "../utils/ErrorHandler.h" // <--- ¡NUEVO: Incluir ErrorHandler!
#include <string>
#include <memory>   // Para std::unique_ptr
#include <sstream>  // Para std::stringstream
#include <vector>   // Para std::vector en parámetros de funciones

// Forward declarations para los nodos del AST (generalmente no necesarias si AST.h se incluye completamente)
class ProgramNode;
class FunctionDeclarationNode;
class VariableDeclarationNode;
class AssignmentStatementNode;
class FunctionCallNode;
class ReturnStatementNode;
class IfStatementNode;
class ForStatementNode;
class PrintStatementNode;
class IdentifierNode;
class LiteralNode;
class BinaryExpressionNode;
class UnaryExpressionNode;
class BlockStatementNode;

class CodeGenerator {
public:
    // Constructor: Ahora toma una referencia a ErrorHandler
    explicit CodeGenerator(ErrorHandler& errorHandler); // <--- ¡CONSTRUCTOR MODIFICADO!

    std::string generate(ProgramNode* program);

    std::string visit(ASTNode* node);
    std::string visitProgramNode(ProgramNode* node);
    std::string visitFunctionDeclarationNode(FunctionDeclarationNode* node);
    std::string visitVariableDeclarationNode(VariableDeclarationNode* node);
    std::string visitAssignmentStatementNode(AssignmentStatementNode* node);
    std::string visitFunctionCallNode(FunctionCallNode* node);
    std::string visitReturnStatementNode(ReturnStatementNode* node);
    std::string visitIfStatementNode(IfStatementNode* node);
    std::string visitForStatementNode(ForStatementNode* node);
    std::string visitPrintStatementNode(PrintStatementNode* node);
    std::string visitBlockStatementNode(BlockStatementNode* node);

    std::string generateExpression(ASTNode* node);
    std::string visitIdentifierNode(IdentifierNode* node);
    std::string visitLiteralNode(LiteralNode* node);
    std::string visitBinaryExpressionNode(BinaryExpressionNode* node);
    std::string visitUnaryExpressionNode(UnaryExpressionNode* node);

private:
    SFMLTranslator translator;
    std::string currentFunctionName;
    ErrorHandler& errorHandler; // <--- ¡NUEVO: Miembro para el manejador de errores!
};

#endif // CODEGENERATOR_H