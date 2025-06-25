// src/semantic_analyzer/SemanticAnalyzer.cpp
#include "SemanticAnalyzer.h"
#include <iostream> // Para depuración

// Constructor
SemanticAnalyzer::SemanticAnalyzer(ErrorHandler& errorHandler)
    : errorHandler(errorHandler) {
    // El constructor de SymbolTable ya se llamará por defecto.
}

void SemanticAnalyzer::analyze(ProgramNode* program) {
    if (!program) {
        errorHandler.reportError("AST del programa es nulo. No se puede realizar el análisis semántico.", -1, -1); // <--- ¡ORDEN CORREGIDO!
        return;
    }
    visitProgramNode(program);
}

void SemanticAnalyzer::visit(ASTNode* node) {
    if (!node) {
        return;
    }

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
        case ASTNodeType::FunctionCall:
            visitFunctionCallNode(static_cast<FunctionCallNode*>(node));
            break;
        case ASTNodeType::ReturnStatement:
            visitReturnStatementNode(static_cast<ReturnStatementNode*>(node));
            break;
        case ASTNodeType::IfStatement:
            visitIfStatementNode(static_cast<IfStatementNode*>(node));
            break;
        case ASTNodeType::ForStatement:
            visitForStatementNode(static_cast<ForStatementNode*>(node));
            break;
        case ASTNodeType::PrintStatement:
            visitPrintStatementNode(static_cast<PrintStatementNode*>(node));
            break;
        case ASTNodeType::BlockStatement:
            visitBlockStatementNode(static_cast<BlockStatementNode*>(node));
            break;
        case ASTNodeType::BinaryExpression:
        case ASTNodeType::UnaryExpression:
        case ASTNodeType::Literal:
        case ASTNodeType::Identifier:
            // Estos son nodos de expresión, que deberían ser manejados por analyzeExpression.
            // Si llegan aquí directamente, significa un error en la traversía.
            errorHandler.reportError("Error interno: Nodo de expresión visitado directamente en SemanticAnalyzer::visit().", -1, -1); // <--- ¡ORDEN CORREGIDO!
            analyzeExpression(node); // Aún así, intentamos analizar la expresión
            break;
        default:
            errorHandler.reportError("Nodo AST desconocido en SemanticAnalyzer::visit(): " + std::to_string(static_cast<int>(node->type)), -1, -1); // <--- ¡ORDEN CORREGIDO!
            break;
    }
}

void SemanticAnalyzer::visitProgramNode(ProgramNode* node) {
    // No es necesario enterScope() aquí, el constructor de SymbolTable ya lo hace.

    // 1. Analizar y registrar declaraciones de funciones (solo firmas)
    for (const auto& funcDeclNode : node->functionDeclarations) {
        auto func = static_cast<FunctionDeclarationNode*>(funcDeclNode.get());
        // Crear un Symbol para la función y añadirlo a la tabla
        auto funcSymbol = std::make_unique<Symbol>(func->name, SymbolType::FUNCTION, func->returnType, func->parameters); // <--- ¡CONSTRUCTOR DE SYMBOL ACTUALIZADO!
        if (!symbolTable.addSymbol(std::move(funcSymbol))) { // <--- ¡LLAMADA A addSymbol MODIFICADA!
            errorHandler.reportError("Redeclaración de función: " + func->name, -1, -1); // <--- ¡ORDEN CORREGIDO!
        }
    }

    // 2. Analizar cuerpos de funciones y sentencias globales/main
    // Esto se hace en dos pasadas para permitir llamadas entre funciones declaradas.
    for (const auto& funcDeclNode : node->functionDeclarations) {
        visitFunctionDeclarationNode(static_cast<FunctionDeclarationNode*>(funcDeclNode.get()));
    }

    // Luego analizar las sentencias globales (si las hay, asumiendo que están en 'statements' del ProgramNode)
    for (const auto& stmtNode : node->statements) {
        visit(stmtNode.get()); // Analizar sentencias globales/del main directamente
    }

    // No es necesario exitScope() aquí, ya que es el ámbito más externo.
}

void SemanticAnalyzer::visitFunctionDeclarationNode(FunctionDeclarationNode* node) {
    symbolTable.enterScope(); // Entrar en el ámbito de la función

    currentFunctionReturnType = node->returnType; // Guardar el tipo de retorno de la función actual

    // Registrar parámetros de la función en el ámbito local
    for (const auto& param : node->parameters) {
        // Crear un Symbol para el parámetro y añadirlo
        auto paramSymbol = std::make_unique<Symbol>(param.second, SymbolType::VARIABLE, param.first); // <--- ¡CONSTRUCTOR DE SYMBOL ACTUALIZADO!
        if (!symbolTable.addSymbol(std::move(paramSymbol))) { // <--- ¡LLAMADA A addSymbol MODIFICADA!
            errorHandler.reportError("Redeclaración de parámetro: " + param.second, -1, -1); // <--- ¡ORDEN CORREGIDO!
        }
    }

    // Analizar el cuerpo de la función
    if (node->body) {
        visitBlockStatementNode(static_cast<BlockStatementNode*>(node->body.get()));
    } else {
        errorHandler.reportWarning("Cuerpo de función nulo para: " + node->name, -1, -1); // <--- ¡ORDEN CORREGIDO!
    }

    currentFunctionReturnType = ""; // Limpiar el tipo de retorno de la función actual al salir
    symbolTable.exitScope(); // Salir del ámbito de la función
}

void SemanticAnalyzer::visitVariableDeclarationNode(VariableDeclarationNode* node) {
    // Verificar si la variable ya existe en el ámbito actual
    if (symbolTable.lookupSymbolInCurrentScope(node->variableName)) {
        errorHandler.reportError("Redeclaración de variable en el mismo ámbito: " + node->variableName, -1, -1); // <--- ¡ORDEN CORREGIDO!
    } else {
        // Añadir la variable a la tabla de símbolos
        auto varSymbol = std::make_unique<Symbol>(node->variableName, SymbolType::VARIABLE, node->typeName); // <--- ¡CONSTRUCTOR DE SYMBOL ACTUALIZADO!
        symbolTable.addSymbol(std::move(varSymbol)); // <--- ¡LLAMADA A addSymbol MODIFICADA!
    }

    // Si hay un inicializador, analizar la expresión
    if (node->initializer) {
        if (!analyzeExpression(node->initializer.get())) {
            errorHandler.reportError("Error en la expresión inicializadora de la variable: " + node->variableName, -1, -1); // <--- ¡ORDEN CORREGIDO!
        }
        // TODO: Verificar compatibilidad de tipos entre typeName y el tipo de la expresión inicializadora
    }
}

void SemanticAnalyzer::visitAssignmentStatementNode(AssignmentStatementNode* node) {
    // Verificar si el identificador ha sido declarado
    if (!symbolTable.lookupSymbol(node->identifierName)) {
        errorHandler.reportError("Uso de variable no declarada: " + node->identifierName, -1, -1); // <--- ¡ORDEN CORREGIDO!
    }

    // Analizar la expresión del lado derecho de la asignación
    if (!analyzeExpression(node->expression.get())) {
        errorHandler.reportError("Error en la expresión de asignación para: " + node->identifierName, -1, -1); // <--- ¡ORDEN CORREGIDO!
    }
    // TODO: Verificar compatibilidad de tipos entre la variable y la expresión
}

void SemanticAnalyzer::visitFunctionCallNode(FunctionCallNode* node) {
    // Buscar la función en la tabla de símbolos
    auto funcSymbol = symbolTable.lookupSymbol(node->functionName);
    if (!funcSymbol || funcSymbol->symbolType != SymbolType::FUNCTION) { // <--- ¡USO DE symbolType!
        errorHandler.reportError("Función no declarada o no es una función: " + node->functionName, -1, -1); // <--- ¡ORDEN CORREGIDO!
        return;
    }

    // Verificar el número de argumentos
    if (node->arguments.size() != funcSymbol->parameters.size()) { // <--- ¡USO DE parameters!
        errorHandler.reportError("Número incorrecto de argumentos para la función '" + node->functionName + "'. Se esperaban " +
                                 std::to_string(funcSymbol->parameters.size()) + ", se obtuvieron " + std::to_string(node->arguments.size()) + ".", -1, -1); // <--- ¡ORDEN CORREGIDO!
    }

    // Analizar cada argumento y verificar tipos (simplificado)
    for (size_t i = 0; i < node->arguments.size(); ++i) {
        if (!analyzeExpression(node->arguments[i].get())) {
            errorHandler.reportError("Error en el argumento " + std::to_string(i + 1) + " de la función " + node->functionName, -1, -1); // <--- ¡ORDEN CORREGIDO!
        }
        // TODO: Comparar el tipo del argumento con el tipo del parámetro esperado
        // if (i < funcSymbol->parameters.size() && inferred_arg_type != funcSymbol->parameters[i].first) { ... }
    }
}

void SemanticAnalyzer::visitReturnStatementNode(ReturnStatementNode* node) {
    if (node->expression) {
        // Si hay una expresión de retorno, analizarla
        if (!analyzeExpression(node->expression.get())) {
            errorHandler.reportError("Error en la expresión de retorno.", -1, -1); // <--- ¡ORDEN CORREGIDO!
        }
        // TODO: Verificar que el tipo de la expresión de retorno coincida con currentFunctionReturnType
        // Si currentFunctionReturnType es "void" y hay una expresión, reportar error.
        // Si currentFunctionReturnType no es "void" y no hay expresión, reportar error.
    } else {
        // No hay expresión de retorno (es un 'return;')
        if (currentFunctionReturnType != "void") {
            errorHandler.reportError("La función '" + currentFunctionReturnType + "' espera un valor de retorno.", -1, -1); // <--- ¡ORDEN CORREGIDO!
        }
    }
}

void SemanticAnalyzer::visitIfStatementNode(IfStatementNode* node) {
    // Analizar la condición del if
    if (!analyzeExpression(node->condition.get())) {
        errorHandler.reportError("Error en la condición del 'if'.", -1, -1); // <--- ¡ORDEN CORREGIDO!
    }
    // TODO: La condición debe evaluarse a un tipo booleano o comparable a booleano.

    // Analizar el bloque then
    if (node->thenBlock) {
        symbolTable.enterScope(); // El bloque 'then' tiene su propio ámbito
        visitBlockStatementNode(static_cast<BlockStatementNode*>(node->thenBlock.get()));
        symbolTable.exitScope();
    }

    // Analizar el bloque else si existe
    if (node->elseBlock) {
        symbolTable.enterScope(); // El bloque 'else' tiene su propio ámbito
        visitBlockStatementNode(static_cast<BlockStatementNode*>(node->elseBlock.get()));
        symbolTable.exitScope();
    }
}

void SemanticAnalyzer::visitForStatementNode(ForStatementNode* node) {
    symbolTable.enterScope(); // El bucle 'for' tiene su propio ámbito

    // Analizar la inicialización
    if (node->initialization) {
        visit(node->initialization.get()); // Puede ser declaración o asignación
    }

    // Analizar la condición
    if (node->condition) {
        if (!analyzeExpression(node->condition.get())) {
            errorHandler.reportError("Error en la condición del bucle 'for'.", -1, -1); // <--- ¡ORDEN CORREGIDO!
        }
        // TODO: La condición debe evaluarse a un tipo booleano
    }

    // Analizar el incremento
    if (node->increment) {
        if (!analyzeExpression(node->increment.get())) {
            errorHandler.reportError("Error en la expresión de incremento del bucle 'for'.", -1, -1); // <--- ¡ORDEN CORREGIDO!
        }
    }

    // Analizar el cuerpo del bucle
    if (node->body) {
        visitBlockStatementNode(static_cast<BlockStatementNode*>(node->body.get()));
    }

    symbolTable.exitScope(); // Salir del ámbito del bucle 'for'
}

void SemanticAnalyzer::visitPrintStatementNode(PrintStatementNode* node) {
    // Analizar la cadena de formato (no hay verificación de tipo real aquí)
    // El formato en C es complejo, esto es solo un placeholder.
    if (node->formatString.empty()) {
        errorHandler.reportWarning("printf sin cadena de formato.", -1, -1); // <--- ¡ORDEN CORREGIDO!
    }

    // Analizar los argumentos
    for (const auto& arg : node->arguments) {
        if (!analyzeExpression(arg.get())) {
            errorHandler.reportError("Error en un argumento de printf.", -1, -1); // <--- ¡ORDEN CORREGIDO!
        }
        // TODO: Se podría intentar verificar la concordancia de tipos con los especificadores de formato en formatString.
    }
}

void SemanticAnalyzer::visitBlockStatementNode(BlockStatementNode* node) {
    // Los ámbitos para los bloques {} ya se manejan en visitIfStatementNode, visitForStatementNode, etc.
    // Aquí solo se visitan las sentencias dentro del bloque.
    for (const auto& stmt : node->statements) {
        visit(stmt.get());
    }
}


// --- Métodos de Análisis de Expresiones ---
// Estos métodos deberían inferir el tipo de la expresión y devolverlo (ej. "int", "bool").
// Por simplicidad, por ahora solo devuelven 'true' si la expresión es sintácticamente válida.

bool SemanticAnalyzer::analyzeExpression(ASTNode* node) {
    if (!node) {
        return false;
    }

    switch (node->type) {
        case ASTNodeType::Identifier:
            return visitIdentifierNode(static_cast<IdentifierNode*>(node));
        case ASTNodeType::Literal:
            return visitLiteralNode(static_cast<LiteralNode*>(node));
        case ASTNodeType::BinaryExpression:
            return visitBinaryExpressionNode(static_cast<BinaryExpressionNode*>(node));
        case ASTNodeType::UnaryExpression:
            return visitUnaryExpressionNode(static_cast<UnaryExpressionNode*>(node));
        case ASTNodeType::FunctionCall:
            // Si una llamada a función es una expresión (ej. int x = func();)
            visitFunctionCallNode(static_cast<FunctionCallNode*>(node));
            // TODO: Devolver el tipo de retorno de la función
            return true;
        default:
            errorHandler.reportError("Tipo de nodo desconocido o no esperado como expresión: " + std::to_string(static_cast<int>(node->type)), -1, -1); // <--- ¡ORDEN CORREGIDO!
            return false;
    }
}

bool SemanticAnalyzer::visitIdentifierNode(IdentifierNode* node) {
    // Verificar si el identificador ha sido declarado
    if (!symbolTable.lookupSymbol(node->name)) {
        errorHandler.reportError("Uso de identificador no declarado: " + node->name, -1, -1); // <--- ¡ORDEN CORREGIDO!
        return false;
    }
    // TODO: Devolver el tipo del identificador
    return true;
}

bool SemanticAnalyzer::visitLiteralNode(LiteralNode* node) {
    // Los literales son siempre válidos sintácticamente
    // TODO: Devolver el tipo del literal (ej. "int" para "10", "string" para "\"hola\"")
    return true;
}

bool SemanticAnalyzer::visitBinaryExpressionNode(BinaryExpressionNode* node) {
    bool leftValid = analyzeExpression(node->left.get());
    bool rightValid = analyzeExpression(node->right.get());

    if (!leftValid || !rightValid) {
        return false;
    }

    // TODO: Verificar la compatibilidad de tipos entre left y right para 'op'.
    // Ej. no puedes sumar int + string.
    return true;
}

bool SemanticAnalyzer::visitUnaryExpressionNode(UnaryExpressionNode* node) {
    bool operandValid = analyzeExpression(node->operand.get());
    if (!operandValid) {
        return false;
    }
    // TODO: Verificar que el operador unario sea aplicable al tipo del operando.
    return true;
}