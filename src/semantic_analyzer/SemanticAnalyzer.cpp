#include "SemanticAnalyzer.h"

// Constructor
SemanticAnalyzer::SemanticAnalyzer(SymbolTable& symbolTable)
    : symbolTable(symbolTable), hasErrors(false) {}

// Método principal para iniciar el análisis semántico.
bool SemanticAnalyzer::analyze(ASTNode* root) {
    if (!root) {
        return false; // No hay AST para analizar
    }
    symbolTable.enterScope(); // Entrar al scope global
    visit(root);
    symbolTable.exitScope(); // Salir del scope global
    return !hasErrors;
}

// Método de dispatch para visitar diferentes tipos de nodos AST
void SemanticAnalyzer::visit(ASTNode* node) {
    if (!node) return;

    switch (node->type) {
        case ASTNodeType::Program:
            visitProgramNode(static_cast<ProgramNode*>(node));
            break;
        case ASTNodeType::FunctionDeclaration:
            visitFunctionDeclarationNode(static_cast<FunctionDeclarationNode*>(node));
            break;
        case ASTNodeType::VariableDeclaration:
            visitVariableDeclarationNode(static_cast<VariableDeclarationNode*>(node));
            break;
        case ASTNodeType::AssignmentStatement:
            visitAssignmentStatementNode(static_cast<AssignmentStatementNode*>(node));
            break;
        case ASTNodeType::BinaryExpression:
            visitBinaryExpressionNode(static_cast<BinaryExpressionNode*>(node));
            break;
        case ASTNodeType::Literal:
            visitLiteralNode(static_cast<LiteralNode*>(node));
            break;
        case ASTNodeType::Identifier:
            visitIdentifierNode(static_cast<IdentifierNode*>(node));
            break;
        case ASTNodeType::IfStatement:
            visitIfStatementNode(static_cast<IfStatementNode*>(node));
            break;
        case ASTNodeType::ForStatement:
            visitForStatementNode(static_cast<ForStatementNode*>(node));
            break;
        case ASTNodeType::ReturnStatement:
            visitReturnStatementNode(static_cast<ReturnStatementNode*>(node));
            break;
        case ASTNodeType::FunctionCall:
            visitFunctionCallNode(static_cast<FunctionCallNode*>(node));
            break;
        case ASTNodeType::PrintStatement:
            visitPrintStatementNode(static_cast<PrintStatementNode*>(node));
            break;
        case ASTNodeType::BlockStatement:
            visitBlockStatementNode(static_cast<BlockStatementNode*>(node));
            break;
        default:
            // Manejar tipos de nodos no reconocidos o ignorar
            break;
    }
}

void SemanticAnalyzer::visitProgramNode(ProgramNode* node) {
    for (const auto& child : node->children) {
        visit(child.get());
    }
}

void SemanticAnalyzer::visitFunctionDeclarationNode(FunctionDeclarationNode* node) {
    // 1. Añadir la función a la tabla de símbolos del scope global.
    auto funcSymbol = std::make_unique<Symbol>(Symbol::SymbolType::FUNCTION, node->name, node->returnType);
    for (const auto& param : node->parameters) {
        funcSymbol->paramTypes.push_back(param.first); // Añadir el tipo del parámetro
    }

    if (!symbolTable.addSymbol(node->name, std::move(funcSymbol))) {
        ErrorHandler::reportError("Error semántico: Redefinición de la función '" + node->name + "'.");
        hasErrors = true;
    }

    // 2. Entrar en un nuevo scope para los parámetros y variables locales.
    symbolTable.enterScope();

    // 3. Añadir parámetros a la tabla de símbolos del scope actual.
    for (const auto& param : node->parameters) {
        auto paramSymbol = std::make_unique<Symbol>(Symbol::SymbolType::VARIABLE, param.second, param.first);
        if (!symbolTable.addSymbol(param.second, std::move(paramSymbol))) {
            ErrorHandler::reportError("Error semántico: Redefinición del parámetro '" + param.second + "' en la función '" + node->name + "'.");
            hasErrors = true;
        }
    }

    // 4. Visitar el cuerpo de la función.
    for (const auto& child : node->children) {
        visit(child.get());
    }

    // 5. Salir del scope de la función.
    symbolTable.exitScope();
}

void SemanticAnalyzer::visitVariableDeclarationNode(VariableDeclarationNode* node) {
    // 1. Comprobar si la variable ya está declarada en el scope actual.
    if (symbolTable.lookupSymbolInCurrentScope(node->variableName)) {
        ErrorHandler::reportError("Error semántico: Redefinición de la variable '" + node->variableName + "'.");
        hasErrors = true;
    } else {
        // 2. Añadir la variable a la tabla de símbolos.
        symbolTable.addSymbol(node->variableName, std::make_unique<Symbol>(Symbol::SymbolType::VARIABLE, node->variableName, node->typeName));
    }

    // 3. Si hay un inicializador, visitarlo y comprobar tipos.
    if (node->initializer) {
        visit(node->initializer.get());
        std::string exprType = getType(node->initializer.get());
        // Simple type check: si son diferentes de "int", reportar error (ej. int x = "hello";)
        if (node->typeName != exprType && exprType != "unknown") { // "unknown" para errores propagados
            ErrorHandler::reportError("Error semántico: Incompatibilidad de tipos en la inicialización de '" + node->variableName + "'. Se esperaba '" + node->typeName + "', se obtuvo '" + exprType + "'.");
            hasErrors = true;
        }
    }
}

void SemanticAnalyzer::visitAssignmentStatementNode(AssignmentStatementNode* node) {
    // 1. Comprobar si el identificador ha sido declarado.
    Symbol* varSymbol = symbolTable.lookupSymbol(node->identifierName);
    if (!varSymbol) {
        ErrorHandler::reportError("Error semántico: Uso de variable no declarada '" + node->identifierName + "'.");
        hasErrors = true;
    } else if (varSymbol->type != Symbol::SymbolType::VARIABLE) {
        ErrorHandler::reportError("Error semántico: '" + node->identifierName + "' no es una variable.");
        hasErrors = true;
    }

    // 2. Visitar la expresión de asignación y comprobar la compatibilidad de tipos.
    visit(node->expression.get());
    if (varSymbol) {
        std::string exprType = getType(node->expression.get());
        if (varSymbol->dataType != exprType && exprType != "unknown") {
            ErrorHandler::reportError("Error semántico: Incompatibilidad de tipos en la asignación de '" + node->identifierName + "'. Se esperaba '" + varSymbol->dataType + "', se obtuvo '" + exprType + "'.");
            hasErrors = true;
        }
    }
}

void SemanticAnalyzer::visitBinaryExpressionNode(BinaryExpressionNode* node) {
    visit(node->left.get());
    visit(node->right.get());

    std::string leftType = getType(node->left.get());
    std::string rightType = getType(node->right.get());

    // Reglas de tipo simples: ambos operandos deben ser "int" para operaciones aritméticas/comparación
    if (leftType != "int" || rightType != "int") {
        ErrorHandler::reportError("Error semántico: Operación binaria '" + node->op + "' con tipos incompatibles: '" + leftType + "' y '" + rightType + "'.");
        hasErrors = true;
    }
    // Asignar un tipo de retorno al nodo para futuras comprobaciones
    // En un compilador completo, esto se guardaría en el nodo AST.
    // Para simplificar, asumimos que el resultado es 'int' o 'bool' (pero en C, 'bool' es 'int').
}

void SemanticAnalyzer::visitLiteralNode(LiteralNode* node) {
    // No hay mucho que hacer aquí, el tipo se determina por el valor del literal
    // (ej. "10" es int, "\"hello\"" es string). Este tipo se usa en getType().
}

void SemanticAnalyzer::visitIdentifierNode(IdentifierNode* node) {
    // Comprobar si el identificador ha sido declarado.
    if (!symbolTable.lookupSymbol(node->name)) {
        ErrorHandler::reportError("Error semántico: Uso de identificador no declarado '" + node->name + "'.");
        hasErrors = true;
    }
}

void SemanticAnalyzer::visitIfStatementNode(IfStatementNode* node) {
    visit(node->condition.get());
    // La condición debe ser de tipo "int" (o convertible a bool en C)
    std::string condType = getType(node->condition.get());
    if (condType != "int") {
        ErrorHandler::reportWarning("Advertencia semántica: La condición 'if' no es de tipo 'int'. Se obtuvo '" + condType + "'.");
    }

    // Visitar el bloque then
    symbolTable.enterScope(); // Nuevo scope para el bloque then
    visit(node->thenBlock.get());
    symbolTable.exitScope();

    // Visitar el bloque else (si existe)
    if (node->elseBlock) {
        symbolTable.enterScope(); // Nuevo scope para el bloque else
        visit(node->elseBlock.get());
        symbolTable.exitScope();
    }
}

void SemanticAnalyzer::visitForStatementNode(ForStatementNode* node) {
    symbolTable.enterScope(); // Nuevo scope para el bucle for (inicialización, variables de bucle)

    if (node->initialization) {
        visit(node->initialization.get());
    }
    if (node->condition) {
        visit(node->condition.get());
        std::string condType = getType(node->condition.get());
        if (condType != "int") {
            ErrorHandler::reportWarning("Advertencia semántica: La condición 'for' no es de tipo 'int'. Se obtuvo '" + condType + "'.");
        }
    }
    if (node->increment) {
        visit(node->increment.get());
    }
    visit(node->body.get());

    symbolTable.exitScope(); // Salir del scope del bucle for
}

void SemanticAnalyzer::visitReturnStatementNode(ReturnStatementNode* node) {
    if (node->expression) {
        visit(node->expression.get());
        // Aquí se necesitaría saber el tipo de retorno de la función actual
        // Esto implicaría pasar el tipo de retorno de la función a este método
        // o almacenar el tipo de retorno de la función actual en un campo de la clase SemanticAnalyzer.
        // Por simplicidad, omitimos la comprobación de tipo de retorno por ahora.
        // Ejemplo: Si la función actual es 'void', no debería tener expresión de retorno.
        // Si es 'int', la expresión debe ser 'int'.
    }
}

void SemanticAnalyzer::visitFunctionCallNode(FunctionCallNode* node) {
    Symbol* funcSymbol = symbolTable.lookupSymbol(node->functionName);
    if (!funcSymbol) {
        ErrorHandler::reportError("Error semántico: Llamada a función no declarada '" + node->functionName + "'.");
        hasErrors = true;
        return;
    }
    if (funcSymbol->type != Symbol::SymbolType::FUNCTION) {
        ErrorHandler::reportError("Error semántico: '" + node->functionName + "' no es una función.");
        hasErrors = true;
        return;
    }

    // Comprobar el número de argumentos
    if (node->arguments.size() != funcSymbol->paramTypes.size()) {
        ErrorHandler::reportError("Error semántico: Número incorrecto de argumentos para la función '" + node->functionName + "'. Se esperaban " + std::to_string(funcSymbol->paramTypes.size()) + ", se obtuvieron " + std::to_string(node->arguments.size()) + ".");
        hasErrors = true;
    }

    // Comprobar los tipos de los argumentos
    for (size_t i = 0; i < node->arguments.size() && i < funcSymbol->paramTypes.size(); ++i) {
        visit(node->arguments[i].get());
        std::string argType = getType(node->arguments[i].get());
        if (argType != funcSymbol->paramTypes[i] && argType != "unknown") {
            ErrorHandler::reportError("Error semántico: Tipo de argumento incompatible para el parámetro " + std::to_string(i + 1) + " de la función '" + node->functionName + "'. Se esperaba '" + funcSymbol->paramTypes[i] + "', se obtuvo '" + argType + "'.");
            hasErrors = true;
        }
    }
}

void SemanticAnalyzer::visitPrintStatementNode(PrintStatementNode* node) {
    // Comprobar la cadena de formato (simplificado)
    // En un compilador real, se analizarían los especificadores de formato (%d, %s)
    // y se compararían con los tipos de los argumentos.
    if (node->formatString.empty()) {
        ErrorHandler::reportWarning("Advertencia semántica: Cadena de formato vacía en printf.");
    }

    for (const auto& arg : node->arguments) {
        visit(arg.get());
        // Puedes añadir comprobaciones aquí para asegurar que los tipos de los argumentos
        // coinciden con lo que se esperaría de los especificadores de formato en la cadena.
        // Por ahora, solo nos aseguramos de que los argumentos sean expresiones válidas.
    }
}

void SemanticAnalyzer::visitBlockStatementNode(BlockStatementNode* node) {
    symbolTable.enterScope(); // Entrar en un nuevo scope para el bloque
    for (const auto& child : node->children) {
        visit(child.get());
    }
    symbolTable.exitScope(); // Salir del scope del bloque
}


// Método auxiliar para determinar el tipo de una expresión
// Esto es una simplificación; idealmente, los nodos AST almacenarían su tipo inferido
std::string SemanticAnalyzer::getType(ASTNode* node) {
    if (!node) return "unknown";

    switch (node->type) {
        case ASTNodeType::Literal: {
            LiteralNode* literal = static_cast<LiteralNode*>(node);
            // Heurística simple para determinar el tipo de literal
            if (!literal->value.empty() && literal->value[0] == '"') {
                return "string";
            }
            // Asumimos que los números son enteros por ahora
            return "int";
        }
        case ASTNodeType::Identifier: {
            IdentifierNode* id = static_cast<IdentifierNode*>(node);
            Symbol* symbol = symbolTable.lookupSymbol(id->name);
            if (symbol) {
                return symbol->dataType;
            }
            return "unknown"; // No declarado
        }
        case ASTNodeType::BinaryExpression:
            // Para operaciones binarias, si ambos son int, el resultado es int
            // (simplificación, no maneja promoción de tipos)
            if (getType(static_cast<BinaryExpressionNode*>(node)->left.get()) == "int" &&
                getType(static_cast<BinaryExpressionNode*>(node)->right.get()) == "int") {
                return "int";
            }
            return "unknown";
        case ASTNodeType::FunctionCall: {
            FunctionCallNode* call = static_cast<FunctionCallNode*>(node);
            Symbol* funcSymbol = symbolTable.lookupSymbol(call->functionName);
            if (funcSymbol && funcSymbol->type == Symbol::SymbolType::FUNCTION) {
                return funcSymbol->dataType; // Tipo de retorno de la función
            }
            return "unknown";
        }
        default:
            return "unknown"; // Tipo desconocido para otros nodos
    }
}