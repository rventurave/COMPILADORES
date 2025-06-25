#ifndef AST_H
#define AST_H

#include <vector>
#include <string>
#include <memory> // Para std::unique_ptr

// Enumeración para los tipos de nodos AST
enum class ASTNodeType {
    Program,
    FunctionDeclaration,
    VariableDeclaration,
    AssignmentStatement,
    BinaryExpression,
    Literal,
    Identifier,
    IfStatement,
    ForStatement,
    ReturnStatement,
    FunctionCall,
    PrintStatement, // Para printf
    BlockStatement // Para bloques de código {}
};

// Clase base para todos los nodos del AST
class ASTNode {
public:
    ASTNodeType type;
    std::vector<std::unique_ptr<ASTNode>> children;

    explicit ASTNode(ASTNodeType type) : type(type) {}
    virtual ~ASTNode() = default; // Destructor virtual para asegurar la correcta limpieza

    // Método virtual para aceptar un visitante (útil para la traversía del AST)
    // virtual void accept(ASTVisitor& visitor) = 0; // Se podría añadir un patrón Visitor
};

// Nodo para el programa completo (raíz del AST)
class ProgramNode : public ASTNode {
public:
    ProgramNode() : ASTNode(ASTNodeType::Program) {}
};

// Nodo para una declaración de función
class FunctionDeclarationNode : public ASTNode {
public:
    std::string name;
    std::string returnType; // "int", "void", etc.
    std::vector<std::pair<std::string, std::string>> parameters; // Tipo, Nombre

    FunctionDeclarationNode(const std::string& name, const std::string& returnType)
        : ASTNode(ASTNodeType::FunctionDeclaration), name(name), returnType(returnType) {}
};

// Nodo para una declaración de variable
class VariableDeclarationNode : public ASTNode {
public:
    std::string typeName; // e.g., "int"
    std::string variableName;
    std::unique_ptr<ASTNode> initializer; // Opcional, si se inicializa

    VariableDeclarationNode(const std::string& typeName, const std::string& variableName, std::unique_ptr<ASTNode> init = nullptr)
        : ASTNode(ASTNodeType::VariableDeclaration), typeName(typeName), variableName(variableName), initializer(std::move(init)) {}
};

// Nodo para una asignación
class AssignmentStatementNode : public ASTNode {
public:
    std::string identifierName;
    std::unique_ptr<ASTNode> expression;

    AssignmentStatementNode(const std::string& identifier, std::unique_ptr<ASTNode> expr)
        : ASTNode(ASTNodeType::AssignmentStatement), identifierName(identifier), expression(std::move(expr)) {}
};

// Nodo para una expresión binaria (ej. a + b, x == y)
class BinaryExpressionNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    std::string op; // Operador: "+", "-", "*", "/", "=", "==", "<", ">", etc.

    BinaryExpressionNode(std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r, const std::string& op)
        : ASTNode(ASTNodeType::BinaryExpression), left(std::move(l)), right(std::move(r)), op(op) {}
};

// Nodo para un literal (entero, cadena)
class LiteralNode : public ASTNode {
public:
    std::string value; // El valor del literal (ej. "10", "\"hola\"")
    // Considerar un campo para el tipo de literal (INT, STRING) si es necesario
    LiteralNode(const std::string& val) : ASTNode(ASTNodeType::Literal), value(val) {}
};

// Nodo para un identificador
class IdentifierNode : public ASTNode {
public:
    std::string name;

    IdentifierNode(const std::string& name) : ASTNode(ASTNodeType::Identifier), name(name) {}
};

// Nodo para una declaración if
class IfStatementNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> thenBlock; // Bloque de código si la condición es verdadera
    std::unique_ptr<ASTNode> elseBlock; // Bloque de código si la condición es falsa (opcional)

    IfStatementNode(std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> thenB, std::unique_ptr<ASTNode> elseB = nullptr)
        : ASTNode(ASTNodeType::IfStatement), condition(std::move(cond)), thenBlock(std::move(thenB)), elseBlock(std::move(elseB)) {}
};

// Nodo para una declaración for
class ForStatementNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> initialization; // Declaración/asignación inicial (ej. int i = 0;)
    std::unique_ptr<ASTNode> condition;      // Condición de bucle (ej. i < 10;)
    std::unique_ptr<ASTNode> increment;      // Expresión de incremento/decremento (ej. i++)
    std::unique_ptr<ASTNode> body;           // Cuerpo del bucle

    ForStatementNode(std::unique_ptr<ASTNode> init, std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> inc, std::unique_ptr<ASTNode> bd)
        : ASTNode(ASTNodeType::ForStatement), initialization(std::move(init)), condition(std::move(cond)), increment(std::move(inc)), body(std::move(bd)) {}
};

// Nodo para una declaración return
class ReturnStatementNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> expression; // Expresión a retornar (opcional)

    ReturnStatementNode(std::unique_ptr<ASTNode> expr = nullptr)
        : ASTNode(ASTNodeType::ReturnStatement), expression(std::move(expr)) {}
};

// Nodo para una llamada a función (ej. printf("hello");)
class FunctionCallNode : public ASTNode {
public:
    std::string functionName;
    std::vector<std::unique_ptr<ASTNode>> arguments;

    FunctionCallNode(const std::string& name) : ASTNode(ASTNodeType::FunctionCall), functionName(name) {}
};

// Nodo específico para la función printf (puede ser una FunctionCallNode, pero es útil tener una distinción si se maneja de forma especial)
class PrintStatementNode : public ASTNode {
public:
    std::string formatString;
    std::vector<std::unique_ptr<ASTNode>> arguments; // Argumentos después de la cadena de formato

    PrintStatementNode(const std::string& format) : ASTNode(ASTNodeType::PrintStatement), formatString(format) {}
};

// Nodo para un bloque de sentencias (ej. el cuerpo de una función o un bloque if/else)
class BlockStatementNode : public ASTNode {
public:
    BlockStatementNode() : ASTNode(ASTNodeType::BlockStatement) {}
    // Las sentencias están en `children`
};


#endif // AST_H