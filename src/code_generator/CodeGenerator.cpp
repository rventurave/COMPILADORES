// src/code_generator/CodeGenerator.cpp
#include "CodeGenerator.h" // Incluye CodeGenerator.h, que a su vez incluye SFMLTranslator.h
#include <iostream>
#include <vector>
#include <utility> // Para std::move

// Incluir los encabezados de los nodos AST específicos para dynamic_cast
#include "../parser/AST.h"


// Constructor: Ahora recibe ErrorHandler
CodeGenerator::CodeGenerator(ErrorHandler& errorHandler)
    : currentFunctionName(""), errorHandler(errorHandler) {
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
            return ""; // La lógica se maneja en visitProgramNode
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
            errorHandler.reportError("Nodo AST desconocido o inesperado en CodeGenerator::visit(): " + std::to_string(static_cast<int>(node->type)), -1, -1);
            return "";
    }
}


std::string CodeGenerator::visitProgramNode(ProgramNode* node) {
    std::stringstream ss;

    // Generar el encabezado SFML (includes, variables globales, prototipos)
    ss << translator.getSFMLHeader();

    // Generar las declaraciones de funciones C (excepto main)
    for (const auto& func : node->functionDeclarations) {
        if (static_cast<FunctionDeclarationNode*>(func.get())->name != "main") {
            ss << visitFunctionDeclarationNode(static_cast<FunctionDeclarationNode*>(func.get()));
            ss << std::endl;
        }
    }

    // Generar la función run_c_program_simulation que contiene la lógica del programa C
    ss << "void run_c_program_simulation() {" << std::endl;
    translator.increaseIndent(); // Indentación para el cuerpo de run_c_program_simulation

    ss << translator.generateProgramStart(); // Llama a recordStep("Program Started")

    bool main_found = false;
    for (const auto& func : node->functionDeclarations) {
        auto funcDecl = static_cast<FunctionDeclarationNode*>(func.get());
        if (funcDecl->name == "main") {
            main_found = true;
            std::string previousFunctionName = currentFunctionName;
            currentFunctionName = "main";

            ss << translator.generateFunctionEntry("main", {}); // Registra la entrada a main

            if (funcDecl->body) {
                ss << visit(funcDecl->body.get()); // Visita el cuerpo de main
            }

            ss << translator.generateFunctionExit("main", ""); // Registra la salida de main

            currentFunctionName = previousFunctionName;
            break;
        }
    }

    if (!main_found) {
        errorHandler.reportWarning("No se encontró la función 'main()' en el código C. Ejecutando sentencias globales si las hay.", -1, -1);
        ss << translator.generateFunctionEntry("global_scope", {});
        for (const auto& stmt : node->statements) {
            ss << visit(stmt.get());
        }
        ss << translator.generateFunctionExit("global_scope", "");
    }

    ss << translator.generateProgramEnd(); // Llama a recordStep("Program Ended")
    translator.decreaseIndent(); // Cierra la indentación de run_c_program_simulation
    ss << "}" << std::endl; // Cierra la función run_c_program_simulation

    // Generar el pie de página SFML (implementaciones de funciones auxiliares)
    ss << translator.getSFMLFooter();

    // Generar la función main de SFML
    ss << std::endl;
    ss << "int main() {" << std::endl;
    translator.increaseIndent(); // Indentación para el cuerpo de main SFML

    // Configuración inicial de la ventana SFML
    ss << translator.getCurrentIndent() << "sf::RenderWindow window(sf::VideoMode(1000, 500), \"C to SFML Compiler Visualization\");" << std::endl;
    ss << translator.getCurrentIndent() << "setupSFML(window);" << std::endl;
    ss << translator.getCurrentIndent() << "window.setFramerateLimit(60);" << std::endl;

    ss << std::endl;
    ss << translator.getCurrentIndent() << "// --- Primera pasada: Ejecutar la simulación C para registrar todos los pasos ---" << std::endl;
    ss << translator.getCurrentIndent() << "run_c_program_simulation();" << std::endl; // Llama a la lógica del programa C simulado
    ss << translator.getCurrentIndent() << "currentStepIndex = 0; // Comienza en el primer paso registrado" << std::endl;
    ss << std::endl;

    ss << translator.getCurrentIndent() << "// --- Bucle principal de eventos SFML para la navegación ---" << std::endl;
    ss << translator.getCurrentIndent() << "while (window.isOpen()) {" << std::endl;
    translator.increaseIndent(); // Indentación para el bucle while(window.isOpen())

    ss << translator.getCurrentIndent() << "sf::Event event;" << std::endl;
    ss << translator.getCurrentIndent() << "while (window.pollEvent(event)) {" << std::endl;
    translator.increaseIndent(); // Indentación para el bucle while(window.pollEvent(event))

    ss << translator.getCurrentIndent() << "if (event.type == sf::Event::Closed) {" << std::endl;
    translator.increaseIndent();
    ss << translator.getCurrentIndent() << "window.close();" << std::endl;
    translator.decreaseIndent();
    ss << translator.getCurrentIndent() << "}" << std::endl;

    ss << translator.getCurrentIndent() << "if (event.type == sf::Event::MouseButtonPressed) {" << std::endl;
    translator.increaseIndent();
    ss << translator.getCurrentIndent() << "if (event.mouseButton.button == sf::Mouse::Left) {" << std::endl;
    translator.increaseIndent();
    ss << translator.getCurrentIndent() << "sf::Vector2i mousePos = sf::Mouse::getPosition(window);" << std::endl;
    ss << translator.getCurrentIndent() << "const float BUTTON_WIDTH = 100.f;" << std::endl;
    ss << translator.getCurrentIndent() << "const float BUTTON_HEIGHT = 40.f;" << std::endl; // Definir BUTTON_HEIGHT
    ss << translator.getCurrentIndent() << "const float BUTTON_Y = window.getSize().y - BUTTON_HEIGHT - 20;" << std::endl; // Usar BUTTON_HEIGHT

    ss << std::endl;
    ss << translator.getCurrentIndent() << "// Botón Siguiente" << std::endl;
    ss << translator.getCurrentIndent() << "const float NEXT_BUTTON_X = window.getSize().x / 2 + 10;" << std::endl;
    ss << translator.getCurrentIndent() << "sf::FloatRect nextButtonBounds(NEXT_BUTTON_X, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT);" << std::endl; // Usar BUTTON_HEIGHT
    ss << translator.getCurrentIndent() << "if (nextButtonBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {" << std::endl;
    translator.increaseIndent();
    ss << translator.getCurrentIndent() << "if (currentStepIndex < simulationHistory.size() - 1) {" << std::endl;
    translator.increaseIndent();
    ss << translator.getCurrentIndent() << "currentStepIndex++;" << std::endl;
    ss << translator.getCurrentIndent() << "}" << std::endl; // Cierra if (currentStepIndex < simulationHistory.size() - 1)
    translator.decreaseIndent();
    ss << translator.getCurrentIndent() << "}" << std::endl; // Cierra if (nextButtonBounds.contains)

    ss << std::endl;
    ss << translator.getCurrentIndent() << "// Botón Anterior" << std::endl;
    ss << translator.getCurrentIndent() << "const float PREV_BUTTON_X = window.getSize().x / 2 - BUTTON_WIDTH - 10;" << std::endl;
    ss << translator.getCurrentIndent() << "sf::FloatRect prevButtonBounds(PREV_BUTTON_X, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT);" << std::endl; // Usar BUTTON_HEIGHT
    ss << translator.getCurrentIndent() << "if (prevButtonBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {" << std::endl;
    translator.increaseIndent();
    ss << translator.getCurrentIndent() << "if (currentStepIndex > 0) {" << std::endl;
    translator.increaseIndent();
    ss << translator.getCurrentIndent() << "currentStepIndex--;" << std::endl;
    ss << translator.getCurrentIndent() << "}" << std::endl; // Cierra if (currentStepIndex > 0)
    translator.decreaseIndent();
    ss << translator.getCurrentIndent() << "}" << std::endl; // Cierra if (prevButtonBounds.contains)

    translator.decreaseIndent(); // Cierra if (event.mouseButton.button == sf::Mouse::Left)
    ss << translator.getCurrentIndent() << "}" << std::endl;

    translator.decreaseIndent(); // Cierra if (event.type == sf::Event::MouseButtonPressed)
    ss << translator.getCurrentIndent() << "}" << std::endl;

    translator.decreaseIndent(); // Cierra while (window.pollEvent(event))
    ss << translator.getCurrentIndent() << "}" << std::endl;

    // Después de procesar los eventos, actualiza y dibuja el paso actual
    ss << translator.getCurrentIndent() << "if (!simulationHistory.empty()) {" << std::endl;
    translator.increaseIndent();
    ss << translator.getCurrentIndent() << "displaySpecificStep(simulationHistory[currentStepIndex]);" << std::endl;
    translator.decreaseIndent();
    ss << translator.getCurrentIndent() << "}" << std::endl;

    ss << translator.getCurrentIndent() << "sf::sleep(sf::milliseconds(10)); // Pequeño sleep para reducir el uso de CPU" << std::endl;

    translator.decreaseIndent(); // Cierra while (window.isOpen())
    ss << translator.getCurrentIndent() << "}" << std::endl;

    ss << translator.getCurrentIndent() << "return 0;" << std::endl;

    translator.decreaseIndent(); // Cierra la indentación de main SFML
    ss << "}" << std::endl; // Cierra la función main SFML

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
        errorHandler.reportWarning("Cuerpo de función nulo para: " + node->name, -1, -1);
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
        argsForSFML.push_back({"param_type", argExpr}); // Tipo genérico, el valor es lo importante para la visualización
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
            errorHandler.reportError("Tipo de nodo desconocido o no esperado como expresión: " + std::to_string(static_cast<int>(node->type)), -1, -1);
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
