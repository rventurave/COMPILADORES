#include "CodeGenerator.h"

// Constructor
CodeGenerator::CodeGenerator(SFMLTranslator& translator)
    : translator(translator) {}

// Método principal para iniciar la generación de código.
std::string CodeGenerator::generate(ASTNode* root) {
    if (!root) {
        return "";
    }
    std::stringstream ss;
    ss << translator.getSFMLHeader();

    // Generar las funciones C/SFML
    for (const auto& child : root->children) {
        if (child->type == ASTNodeType::FunctionDeclaration) {
            ss << visit(child.get());
        }
    }

    // Generar el main de SFML y llamar a la función 'main' del código C
    ss << translator.generateSFMLMainStart();
    translator.increaseIndent(); // Indentar el contenido del while(window.isOpen())
    translator.increaseIndent(); // Indentar el contenido después del comentario de inicio de simulación

    // Llamar a la función principal del programa C (asumiendo que siempre hay un 'main')
    ss << translator.generateFunctionCall("main", {}); // Llama a la función main del código C
    ss << translator.generateSFMLMainEnd();
    translator.decreaseIndent(); // Desindentar antes del return 0
    translator.decreaseIndent(); // Desindentar el contenido del while(window.isOpen())

    return ss.str();
}

// Método de dispatch para visitar diferentes tipos de nodos AST
std::string CodeGenerator::visit(ASTNode* node) {
    if (!node) return "";

    std::stringstream ss;
    switch (node->type) {
        case ASTNodeType::Program:
            // El ProgramNode se maneja en el método generate() principal
            break;
        case ASTNodeType::FunctionDeclaration:
            ss << visitFunctionDeclarationNode(static_cast<FunctionDeclarationNode*>(node));
            break;
        case ASTNodeType::VariableDeclaration:
            ss << visitVariableDeclarationNode(static_cast<VariableDeclarationNode*>(node));
            break;
        case ASTNodeType::AssignmentStatement:
            ss << visitAssignmentStatementNode(static_cast<AssignmentStatementNode*>(node));
            break;
        case ASTNodeType::BinaryExpression:
            // Esto se genera como parte de una expresión, no como una sentencia independiente.
            // El `generateExpression` se encargará de ello.
            break;
        case ASTNodeType::Literal:
            // Esto se genera como parte de una expresión
            break;
        case ASTNodeType::Identifier:
            // Esto se genera como parte de una expresión
            break;
        case ASTNodeType::IfStatement:
            ss << visitIfStatementNode(static_cast<IfStatementNode*>(node));
            break;
        case ASTNodeType::ForStatement:
            ss << visitForStatementNode(static_cast<ForStatementNode*>(node));
            break;
        case ASTNodeType::ReturnStatement:
            ss << visitReturnStatementNode(static_cast<ReturnStatementNode*>(node));
            break;
        case ASTNodeType::FunctionCall:
            ss << translator.generateFunctionCall(static_cast<FunctionCallNode*>(node)->functionName, {}); // Solo el nombre, args handled by generateExpression
            break;
        case ASTNodeType::PrintStatement:
            ss << visitPrintStatementNode(static_cast<PrintStatementNode*>(node));
            break;
        case ASTNodeType::BlockStatement:
            ss << visitBlockStatementNode(static_cast<BlockStatementNode*>(node));
            break;
        default:
            // Manejar tipos de nodos no reconocidos o ignorar
            break;
    }
    return ss.str();
}

std::string CodeGenerator::visitProgramNode(ProgramNode* node) {
    // Ya manejado en el método `generate` principal
    return "";
}

std::string CodeGenerator::visitFunctionDeclarationNode(FunctionDeclarationNode* node) {
    std::stringstream ss;
    ss << std::endl << translator.getCurrentIndent() << node->returnType << " " << node->name << "(";
    // Parámetros
    for (size_t i = 0; i < node->parameters.size(); ++i) {
        ss << node->parameters[i].first << " " << node->parameters[i].second;
        if (i < node->parameters.size() - 1) {
            ss << ", ";
        }
    }
    ss << ") {" << std::endl;
    translator.increaseIndent();
    ss << translator.generateFunctionEntry(node->name, node->parameters); // Visualizar entrada a función

    for (const auto& child : node->children) {
        ss << visit(child.get());
    }
    translator.decreaseIndent();
    ss << translator.getCurrentIndent() << "}" << std::endl;
    return ss.str();
}

std::string CodeGenerator::visitVariableDeclarationNode(VariableDeclarationNode* node) {
    std::string initialValue = "";
    if (node->initializer) {
        initialValue = generateExpression(node->initializer.get());
    }
    return translator.generateVariableDeclaration(node->typeName, node->variableName, initialValue);
}

std::string CodeGenerator::visitAssignmentStatementNode(AssignmentStatementNode* node) {
    std::string exprCode = generateExpression(node->expression.get());
    return translator.generateAssignment(node->identifierName, exprCode);
}

std::string CodeGenerator::visitBinaryExpressionNode(BinaryExpressionNode* node) {
    // Este método solo se llama internamente por generateExpression para construir partes de expresiones.
    return generateExpression(node->left.get()) + " " + node->op + " " + generateExpression(node->right.get());
}

std::string CodeGenerator::visitLiteralNode(LiteralNode* node) {
    // Retorna el valor literal tal cual
    return node->value;
}

std::string CodeGenerator::visitIdentifierNode(IdentifierNode* node) {
    // Retorna el nombre del identificador
    return node->name;
}

std::string CodeGenerator::visitIfStatementNode(IfStatementNode* node) {
    std::stringstream ss;
    ss << translator.generateIfStart(generateExpression(node->condition.get()));
    ss << visit(node->thenBlock.get());
    if (node->elseBlock) {
        ss << translator.generateElseStart();
        ss << visit(node->elseBlock.get());
    }
    ss << translator.generateBlockEnd();
    return ss.str();
}

std::string CodeGenerator::visitForStatementNode(ForStatementNode* node) {
    std::stringstream ss;
    std::string initCode = "";
    if (node->initialization) {
        // Para for, la inicialización puede ser una declaración o asignación.
        // Aquí solo extraemos el código C subyacente.
        if (node->initialization->type == ASTNodeType::VariableDeclaration) {
            auto declNode = static_cast<VariableDeclarationNode*>(node->initialization.get());
            initCode = declNode->typeName + " " + declNode->variableName;
            if (declNode->initializer) {
                initCode += " = " + generateExpression(declNode->initializer.get());
            }
        } else if (node->initialization->type == ASTNodeType::AssignmentStatement) {
            auto assignNode = static_cast<AssignmentStatementNode*>(node->initialization.get());
            initCode = assignNode->identifierName + " = " + generateExpression(assignNode->expression.get());
        }
    }
    std::string condCode = generateExpression(node->condition.get());
    std::string incCode = generateExpression(node->increment.get());

    ss << translator.generateForStart(initCode, condCode, incCode);
    ss << visit(node->body.get());
    ss << translator.generateBlockEnd();
    return ss.str();
}

std::string CodeGenerator::visitReturnStatementNode(ReturnStatementNode* node) {
    std::stringstream ss;
    std::string returnValue = "";
    if (node->expression) {
        returnValue = generateExpression(node->expression.get());
        ss << translator.getCurrentIndent() << "return " << returnValue << ";" << std::endl;
    } else {
        ss << translator.getCurrentIndent() << "return;" << std::endl;
    }
    ss << translator.generateFunctionExit("current_function_name_placeholder", returnValue); // Necesita saber el nombre de la función actual
    return ss.str();
}

std::string CodeGenerator::visitFunctionCallNode(FunctionCallNode* node) {
    std::vector<std::string> args;
    for (const auto& arg : node->arguments) {
        args.push_back(generateExpression(arg.get()));
    }
    return translator.generateFunctionCall(node->functionName, args);
}

std::string CodeGenerator::visitPrintStatementNode(PrintStatementNode* node) {
    std::vector<std::string> args;
    for (const auto& arg : node->arguments) {
        args.push_back(generateExpression(arg.get()));
    }
    return translator.generatePrint(node->formatString, args);
}

std::string CodeGenerator::visitBlockStatementNode(BlockStatementNode* node) {
    std::stringstream ss;
    ss << translator.getCurrentIndent() << "{" << std::endl;
    translator.increaseIndent();
    for (const auto& child : node->children) {
        ss << visit(child.get());
    }
    translator.decreaseIndent();
    ss << translator.getCurrentIndent() << "}" << std::endl;
    return ss.str();
}

// Método auxiliar para construir expresiones de forma recursiva
std::string CodeGenerator::generateExpression(ASTNode* node) {
    if (!node) return "";

    switch (node->type) {
        case ASTNodeType::Literal:
            return static_cast<LiteralNode*>(node)->value;
        case ASTNodeType::Identifier:
            return static_cast<IdentifierNode*>(node)->name;
        case ASTNodeType::BinaryExpression: {
            BinaryExpressionNode* binExpr = static_cast<BinaryExpressionNode*>(node);
            return generateExpression(binExpr->left.get()) + " " + binExpr->op + " " + generateExpression(binExpr->right.get());
        }
        case ASTNodeType::FunctionCall: {
            FunctionCallNode* call = static_cast<FunctionCallNode*>(node);
            std::stringstream ss;
            ss << call->functionName << "(";
            for (size_t i = 0; i < call->arguments.size(); ++i) {
                ss << generateExpression(call->arguments[i].get());
                if (i < call->arguments.size() - 1) {
                    ss << ", ";
                }
            }
            ss << ")";
            return ss.str();
        }
        default:
            // Para otros nodos que no deberían ser parte de una expresión directamente
            return "";
    }
}