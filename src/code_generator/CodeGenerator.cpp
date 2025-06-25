// src/code_generator/CodeGenerator.cpp
#include "CodeGenerator.h"
#include <iostream>
#include <vector>
#include <utility> // Para std::move

// Incluir los encabezados de los nodos AST específicos para dynamic_cast
#include "../parser/AST.h"


// Constructor: Ahora recibe ErrorHandler
CodeGenerator::CodeGenerator(ErrorHandler& errorHandler)
    : currentFunctionName(""), errorHandler(errorHandler) { // <--- ¡CONSTRUCTOR MODIFICADO!
    // Constructor
}

std::string CodeGenerator::generate(ProgramNode* program) {
    return visitProgramNode(program);
}

std::string CodeGenerator::visit(ASTNode* node) {
    if (!node) {
        return "";
    }

    switch(node->type) {
        case ASTNodeType::Program:
            return ""; // Logic handled in visitProgramNode
        case ASTNodeType::FunctionDeclaration:
            return visitFunctionDeclarationNode(static_cast<FunctionDeclarationNode*>(node));
        case ASTNodeType::VariableDeclaration:
            return visitVariableDeclarationNode(static_cast<VariableDeclarationNode*>(node));
        case ASTNodeType::AssignmentStatement:
            return visitAssignmentStatementNode(static_cast<AssignmentStatementNode*>(node));
        case ASTNodeType::ReturnStatement:
            return visitReturnStatementNode(static_cast<ReturnStatementNode*>(node));
        case ASTNodeType::IfStatement:
            return visitIfStatementNode(static_cast<IfStatementNode*>(node));
        case ASTNodeType::ForStatement:
            return visitForStatementNode(static_cast<ForStatementNode*>(node));
        case ASTNodeType::PrintStatement:
            return visitPrintStatementNode(static_cast<PrintStatementNode*>(node));
        case ASTNodeType::BlockStatement:
            return visitBlockStatementNode(static_cast<BlockStatementNode*>(node));
        case ASTNodeType::FunctionCall:
            return visitFunctionCallNode(static_cast<FunctionCallNode*>(node));
        case ASTNodeType::BinaryExpression:
        case ASTNodeType::UnaryExpression:
        case ASTNodeType::Literal:
        case ASTNodeType::Identifier:
            return generateExpression(node);
        default:
            errorHandler.reportError("Nodo AST desconocido o inesperado en CodeGenerator::visit(): " + std::to_string(static_cast<int>(node->type)), -1, -1); // <--- Acceso a errorHandler
            return "";
    }
}


std::string CodeGenerator::visitProgramNode(ProgramNode* node) {
    std::stringstream ss;

    ss << translator.getSFMLHeader();

    for (const auto& func : node->functionDeclarations) {
        if (static_cast<FunctionDeclarationNode*>(func.get())->name != "main") {
            ss << visitFunctionDeclarationNode(static_cast<FunctionDeclarationNode*>(func.get()));
            ss << std::endl;
        }
    }

    ss << "void run_c_program_simulation() {" << std::endl;
    translator.increaseIndent();
    ss << translator.generateProgramStart();

    bool main_found = false;
    for (const auto& func : node->functionDeclarations) {
        auto funcDecl = static_cast<FunctionDeclarationNode*>(func.get());
        if (funcDecl->name == "main") {
            main_found = true;
            std::string previousFunctionName = currentFunctionName;
            currentFunctionName = "main";

            ss << translator.generateFunctionEntry("main", {});

            if (funcDecl->body) {
                ss << visit(funcDecl->body.get());
            }

            ss << translator.generateFunctionExit("main", "");

            currentFunctionName = previousFunctionName;
            break;
        }
    }

    if (!main_found) {
        errorHandler.reportWarning("No se encontró la función 'main()' en el código C. Ejecutando sentencias globales si las hay.", -1, -1); // <--- Acceso a errorHandler
        ss << translator.generateFunctionEntry("global_scope", {});
        for (const auto& stmt : node->statements) {
            ss << visit(stmt.get());
        }
        ss << translator.generateFunctionExit("global_scope", "");
    }

    ss << translator.generateProgramEnd();
    translator.decreaseIndent();
    ss << "}" << std::endl;

    ss << translator.getSFMLFooter();

    ss << std::endl;
    ss << "int main() {" << std::endl;
    translator.increaseIndent();
    ss << translator.getCurrentIndent() << "sf::RenderWindow window(sf::VideoMode(900, 600), \"C to SFML Compiler Visualization\");" << std::endl;
    ss << translator.getCurrentIndent() << "setupSFML(window);" << std::endl;
    ss << translator.getCurrentIndent() << "window.setFramerateLimit(60);" << std::endl;

    ss << std::endl;
    ss << translator.getCurrentIndent() << "// --- Inicio de la simulación del código C ---" << std::endl;
    ss << translator.getCurrentIndent() << "run_c_program_simulation(); // Llama a la lógica del programa C simulado" << std::endl;
    ss << translator.getCurrentIndent() << "// --- Fin de la simulación del código C ---" << std::endl;
    ss << std::endl;

    ss << translator.getCurrentIndent() << "// Mantener la ventana abierta después de que la simulación termine," << std::endl;
    ss << translator.getCurrentIndent() << "// esperando que el usuario la cierre." << std::endl;
    ss << translator.getCurrentIndent() << "while (window.isOpen()) {" << std::endl;
    translator.increaseIndent();
    ss << translator.getCurrentIndent() << "sf::Event event;" << std::endl;
    ss << translator.getCurrentIndent() << "while (window.pollEvent(event)) {" << std::endl;
    translator.increaseIndent();
    ss << translator.getCurrentIndent() << "if (event.type == sf::Event::Closed) {" << std::endl;
    translator.increaseIndent();
    ss << translator.getCurrentIndent() << "window.close();" << std::endl;
    translator.decreaseIndent();
    ss << translator.getCurrentIndent() << "}" << std::endl;
    translator.decreaseIndent();
    ss << translator.getCurrentIndent() << "}" << std::endl;
    ss << translator.getCurrentIndent() << "sf::sleep(sf::milliseconds(50)); // Pequeño sleep para reducir el uso de CPU" << std::endl;
    translator.decreaseIndent();
    ss << translator.getCurrentIndent() << "}" << std::endl;

    ss << translator.getCurrentIndent() << "return 0;" << std::endl;
    translator.decreaseIndent();
    ss << "}" << std::endl;

    return ss.str();
}

std::string CodeGenerator::visitFunctionDeclarationNode(FunctionDeclarationNode* node) {
    std::stringstream ss;
    std::string paramsCode;
    std::vector<std::pair<std::string, std::string>> paramsForSFML;

    for (size_t i = 0; i < node->parameters.size(); ++i) {
        paramsCode += node->parameters[i].first + " " + node->parameters[i].second;
        paramsForSFML.push_back({node->parameters[i].first, node->parameters[i].second});
        if (i < node->parameters.size() - 1) {
            paramsCode += ", ";
        }
    }

    ss << translator.getCurrentIndent() << node->returnType << " " << node->name << "(" << paramsCode << ") {" << std::endl;
    translator.increaseIndent();

    std::string previousFunctionName = currentFunctionName;
    currentFunctionName = node->name;

    if (node->body) {
        ss << visit(node->body.get());
    } else {
        errorHandler.reportWarning("Cuerpo de función nulo para: " + node->name, -1, -1); // <--- Acceso a errorHandler
    }

    currentFunctionName = previousFunctionName;
    translator.decreaseIndent();
    ss << translator.getCurrentIndent() << "}" << std::endl;
    return ss.str();
}

std::string CodeGenerator::visitVariableDeclarationNode(VariableDeclarationNode* node) {
    std::stringstream ss;
    std::string initialValueStr;

    if (node->initializer) {
        initialValueStr = generateExpression(node->initializer.get());
    }

    ss << translator.generateVariableDeclaration(node->typeName, node->variableName, initialValueStr);

    if (!initialValueStr.empty()) {
        ss << translator.generateVariableUpdate(node->variableName, initialValueStr);
    } else {
        if (node->typeName == "int") {
            ss << translator.generateVariableUpdate(node->variableName, "0");
        }
    }
    return ss.str();
}

std::string CodeGenerator::visitAssignmentStatementNode(AssignmentStatementNode* node) {
    std::stringstream ss;
    std::string exprCode = generateExpression(node->expression.get());

    ss << translator.generateAssignment(node->identifierName, exprCode);
    ss << translator.generateVariableUpdate(node->identifierName, exprCode);
    return ss.str();
}

std::string CodeGenerator::visitFunctionCallNode(FunctionCallNode* node) {
    std::stringstream ss;
    std::string argsCode;
    std::vector<std::pair<std::string, std::string>> argsForSFML;

    for (size_t i = 0; i < node->arguments.size(); ++i) {
        std::string argExpr = generateExpression(node->arguments[i].get());
        argsCode += argExpr;
        argsForSFML.push_back({"param_type", argExpr});
        if (i < node->arguments.size() - 1) {
            argsCode += ", ";
        }
    }
    ss << translator.generateFunctionEntry(node->functionName, argsForSFML);
    ss << translator.getCurrentIndent() << node->functionName << "(" << argsCode << ");" << std::endl;
    return ss.str();
}

std::string CodeGenerator::visitReturnStatementNode(ReturnStatementNode* node) {
    std::stringstream ss;
    std::string exprCode = "";
    if (node->expression) {
        exprCode = generateExpression(node->expression.get());
    }
    ss << translator.generateReturnStatement(exprCode, currentFunctionName);
    ss << translator.generateFunctionExit(currentFunctionName, exprCode);
    return ss.str();
}

std::string CodeGenerator::visitIfStatementNode(IfStatementNode* node) {
    std::stringstream ss;
    std::string conditionCode = generateExpression(node->condition.get());
    std::string thenBlockCode = visit(node->thenBlock.get());
    std::string elseBlockCode = node->elseBlock ? visit(node->elseBlock.get()) : "";

    ss << translator.generateIfStatement(conditionCode, thenBlockCode, elseBlockCode);
    return ss.str();
}

std::string CodeGenerator::visitForStatementNode(ForStatementNode* node) {
    std::stringstream ss;
    std::string initCode = visit(node->initialization.get());
    std::string conditionCode = generateExpression(node->condition.get());
    std::string updateCode = generateExpression(node->increment.get());
    std::string bodyCode = visit(node->body.get());

    ss << translator.generateForLoop(initCode, conditionCode, updateCode, bodyCode);
    return ss.str();
}

std::string CodeGenerator::visitPrintStatementNode(PrintStatementNode* node) {
    std::stringstream ss;
    std::string printArgs = "\"" + node->formatString + "\"";

    for (const auto& arg : node->arguments) {
        printArgs += ", " + generateExpression(arg.get());
    }
    ss << translator.generatePrintStatement(printArgs);
    return ss.str();
}

std::string CodeGenerator::visitBlockStatementNode(BlockStatementNode* node) {
    std::stringstream ss;
    ss << translator.getCurrentIndent() << "{" << std::endl;
    translator.increaseIndent();

    for (const auto& stmt : node->statements) {
        ss << visit(stmt.get());
    }

    translator.decreaseIndent();
    ss << translator.getCurrentIndent() << "}" << std::endl;
    return ss.str();
}

std::string CodeGenerator::generateExpression(ASTNode* node) {
    if (!node) {
        return "";
    }

    switch(node->type) {
        case ASTNodeType::Identifier:
            return visitIdentifierNode(static_cast<IdentifierNode*>(node));
        case ASTNodeType::Literal:
            return visitLiteralNode(static_cast<LiteralNode*>(node));
        case ASTNodeType::BinaryExpression:
            return visitBinaryExpressionNode(static_cast<BinaryExpressionNode*>(node));
        case ASTNodeType::UnaryExpression:
            return visitUnaryExpressionNode(static_cast<UnaryExpressionNode*>(node));
        case ASTNodeType::FunctionCall:
            {
                auto funcCall = static_cast<FunctionCallNode*>(node);
                std::stringstream ss_args;
                for (size_t i = 0; i < funcCall->arguments.size(); ++i) {
                    ss_args << generateExpression(funcCall->arguments[i].get());
                    if (i < funcCall->arguments.size() - 1) {
                        ss_args << ", ";
                    }
                }
                return funcCall->functionName + "(" + ss_args.str() + ")";
            }
        default:
            errorHandler.reportError("Tipo de nodo desconocido o no esperado como expresión: " + std::to_string(static_cast<int>(node->type)), -1, -1); // <--- Acceso a errorHandler
            return "";
    }
}

std::string CodeGenerator::visitIdentifierNode(IdentifierNode* node) {
    return node->name;
}

std::string CodeGenerator::visitLiteralNode(LiteralNode* node) {
    return node->value;
}

std::string CodeGenerator::visitBinaryExpressionNode(BinaryExpressionNode* node) {
    std::string left = generateExpression(node->left.get());
    std::string right = generateExpression(node->right.get());
    return "(" + left + " " + node->op + " " + right + ")";
}

std::string CodeGenerator::visitUnaryExpressionNode(UnaryExpressionNode* node) {
    std::string operand = generateExpression(node->operand.get());
    return "(" + node->op + operand + ")";
}