// src/code_generator/SFMLTranslator.cpp
#include "SFMLTranslator.h"
#include <iostream>
#include <utility> // Para std::move en algunos lugares si fuera necesario

SFMLTranslator::SFMLTranslator() : indentLevel(0) {
    // Constructor
}

void SFMLTranslator::increaseIndent() {
    indentLevel++;
}

void SFMLTranslator::decreaseIndent() {
    if (indentLevel > 0) {
        indentLevel--;
    }
}

std::string SFMLTranslator::getCurrentIndent() const {
    return std::string(indentLevel * 4, ' '); // 4 espacios por nivel de indentación
}

// --- Métodos de generación de código SFML ---

// Este método genera los includes, las variables globales y los prototipos de funciones.
std::string SFMLTranslator::getSFMLHeader() {
    std::stringstream ss;
    ss << "#include <SFML/Graphics.hpp>" << std::endl;
    ss << "#include <SFML/Window.hpp>" << std::endl;
    ss << "#include <iostream>" << std::endl;
    ss << "#include <string>" << std::endl;
    ss << "#include <thread>" << std::endl; // Para sf::sleep
    ss << "#include <chrono>" << std::endl; // Para std::chrono::milliseconds
    ss << "#include <vector>" << std::endl; // Para stack de frames
    ss << "#include <map>" << std::endl;    // Para variables dentro de frames

    ss << std::endl;
    ss << "// Global SFML objects and constants" << std::endl;
    ss << "sf::RenderWindow* globalWindow = nullptr;" << std::endl;
    ss << "sf::Font globalFont;" << std::endl;
    ss << "const float LINE_HEIGHT = 25.f;" << std::endl;
    ss << "const sf::Color COLOR_DEFAULT_TEXT = sf::Color::White;" << std::endl;
    ss << "const sf::Color COLOR_HIGHLIGHT = sf::Color::Yellow;" << std::endl;
    ss << "const sf::Color COLOR_VARIABLE_DECL = sf::Color::Cyan;" << std::endl;
    ss << "const sf::Color COLOR_ASSIGNMENT = sf::Color::Magenta;" << std::endl;
    ss << "const sf::Color COLOR_FUNCTION_CALL = sf::Color::Green;" << std::endl;
    ss << "const sf::Color COLOR_RETURN = sf::Color::Red;" << std::endl;
    ss << "const sf::Color COLOR_PRINT = sf::Color::Blue;" << std::endl;

    ss << std::endl;
    ss << "// Global state for Stack Visualization" << std::endl;
    ss << "std::vector<std::map<std::string, std::string>> stackFrames;" << std::endl;
    ss << "std::map<std::string, std::string> heapObjects;" << std::endl;

    ss << "const float STACK_RECT_X = 50.f;" << std::endl;
    ss << "const float STACK_RECT_Y = 80.f;" << std::endl;
    ss << "const float STACK_RECT_WIDTH = 300.f;" << std::endl;
    ss << "const float STACK_RECT_HEIGHT = 400.f;" << std::endl;

    ss << "const float HEAP_RECT_X = 400.f;" << std::endl;
    ss << "const float HEAP_RECT_Y = 80.f;" << std::endl;
    ss << "const float HEAP_RECT_WIDTH = 300.f;" << std::endl;
    ss << "const float HEAP_RECT_HEIGHT = 400.f;" << std::endl;


    ss << std::endl;
    ss << "// Helper functions prototypes (defined in getSFMLFooter)" << std::endl;
    ss << "void setupSFML(sf::RenderWindow& window);" << std::endl;
    ss << "void displayText(const std::string& text_str, float x, float y, sf::Color color);" << std::endl;
    ss << "void drawRectangle(float x, float y, float width, float height, sf::Color color, bool filled, float outlineThickness, sf::Color outlineColor);" << std::endl;
    ss << "void visualizeStep(const std::string& description, sf::Color color);" << std::endl;

    // Prototypes for functions that CodeGenerator generates code for
    ss << "std::string generateProgramStart();" << std::endl; // Not a prototype, this is a method of SFMLTranslator.
    // The following are methods of SFMLTranslator, so their declarations are in SFMLTranslator.h
    // and their implementations are below.
    return ss.str();
}

// Este método solo genera las implementaciones de las funciones auxiliares.
// NO CONTIENE la función main() de SFML.
std::string SFMLTranslator::getSFMLFooter() {
    std::stringstream ss;
    ss << std::endl;
    ss << "// Helper functions implementations" << std::endl;

    ss << "void setupSFML(sf::RenderWindow& window) {" << std::endl;
    ss << getCurrentIndent() << "globalWindow = &window;" << std::endl;
    ss << getCurrentIndent() << "if (!globalFont.loadFromFile(\"../../resources/arial.ttf\")) {" << std::endl;
    ss << getCurrentIndent() << "    std::cerr << \"Error loading font: ../../resources/arial.ttf\" << std::endl;" << std::endl;
    ss << getCurrentIndent() << "}" << std::endl;
    ss << "}" << std::endl;
    ss << std::endl;

    ss << "void displayText(const std::string& text_str, float x, float y, sf::Color color) {" << std::endl;
    ss << getCurrentIndent() << "if (!globalWindow) return;" << std::endl;
    ss << getCurrentIndent() << "sf::Text text(text_str, globalFont, 18);" << std::endl;
    ss << getCurrentIndent() << "text.setPosition(x, y);" << std::endl;
    ss << getCurrentIndent() << "text.setFillColor(color);" << std::endl;
    ss << getCurrentIndent() << "globalWindow->draw(text);" << std::endl;
    ss << "}" << std::endl;
    ss << std::endl;

    ss << "void drawRectangle(float x, float y, float width, float height, sf::Color color, bool filled, float outlineThickness, sf::Color outlineColor) {" << std::endl;
    ss << getCurrentIndent() << "if (!globalWindow) return;" << std::endl;
    ss << getCurrentIndent() << "sf::RectangleShape rectangle(sf::Vector2f(width, height));" << std::endl;
    ss << getCurrentIndent() << "rectangle.setPosition(x, y);" << std::endl;
    ss << getCurrentIndent() << "if (filled) {" << std::endl;
    ss << getCurrentIndent() << "    rectangle.setFillColor(color);" << std::endl;
    ss << getCurrentIndent() << "} else {" << std::endl;
    ss << getCurrentIndent() << "    rectangle.setFillColor(sf::Color::Transparent);" << std::endl;
    ss << getCurrentIndent() << "    rectangle.setOutlineThickness(outlineThickness);" << std::endl;
    ss << getCurrentIndent() << "    rectangle.setOutlineColor(outlineColor);" << std::endl;
    ss << getCurrentIndent() << "}" << std::endl;
    ss << getCurrentIndent() << "globalWindow->draw(rectangle);" << std::endl;
    ss << "}" << std::endl;
    ss << std::endl;

    ss << "void visualizeStep(const std::string& description, sf::Color color) {" << std::endl;
    ss << getCurrentIndent() << "if (!globalWindow || !globalWindow->isOpen()) return;" << std::endl;
    ss << getCurrentIndent() << "globalWindow->clear(sf::Color::Black);" << std::endl;
    ss << getCurrentIndent() << "displayText(description, 10.f, 10.f, color);" << std::endl;

    ss << getCurrentIndent() << "// Draw Stack Rectangle" << std::endl;
    ss << getCurrentIndent() << "drawRectangle(STACK_RECT_X, STACK_RECT_Y, STACK_RECT_WIDTH, STACK_RECT_HEIGHT, sf::Color(50, 50, 150, 100), true, 0.f, sf::Color::Black);" << std::endl;
    ss << getCurrentIndent() << "displayText(\"Stack:\", STACK_RECT_X + 10, STACK_RECT_Y + 5, sf::Color::White);" << std::endl;

    ss << getCurrentIndent() << "// Draw Stack Variables" << std::endl;
    ss << getCurrentIndent() << "float currentVariableY = STACK_RECT_Y + LINE_HEIGHT + 10;" << std::endl;
    ss << getCurrentIndent() << "for (size_t i = 0; i < stackFrames.size(); ++i) {" << std::endl;
    ss << getCurrentIndent() << "    displayText(\"Scope \" + std::to_string(i + 1) + \":\", STACK_RECT_X + 20, currentVariableY, sf::Color::Yellow);" << std::endl;
    ss << getCurrentIndent() << "    currentVariableY += LINE_HEIGHT;" << std::endl;
    ss << getCurrentIndent() << "    for (const auto& varPair : stackFrames[i]) {" << std::endl;
    ss << getCurrentIndent() << "        displayText(varPair.first + \" = \" + varPair.second, STACK_RECT_X + 40, currentVariableY, COLOR_DEFAULT_TEXT);" << std::endl;
    ss << getCurrentIndent() << "        currentVariableY += LINE_HEIGHT;" << std::endl;
    ss << getCurrentIndent() << "    }" << std::endl;
    ss << getCurrentIndent() << "}" << std::endl;

    ss << getCurrentIndent() << "// Draw Heap Rectangle (Placeholder)" << std::endl;
    ss << getCurrentIndent() << "drawRectangle(HEAP_RECT_X, HEAP_RECT_Y, HEAP_RECT_WIDTH, HEAP_RECT_HEIGHT, sf::Color(150, 50, 50, 100), true, 0.f, sf::Color::Black);" << std::endl;
    ss << getCurrentIndent() << "displayText(\"Heap:\", HEAP_RECT_X + 10, HEAP_RECT_Y + 5, sf::Color::White);" << std::endl;
    ss << getCurrentIndent() << "float currentHeapY = HEAP_RECT_Y + LINE_HEIGHT + 10;" << std::endl;
    ss << getCurrentIndent() << "for (const auto& objPair : heapObjects) {" << std::endl;
    ss << getCurrentIndent() << "    displayText(objPair.first + \": \" + objPair.second, HEAP_RECT_X + 20, currentHeapY, COLOR_DEFAULT_TEXT);" << std::endl;
    ss << getCurrentIndent() << "    currentHeapY += LINE_HEIGHT;" << std::endl;
    ss << getCurrentIndent() << "}" << std::endl;


    ss << getCurrentIndent() << "globalWindow->display();" << std::endl;
    ss << getCurrentIndent() << "sf::Event event;" << std::endl;
    ss << getCurrentIndent() << "while (globalWindow->pollEvent(event)) {" << std::endl;
    ss << getCurrentIndent() << "    if (event.type == sf::Event::Closed) {" << std::endl;
    ss << getCurrentIndent() << "        globalWindow->close();" << std::endl;
    ss << getCurrentIndent() << "    }" << std::endl;
    ss << getCurrentIndent() << "}" << std::endl;
    ss << getCurrentIndent() << "sf::sleep(sf::milliseconds(500));" << std::endl;
    ss << "}" << std::endl;

    return ss.str();
}

// A continuación, las implementaciones de las funciones declaradas en SFMLTranslator.h
// Estas deben coincidir exactamente con sus prototipos.

std::string SFMLTranslator::generateProgramStart() {
    std::stringstream ss;
    ss << getCurrentIndent() << "// Program Start" << std::endl;
    ss << getCurrentIndent() << "visualizeStep(\"Program Started\", COLOR_DEFAULT_TEXT);" << std::endl;
    return ss.str();
}

std::string SFMLTranslator::generateProgramEnd() {
    std::stringstream ss;
    ss << getCurrentIndent() << "// Program End" << std::endl;
    ss << getCurrentIndent() << "visualizeStep(\"Program Ended\", COLOR_DEFAULT_TEXT);" << std::endl;
    return ss.str();
}

std::string SFMLTranslator::generateFunctionDeclaration(const std::string& returnType, const std::string& functionName, const std::string& paramsCode, const std::string& bodyCode) {
    std::stringstream ss;
    ss << getCurrentIndent() << returnType << " " << functionName << "(" << paramsCode << ") {" << std::endl;
    increaseIndent();
    ss << bodyCode;
    decreaseIndent();
    ss << getCurrentIndent() << "}" << std::endl;
    return ss.str();
}

std::string SFMLTranslator::generateFunctionCall(const std::string& functionName, const std::string& argsCode) {
    std::stringstream ss;
    ss << getCurrentIndent() << "visualizeStep(\"Calling function: " << functionName << "(" << argsCode << ")\", COLOR_FUNCTION_CALL);" << std::endl;
    ss << getCurrentIndent() << functionName << "(" << argsCode << ");" << std::endl;
    return ss.str();
}

std::string SFMLTranslator::generateVariableDeclaration(const std::string& typeName, const std::string& variableName, const std::string& initialValue) {
    std::stringstream ss;
    ss << getCurrentIndent() << "visualizeStep(\"Declaring: " << typeName << " " << variableName << (initialValue.empty() ? "" : " = " + initialValue) << "\", COLOR_VARIABLE_DECL);" << std::endl;
    ss << getCurrentIndent() << typeName << " " << variableName;
    if (!initialValue.empty()) {
        ss << " = " << initialValue;
    }
    ss << ";" << std::endl;
    return ss.str();
}

std::string SFMLTranslator::generateAssignment(const std::string& identifierName, const std::string& expressionCode) {
    std::stringstream ss;
    ss << getCurrentIndent() << "visualizeStep(\"Assigning to " << identifierName << " = " << expressionCode << "\", COLOR_ASSIGNMENT);" << std::endl;
    ss << getCurrentIndent() << identifierName << " = " << expressionCode << ";" << std::endl;
    return ss.str();
}

std::string SFMLTranslator::generateReturnStatement(const std::string& expressionCode, const std::string& functionName) {
    std::stringstream ss;
    std::string returnMsg = "Returning from " + functionName + (expressionCode.empty() ? "" : " with value: " + expressionCode);
    ss << getCurrentIndent() << "visualizeStep(\"" << returnMsg << "\", COLOR_RETURN);" << std::endl;
    ss << getCurrentIndent() << "return " << expressionCode << ";" << std::endl;
    return ss.str();
}

std::string SFMLTranslator::generateIfStatement(const std::string& conditionCode, const std::string& thenBlockCode, const std::string& elseBlockCode) {
    std::stringstream ss;
    ss << getCurrentIndent() << "visualizeStep(\"Evaluating if (" << conditionCode << ")\", COLOR_HIGHLIGHT);" << std::endl;
    ss << getCurrentIndent() << "if (" << conditionCode << ") {" << std::endl;
    increaseIndent();
    ss << thenBlockCode;
    decreaseIndent();
    ss << getCurrentIndent() << "}" << std::endl;
    if (!elseBlockCode.empty()) {
        ss << getCurrentIndent() << "else {" << std::endl;
        increaseIndent();
        ss << elseBlockCode;
        decreaseIndent();
        ss << getCurrentIndent() << "}" << std::endl;
    }
    return ss.str();
}

std::string SFMLTranslator::generateForLoop(const std::string& initCode, const std::string& conditionCode, const std::string& updateCode, const std::string& bodyCode) {
    std::stringstream ss;
    ss << getCurrentIndent() << "visualizeStep(\"Entering for loop: for (" << initCode << "; " << conditionCode << "; " << updateCode << ")\", COLOR_HIGHLIGHT);" << std::endl;
    ss << getCurrentIndent() << "for (" << initCode << "; " << conditionCode << "; " << updateCode << ") {" << std::endl;
    increaseIndent();
    ss << bodyCode;
    decreaseIndent();
    ss << getCurrentIndent() << "}" << std::endl;
    return ss.str();
}

std::string SFMLTranslator::generatePrintStatement(const std::string& expressionCode) {
    std::stringstream ss;
    ss << getCurrentIndent() << "visualizeStep(\"Printing: " << expressionCode << "\", COLOR_PRINT);" << std::endl;
    ss << getCurrentIndent() << "std::cout << " << expressionCode << " << std::endl;" << std::endl;
    return ss.str();
}

std::string SFMLTranslator::generateBreakStatement() {
    std::stringstream ss;
    ss << getCurrentIndent() << "visualizeStep(\"Break statement encountered\", COLOR_HIGHLIGHT);" << std::endl;
    ss << getCurrentIndent() << "break;" << std::endl;
    return ss.str();
}

std::string SFMLTranslator::generateContinueStatement() {
    std::stringstream ss;
    ss << getCurrentIndent() << "visualizeStep(\"Continue statement encountered\", COLOR_HIGHLIGHT);" << std::endl;
    ss << getCurrentIndent() << "continue;" << std::endl;
    return ss.str();
}

std::string SFMLTranslator::generateFunctionEntry(const std::string& functionName, const std::vector<std::pair<std::string, std::string>>& params) {
    std::stringstream ss;
    ss << getCurrentIndent() << "visualizeStep(\"Entering function: " << functionName << "\", COLOR_FUNCTION_CALL);" << std::endl;
    ss << getCurrentIndent() << "// Enter new stack frame for " << functionName << std::endl;
    ss << getCurrentIndent() << "stackFrames.emplace_back();" << std::endl;
    for (const auto& param : params) {
        ss << getCurrentIndent() << "stackFrames.back()[\"" << param.second << "\"] = std::to_string(" << param.second << ");" << std::endl;
    }
    return ss.str();
}

std::string SFMLTranslator::generateFunctionExit(const std::string& functionName, const std::string& returnValueCode) {
    std::stringstream ss;
    ss << getCurrentIndent() << "visualizeStep(\"Exiting function: " << functionName << (returnValueCode.empty() ? "" : " (Retorna: " + returnValueCode + ")") << "\", COLOR_RETURN);" << std::endl;
    ss << getCurrentIndent() << "// Exit stack frame for " << functionName << std::endl;
    ss << getCurrentIndent() << "if (!stackFrames.empty()) {" << std::endl;
    increaseIndent();
    ss << getCurrentIndent() << "stackFrames.pop_back();" << std::endl;
    decreaseIndent();
    ss << getCurrentIndent() << "}" << std::endl;
    return ss.str();
}

std::string SFMLTranslator::generateVariableUpdate(const std::string& variableName, const std::string& valueCode) {
    std::stringstream ss;
    ss << getCurrentIndent() << "if (!stackFrames.empty()) {" << std::endl;
    increaseIndent();
    ss << getCurrentIndent() << "stackFrames.back()[\"" << variableName << "\"] = std::to_string(" << valueCode << ");" << std::endl;
    decreaseIndent();
    ss << getCurrentIndent() << "}" << std::endl;
    return ss.str();
}

std::string SFMLTranslator::generateScopeEnter() {
    std::stringstream ss;
    ss << getCurrentIndent() << "// Enter new generic scope" << std::endl;
    ss << getCurrentIndent() << "stackFrames.emplace_back();" << std::endl;
    return ss.str();
}

std::string SFMLTranslator::generateScopeExit() {
    std::stringstream ss;
    ss << getCurrentIndent() << "// Exit generic scope" << std::endl;
    ss << getCurrentIndent() << "if (!stackFrames.empty()) {" << std::endl;
    increaseIndent();
    ss << getCurrentIndent() << "stackFrames.pop_back();" << std::endl;
    decreaseIndent();
    ss << getCurrentIndent() << "}" << std::endl;
    return ss.str();
}