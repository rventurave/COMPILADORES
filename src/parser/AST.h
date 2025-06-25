// src/parser/AST.h
#ifndef AST_H
#define AST_H

#include <vector>
#include <string>
#include <memory> // Para std::unique_ptr
#include <utility> // Para std::move en constructores

// Enumeración para los tipos de nodos AST
enum class ASTNodeType {
    Program,
    FunctionDeclaration,
    VariableDeclaration,
    AssignmentStatement,
    BinaryExpression,
    UnaryExpression,
    Literal,
    Identifier,
    IfStatement,
    ForStatement,
    ReturnStatement,
    FunctionCall,
    PrintStatement, // Para printf
    BlockStatement  // Para bloques de código {}
};

// Clase base para todos los nodos del AST
class ASTNode {
public:
    ASTNodeType type;

    explicit ASTNode(ASTNodeType type);
    virtual ~ASTNode() = default; // Destructor virtual para asegurar la correcta limpieza
};

// Nodo para el programa completo (raíz del AST)
class ProgramNode : public ASTNode {
public:
    // Los hijos ProgramNode::children serán FunctionDeclarationNode y StatementNode (globales o main)
    std::vector<std::unique_ptr<ASTNode>> functionDeclarations; // Aquí almacenaremos las funciones
    std::vector<std::unique_ptr<ASTNode>> statements; // Y aquí las sentencias globales/main

    ProgramNode(); // Constructor declarado
};

// Nodo para una declaración de función
class FunctionDeclarationNode : public ASTNode {
public:
    std::string name;
    std::string returnType; // "int", "void", etc.
    std::vector<std::pair<std::string, std::string>> parameters; // Tipo, Nombre
    std::unique_ptr<ASTNode> body; // El cuerpo de la función es un BlockStatementNode

    FunctionDeclarationNode(const std::string& name, const std::string& returnType,
                            std::vector<std::pair<std::string, std::string>> params,
                            std::unique_ptr<ASTNode> body); // Constructor declarado
};

// Nodo para una declaración de variable
class VariableDeclarationNode : public ASTNode {
public:
    std::string typeName; // e.g., "int"
    std::string variableName;
    std::unique_ptr<ASTNode> initializer; // Opcional, si se inicializa

    VariableDeclarationNode(const std::string& typeName, const std::string& variableName, std::unique_ptr<ASTNode> init = nullptr); // Constructor declarado
};

// Nodo para una asignación
class AssignmentStatementNode : public ASTNode {
public:
    std::string identifierName;
    std::unique_ptr<ASTNode> expression;

    AssignmentStatementNode(const std::string& identifier, std::unique_ptr<ASTNode> expr); // Constructor declarado
};

// Nodo para una expresión binaria (ej. a + b, x == y)
class BinaryExpressionNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    std::string op; // Operador: "+", "-", "*", "/", "=", "==", "<", ">", etc.

    BinaryExpressionNode(std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r, const std::string& op); // Constructor declarado
};

// Nodo para una expresión unaria (ej. -x, !cond)
class UnaryExpressionNode : public ASTNode {
public:
    std::string op; // Operador unario: "-", "!"
    std::unique_ptr<ASTNode> operand; // El operando de la expresión unaria

    UnaryExpressionNode(const std::string& op, std::unique_ptr<ASTNode> operand); // Constructor declarado
};

// Nodo para un literal (entero, cadena)
class LiteralNode : public ASTNode {
public:
    std::string value; // El valor del literal (ej. "10", "\"hola\"")
    // Considerar un campo para el tipo de literal (INT, STRING) si es necesario
    LiteralNode(const std::string& val); // Constructor declarado
};

// Nodo para un identificador
class IdentifierNode : public ASTNode {
public:
    std::string name;

    IdentifierNode(const std::string& name); // Constructor declarado
};

// Nodo para una declaración if
class IfStatementNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> thenBlock; // Bloque de código si la condición es verdadera
    std::unique_ptr<ASTNode> elseBlock; // Bloque de código si la condición es falsa (opcional)

    IfStatementNode(std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> thenB, std::unique_ptr<ASTNode> elseB = nullptr); // Constructor declarado
};

// Nodo para una declaración for
class ForStatementNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> initialization; // Declaración/asignación inicial (ej. int i = 0;)
    std::unique_ptr<ASTNode> condition;      // Condición de bucle (ej. i < 10;)
    std::unique_ptr<ASTNode> increment;      // Expresión de incremento/decremento (ej. i++)
    std::unique_ptr<ASTNode> body;           // Cuerpo del bucle

    ForStatementNode(std::unique_ptr<ASTNode> init, std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> inc, std::unique_ptr<ASTNode> bd); // Constructor declarado
};

// Nodo para una declaración return
class ReturnStatementNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> expression; // Expresión a retornar (opcional)

    ReturnStatementNode(std::unique_ptr<ASTNode> expr = nullptr); // Constructor declarado
};

// Nodo para una llamada a función (ej. printf("hello");)
class FunctionCallNode : public ASTNode {
public:
    std::string functionName;
    std::vector<std::unique_ptr<ASTNode>> arguments;

    FunctionCallNode(const std::string& name, std::vector<std::unique_ptr<ASTNode>> args = {}); // Constructor declarado
};

// Nodo específico para la función printf
class PrintStatementNode : public ASTNode {
public:
    std::string formatString;
    std::vector<std::unique_ptr<ASTNode>> arguments; // Argumentos después de la cadena de formato

    PrintStatementNode(const std::string& format, std::vector<std::unique_ptr<ASTNode>> args = {}); // Constructor declarado
};

// Nodo para un bloque de sentencias (ej. el cuerpo de una función o un bloque if/else)
class BlockStatementNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> statements; // Contiene las sentencias dentro del bloque

    BlockStatementNode(std::vector<std::unique_ptr<ASTNode>> stmts = {}); // Constructor declarado
};

#endif // AST_H