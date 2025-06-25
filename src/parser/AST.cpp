// src/parser/AST.cpp
#include "AST.h" // Incluir el propio encabezado

// Definiciones de constructores
ASTNode::ASTNode(ASTNodeType type) : type(type) {}

ProgramNode::ProgramNode() : ASTNode(ASTNodeType::Program) {}

FunctionDeclarationNode::FunctionDeclarationNode(const std::string& name, const std::string& returnType,
                                                std::vector<std::pair<std::string, std::string>> params,
                                                std::unique_ptr<ASTNode> body)
    : ASTNode(ASTNodeType::FunctionDeclaration), name(name), returnType(returnType),
      parameters(std::move(params)), body(std::move(body)) {}

VariableDeclarationNode::VariableDeclarationNode(const std::string& typeName, const std::string& variableName, std::unique_ptr<ASTNode> init)
    : ASTNode(ASTNodeType::VariableDeclaration), typeName(typeName), variableName(variableName), initializer(std::move(init)) {}

AssignmentStatementNode::AssignmentStatementNode(const std::string& identifier, std::unique_ptr<ASTNode> expr)
    : ASTNode(ASTNodeType::AssignmentStatement), identifierName(identifier), expression(std::move(expr)) {}

BinaryExpressionNode::BinaryExpressionNode(std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r, const std::string& op)
    : ASTNode(ASTNodeType::BinaryExpression), left(std::move(l)), right(std::move(r)), op(op) {}

UnaryExpressionNode::UnaryExpressionNode(const std::string& op, std::unique_ptr<ASTNode> operand)
    : ASTNode(ASTNodeType::UnaryExpression), op(op), operand(std::move(operand)) {}

LiteralNode::LiteralNode(const std::string& val) : ASTNode(ASTNodeType::Literal), value(val) {}

IdentifierNode::IdentifierNode(const std::string& name) : ASTNode(ASTNodeType::Identifier), name(name) {}

IfStatementNode::IfStatementNode(std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> thenB, std::unique_ptr<ASTNode> elseB)
    : ASTNode(ASTNodeType::IfStatement), condition(std::move(cond)), thenBlock(std::move(thenB)), elseBlock(std::move(elseB)) {}

ForStatementNode::ForStatementNode(std::unique_ptr<ASTNode> init, std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> inc, std::unique_ptr<ASTNode> bd)
    : ASTNode(ASTNodeType::ForStatement), initialization(std::move(init)), condition(std::move(cond)), increment(std::move(inc)), body(std::move(bd)) {}

ReturnStatementNode::ReturnStatementNode(std::unique_ptr<ASTNode> expr)
    : ASTNode(ASTNodeType::ReturnStatement), expression(std::move(expr)) {}

FunctionCallNode::FunctionCallNode(const std::string& name, std::vector<std::unique_ptr<ASTNode>> args)
    : ASTNode(ASTNodeType::FunctionCall), functionName(name), arguments(std::move(args)) {}

PrintStatementNode::PrintStatementNode(const std::string& format, std::vector<std::unique_ptr<ASTNode>> args)
    : ASTNode(ASTNodeType::PrintStatement), formatString(format), arguments(std::move(args)) {}

BlockStatementNode::BlockStatementNode(std::vector<std::unique_ptr<ASTNode>> stmts)
    : ASTNode(ASTNodeType::BlockStatement), statements(std::move(stmts)) {}