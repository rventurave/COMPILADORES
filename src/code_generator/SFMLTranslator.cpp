// src/code_generator/SFMLTranslator.cpp
#include "SFMLTranslator.h"
#include <iostream>
#include <utility> // Para std::move en algunos lugares si fuera necesario
#include <algorithm> // Para std::max

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
    ss << "#include <thread>" << std::endl;
    ss << "#include <chrono>" << std::endl;
    ss << "#include <vector>" << std::endl;
    ss << "#include <map>" << std::endl;
    ss << "#include <algorithm>" << std::endl; // Para std::max
    ss << "#include <sstream>" << std::endl;

    ss << std::endl;
    ss << "// Objetos y constantes globales de SFML" << std::endl;
    ss << "sf::RenderWindow* globalWindow = nullptr;" << std::endl;
    ss << "sf::Font globalFont;" << std::endl;
    ss << "const float LINE_HEIGHT = 25.f;" << std::endl;
    ss << "const sf::Color COLOR_DEFAULT_TEXT = sf::Color::Black;" << std::endl; // CORREGIDO: sf::Color::Black
    ss << "const sf::Color COLOR_HIGHLIGHT = sf::Color::Yellow;" << std::endl;
    ss << "const sf::Color COLOR_VARIABLE_DECL = sf::Color::Cyan;" << std::endl;
    ss << "const sf::Color COLOR_ASSIGNMENT = sf::Color::Magenta;" << std::endl;
    ss << "const sf::Color COLOR_FUNCTION_CALL = sf::Color::Green;" << std::endl;
    ss << "const sf::Color COLOR_RETURN = sf::Color::Red;" << std::endl;
    ss << "const sf::Color COLOR_PRINT = sf::Color::Blue;" << std::endl;

    ss << std::endl;
    ss << "// Global state for current memory snapshot (used during recording)" << std::endl;
    ss << "std::vector<std::map<std::string, std::string>> currentStackFrames;" << std::endl;
    ss << "std::map<std::string, std::string> currentHeapObjects;" << std::endl;

    ss << std::endl;
    ss << "// --- Variables globales del historial de simulación ---" << std::endl;
    ss << "struct SimulationStep {" << std::endl;
    ss << getCurrentIndent() << "std::string description;" << std::endl;
    ss << getCurrentIndent() << "sf::Color color;" << std::endl;
    ss << getCurrentIndent() << "std::vector<std::map<std::string, std::string>> stackSnapshot;" << std::endl;
    ss << getCurrentIndent() << "std::map<std::string, std::string> heapSnapshot;" << std::endl;
    ss << "};" << std::endl;
    ss << "std::vector<SimulationStep> simulationHistory;" << std::endl;
    ss << "extern int currentStepIndex; // Declarado como externo para que CodeGenerator pueda usarlo" << std::endl;

    ss << std::endl;
    ss << "// Posiciones y tamaños ajustados para el diseño basado en la imagen" << std::endl;
    ss << "const float PADDING = 20.f;" << std::endl;
    ss << "const float BAR_HEIGHT = 100.f;" << std::endl; // Altura de las barras de Heap y Stack (ajustado)
    ss << "const float HEAP_BAR_Y = 50.f;" << std::endl; // Posición Y del Heap (ajustado para dejar espacio arriba)
    ss << "const float STACK_BAR_Y = HEAP_BAR_Y + BAR_HEIGHT + PADDING;" << std::endl;
    ss << "const float MEMORY_BAR_WIDTH = 1000.f - (2 * PADDING);" << std::endl; // Ancho total de la ventana - padding

    ss << std::endl;
    ss << "const float BOX_HEIGHT = 50.f;" << std::endl; // Altura de las cajas de variables/punteros
    ss << "const float BOX_PADDING = 10.f;" << std::endl; // Espacio entre cajas

    ss << std::endl;
    ss << "// Prototipos de funciones auxiliares (implementadas en getSFMLFooter)" << std::endl;
    ss << "void setupSFML(sf::RenderWindow& window);" << std::endl;
    ss << "void displayText(const std::string& text_str, float x, float y, sf::Color color, unsigned int characterSize = 18);" << std::endl;
    ss << "void drawRectangle(float x, float y, float width, float height, sf::Color color, bool filled, float outlineThickness, sf::Color outlineColor);" << std::endl;
    ss << "void recordStep(const std::string& description, sf::Color color);" << std::endl;
    ss << "void displaySpecificStep(const SimulationStep& step);" << std::endl;
    ss << "void updateStackFrame(const std::string& varName, const std::string& value);" << std::endl;
    ss << "void pushStackFrame();" << std::endl;
    ss << "void popStackFrame();" << std::endl;
    ss << "void updateHeapObject(const std::string& address, const std::string& value);" << std::endl;
    ss << "void run_c_program_simulation(); // Prototipo de la función que contiene el código C simulado" << std::endl;


    return ss.str();
}

// Este método solo genera las implementaciones de las funciones auxiliares.
// NO CONTIENE la función main() de SFML.
std::string SFMLTranslator::getSFMLFooter() {
    std::stringstream ss;
    ss << std::endl;
    ss << "// Implementaciones de funciones auxiliares" << std::endl;

    // Definición de currentStepIndex
    ss << "int currentStepIndex = 0;" << std::endl;

    ss << "void setupSFML(sf::RenderWindow& window) {" << std::endl;
    ss << getCurrentIndent() << "globalWindow = &window;" << std::endl;
    ss << getCurrentIndent() << "if (!globalFont.loadFromFile(\"../../resources/arial.ttf\")) {" << std::endl;
    ss << getCurrentIndent() << "    std::cerr << \"Error loading font: ../../resources/arial.ttf\" << std::endl;" << std::endl;
    ss << getCurrentIndent() << "}" << std::endl;
    ss << "}" << std::endl;
    ss << std::endl;

    ss << "void displayText(const std::string& text_str, float x, float y, sf::Color color, unsigned int characterSize) {" << std::endl;
    ss << getCurrentIndent() << "if (!globalWindow) return;" << std::endl;
    ss << getCurrentIndent() << "sf::Text text(text_str, globalFont, characterSize);" << std::endl;
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

    ss << "void recordStep(const std::string& description, sf::Color color) {" << std::endl;
    ss << getCurrentIndent() << "SimulationStep step;" << std::endl;
    ss << getCurrentIndent() << "step.description = description;" << std::endl;
    ss << getCurrentIndent() << "step.color = color;" << std::endl;
    ss << getCurrentIndent() << "step.stackSnapshot = currentStackFrames; // Copia profunda" << std::endl;
    ss << getCurrentIndent() << "step.heapSnapshot = currentHeapObjects;   // Copia profunda" << std::endl;
    ss << getCurrentIndent() << "simulationHistory.push_back(step);" << std::endl;
    ss << "}" << std::endl;
    ss << std::endl;


    ss << "void displaySpecificStep(const SimulationStep& step) {" << std::endl;
    ss << getCurrentIndent() << "if (!globalWindow || !globalWindow->isOpen()) return;" << std::endl;
    ss << getCurrentIndent() << "globalWindow->clear(sf::Color(240, 240, 240)); // Fondo gris muy claro para el nuevo diseño" << std::endl; // Fondo gris muy claro

    // Mostrar la descripción del paso actual
    ss << getCurrentIndent() << "displayText(step.description, PADDING, PADDING / 2, sf::Color::Black, 18);" << std::endl;

    ss << std::endl;
    ss << getCurrentIndent() << "// --- Dibujar Área del Heap ---" << std::endl;
    ss << getCurrentIndent() << "displayText(\"Heap\", PADDING, HEAP_BAR_Y - 25, sf::Color::Black, 20);" << std::endl;
    ss << getCurrentIndent() << "drawRectangle(PADDING, HEAP_BAR_Y, MEMORY_BAR_WIDTH, BAR_HEIGHT, sf::Color(210, 210, 210), true, 2.f, sf::Color::Black);" << std::endl; // Fondo gris claro, borde negro

    ss << getCurrentIndent() << "float currentHeapX = PADDING + BOX_PADDING;" << std::endl;
    ss << getCurrentIndent() << "for (const auto& objPair : step.heapSnapshot) {" << std::endl;
    ss << getCurrentIndent() << "    std::string text = objPair.first + \": \" + objPair.second;" << std::endl;
    ss << getCurrentIndent() << "    sf::Text tempText(text, globalFont, 16);" << std::endl;
    ss << getCurrentIndent() << "    float boxWidth = std::max(80.f, tempText.getLocalBounds().width + (BOX_PADDING * 2));" << std::endl; // Ancho mínimo de 80

    ss << getCurrentIndent() << "    // Verificar si la caja se sale del área del heap" << std::endl;
    ss << getCurrentIndent() << "    if (currentHeapX + boxWidth > PADDING + MEMORY_BAR_WIDTH - BOX_PADDING) {" << std::endl;
    ss << getCurrentIndent() << "        break;" << std::endl; // Salir si no hay espacio
    ss << getCurrentIndent() << "    }" << std::endl;

    ss << getCurrentIndent() << "    sf::Color boxColor = sf::Color(255, 255, 150); // Amarillo claro para punteros en heap" << std::endl;
    ss << getCurrentIndent() << "    drawRectangle(currentHeapX, HEAP_BAR_Y + (BAR_HEIGHT - BOX_HEIGHT) / 2, boxWidth, BOX_HEIGHT, boxColor, true, 1.f, sf::Color::Black);" << std::endl;
    ss << getCurrentIndent() << "    displayText(text, currentHeapX + BOX_PADDING, HEAP_BAR_Y + (BAR_HEIGHT - BOX_HEIGHT) / 2 + BOX_PADDING, sf::Color::Black, 16);" << std::endl;
    ss << getCurrentIndent() << "    currentHeapX += boxWidth + BOX_PADDING;" << std::endl;
    ss << getCurrentIndent() << "}" << std::endl;

    ss << std::endl;
    ss << getCurrentIndent() << "// --- Dibujar Área de la Pila ---" << std::endl;
    ss << getCurrentIndent() << "displayText(\"Stack\", PADDING, STACK_BAR_Y - 25, sf::Color::Black, 20);" << std::endl;
    ss << getCurrentIndent() << "drawRectangle(PADDING, STACK_BAR_Y, MEMORY_BAR_WIDTH, BAR_HEIGHT, sf::Color(210, 210, 210), true, 2.f, sf::Color::Black);" << std::endl; // Fondo gris claro, borde negro

    ss << getCurrentIndent() << "float currentStackX = PADDING + BOX_PADDING;" << std::endl;

    ss << getCurrentIndent() << "// Dibujar los marcos de la pila de izquierda a derecha (orden de llamada)" << std::endl;
    ss << getCurrentIndent() << "for (size_t i = 0; i < step.stackSnapshot.size(); ++i) {" << std::endl;
    ss << getCurrentIndent() << "    std::string frameLabel = (i == 0) ? \"main\" : \"Func\" + std::to_string(i + 1);" << std::endl;

    ss << getCurrentIndent() << "    // Dibujar el label del marco (main, Func1, etc.)" << std::endl;
    ss << getCurrentIndent() << "    sf::Color frameLabelColor = sf::Color(150, 255, 150); // Verde claro para 'main' y otros labels" << std::endl;
    ss << getCurrentIndent() << "    sf::Text tempFrameText(frameLabel, globalFont, 16);" << std::endl;
    ss << getCurrentIndent() << "    float frameLabelWidth = std::max(80.f, tempFrameText.getLocalBounds().width + (BOX_PADDING * 2));" << std::endl;

    ss << getCurrentIndent() << "    if (currentStackX + frameLabelWidth > PADDING + MEMORY_BAR_WIDTH - BOX_PADDING) break;" << std::endl; // Salir si no hay espacio

    ss << getCurrentIndent() << "    drawRectangle(currentStackX, STACK_BAR_Y + (BAR_HEIGHT - BOX_HEIGHT) / 2, frameLabelWidth, BOX_HEIGHT, frameLabelColor, true, 1.f, sf::Color::Black);" << std::endl;
    ss << getCurrentIndent() << "    displayText(frameLabel, currentStackX + BOX_PADDING, STACK_BAR_Y + (BAR_HEIGHT - BOX_HEIGHT) / 2 + BOX_PADDING, sf::Color::Black, 16);" << std::endl;
    ss << getCurrentIndent() << "    currentStackX += frameLabelWidth + BOX_PADDING;" << std::endl;


    ss << getCurrentIndent() << "    // Dibujar las variables dentro del marco de la pila" << std::endl;
    ss << getCurrentIndent() << "    for (const auto& varPair : step.stackSnapshot[i]) {" << std::endl;
    ss << getCurrentIndent() << "        std::string varName = varPair.first;" << std::endl;
    ss << getCurrentIndent() << "        std::string varValue = varPair.second;" << std::endl;
    ss << getCurrentIndent() << "        std::string varText = varName;" << std::endl; // Solo el nombre para la caja, el valor se dibuja aparte

    ss << getCurrentIndent() << "        sf::Text tempVarNameText(varName, globalFont, 16);" << std::endl;
    ss << getCurrentIndent() << "        sf::Text tempVarValueText(varValue, globalFont, 16);" << std::endl;
    ss << getCurrentIndent() << "        float nameWidth = tempVarNameText.getLocalBounds().width;" << std::endl;
    ss << getCurrentIndent() << "        float valueWidth = tempVarValueText.getLocalBounds().width;" << std::endl;

    ss << getCurrentIndent() << "        float boxWidth = std::max(80.f, nameWidth + valueWidth + (BOX_PADDING * 3));" << std::endl; // Ancho para nombre + valor

    ss << getCurrentIndent() << "        if (currentStackX + boxWidth > PADDING + MEMORY_BAR_WIDTH - BOX_PADDING) {" << std::endl;
    ss << getCurrentIndent() << "            break;" << std::endl;
    ss << getCurrentIndent() << "        }" << std::endl;

    ss << getCurrentIndent() << "        sf::Color varCellColor;" << std::endl;
    ss << getCurrentIndent() << "        if (varValue.rfind(\"0x\", 0) == 0) {" << std::endl; // Es un puntero
    ss << getCurrentIndent() << "            varCellColor = sf::Color(255, 255, 150); // Amarillo claro para punteros" << std::endl;
    ss << getCurrentIndent() << "        } else {" << std::endl; // Es un valor numérico/normal
    ss << getCurrentIndent() << "            varCellColor = sf::Color(150, 255, 150); // Verde claro para valores" << std::endl;
    ss << getCurrentIndent() << "        }" << std::endl;

    ss << getCurrentIndent() << "        drawRectangle(currentStackX, STACK_BAR_Y + (BAR_HEIGHT - BOX_HEIGHT) / 2, boxWidth, BOX_HEIGHT, varCellColor, true, 1.f, sf::Color::Black);" << std::endl;
    ss << getCurrentIndent() << "        displayText(varName, currentStackX + BOX_PADDING, STACK_BAR_Y + (BAR_HEIGHT - BOX_HEIGHT) / 2 + BOX_PADDING, sf::Color::Black, 16);" << std::endl;
    ss << getCurrentIndent() << "        displayText(varValue, currentStackX + BOX_PADDING + nameWidth + BOX_PADDING, STACK_BAR_Y + (BAR_HEIGHT - BOX_HEIGHT) / 2 + BOX_PADDING, sf::Color::Black, 16);" << std::endl;
    ss << getCurrentIndent() << "        currentStackX += boxWidth + BOX_PADDING;" << std::endl;
    ss << getCurrentIndent() << "    }" << std::endl;
    ss << getCurrentIndent() << "}" << std::endl;


    ss << std::endl;
    ss << getCurrentIndent() << "// --- Dibujar botones \"Previous\" y \"Next\" ---" << std::endl;
    ss << getCurrentIndent() << "const float BUTTON_WIDTH = 100.f;" << std::endl;
    ss << getCurrentIndent() << "const float BUTTON_HEIGHT = 40.f;" << std::endl;
    ss << getCurrentIndent() << "const float BUTTON_Y = globalWindow->getSize().y - BUTTON_HEIGHT - PADDING;" << std::endl; // Posición de los botones

    ss << std::endl;
    ss << getCurrentIndent() << "// Botón Anterior" << std::endl;
    ss << getCurrentIndent() << "const float PREV_BUTTON_X = (globalWindow->getSize().x / 2) - BUTTON_WIDTH - (BOX_PADDING * 2);" << std::endl;
    ss << getCurrentIndent() << "sf::RectangleShape prevButton(sf::Vector2f(BUTTON_WIDTH, BUTTON_HEIGHT));" << std::endl;
    ss << getCurrentIndent() << "prevButton.setPosition(PREV_BUTTON_X, BUTTON_Y);" << std::endl;
    ss << getCurrentIndent() << "prevButton.setFillColor(currentStepIndex > 0 ? sf::Color(70, 70, 70) : sf::Color(30, 30, 30));" << std::endl;
    ss << getCurrentIndent() << "prevButton.setOutlineThickness(2);" << std::endl;
    ss << getCurrentIndent() << "prevButton.setOutlineColor(sf::Color::Black);" << std::endl; // CORREGIDO: sf::Color::Black
    ss << getCurrentIndent() << "globalWindow->draw(prevButton);" << std::endl;
    ss << getCurrentIndent() << "displayText(\"Previous\", PREV_BUTTON_X + (BUTTON_WIDTH - sf::Text(\"Previous\", globalFont, 18).getLocalBounds().width) / 2, BUTTON_Y + (BUTTON_HEIGHT - 18) / 2, sf::Color::White);" << std::endl; // CORREGIDO: sf::Color::White

    ss << std::endl;
    ss << getCurrentIndent() << "// Botón Siguiente" << std::endl;
    ss << getCurrentIndent() << "const float NEXT_BUTTON_X = (globalWindow->getSize().x / 2) + (BOX_PADDING * 2);" << std::endl;
    ss << getCurrentIndent() << "sf::RectangleShape nextButton(sf::Vector2f(BUTTON_WIDTH, BUTTON_HEIGHT));" << std::endl;
    ss << getCurrentIndent() << "nextButton.setPosition(NEXT_BUTTON_X, BUTTON_Y);" << std::endl;
    ss << getCurrentIndent() << "nextButton.setFillColor(currentStepIndex < simulationHistory.size() - 1 ? sf::Color(70, 70, 70) : sf::Color(30, 30, 30));" << std::endl;
    ss << getCurrentIndent() << "nextButton.setOutlineThickness(2);" << std::endl;
    ss << getCurrentIndent() << "nextButton.setOutlineColor(sf::Color::Black);" << std::endl; // CORREGIDO: sf::Color::Black
    ss << getCurrentIndent() << "globalWindow->draw(nextButton);" << std::endl;
    ss << getCurrentIndent() << "displayText(\"Next\", NEXT_BUTTON_X + (BUTTON_WIDTH - sf::Text(\"Next\", globalFont, 18).getLocalBounds().width) / 2, BUTTON_Y + (BUTTON_HEIGHT - 18) / 2, sf::Color::White);" << std::endl; // CORREGIDO: sf::Color::White

    ss << getCurrentIndent() << "globalWindow->display();" << std::endl;
    ss << "}" << std::endl;

    ss << std::endl;
    ss << "void updateStackFrame(const std::string& varName, const std::string& value) {" << std::endl;
    ss << getCurrentIndent() << "if (!currentStackFrames.empty()) {" << std::endl;
    ss << getCurrentIndent() << "    currentStackFrames.back()[varName] = value;" << std::endl;
    ss << getCurrentIndent() << "}" << std::endl;
    ss << "}" << std::endl;
    ss << std::endl;

    ss << "void pushStackFrame() {" << std::endl;
    ss << getCurrentIndent() << "currentStackFrames.emplace_back();" << std::endl;
    ss << "}" << std::endl;
    ss << std::endl;

    ss << "void popStackFrame() {" << std::endl;
    ss << getCurrentIndent() << "if (!currentStackFrames.empty()) {" << std::endl;
    ss << getCurrentIndent() << "    currentStackFrames.pop_back();" << std::endl;
    ss << getCurrentIndent() << "}" << std::endl;
    ss << "}" << std::endl;
    ss << std::endl;

    ss << "void updateHeapObject(const std::string& address, const std::string& value) {" << std::endl;
    ss << getCurrentIndent() << "currentHeapObjects[address] = value;" << std::endl;
    ss << "}" << std::endl;

    return ss.str();
}

// A continuación, las implementaciones de las funciones declaradas en SFMLTranslator.h
// Estas deben coincidir exactamente con sus prototipos.

std::string SFMLTranslator::generateProgramStart() {
    std::stringstream ss;
    ss << getCurrentIndent() << "// Inicio del programa" << std::endl;
    ss << getCurrentIndent() << "recordStep(\"Program Started\", sf::Color::Black);" << std::endl; // CORREGIDO: sf::Color::Black
    return ss.str();
}

std::string SFMLTranslator::generateProgramEnd() {
    std::stringstream ss;
    ss << getCurrentIndent() << "// Fin del programa" << std::endl;
    ss << getCurrentIndent() << "recordStep(\"Program Ended\", sf::Color::Black);" << std::endl; // CORREGIDO: sf::Color::Black
    return ss.str();
}

std::string SFMLTranslator::generateFunctionDeclaration(const std::string& returnType, const std::string& functionName, const std::string& paramsCode, const std::string& bodyCode) {
    // Esta función no se usa directamente para generar la definición de la función C++ en output_sfml.cpp.
    // CodeGenerator::visitFunctionDeclarationNode maneja la definición real de la función.
    // Se mantiene aquí si hubiera un plan futuro para usarla de manera diferente.
    return "";
}

std::string SFMLTranslator::generateFunctionCall(const std::string& functionName, const std::string& argsCode) {
    std::stringstream ss;
    ss << getCurrentIndent() << "recordStep(\"Calling function: " << functionName << "(" << argsCode << ")\", COLOR_FUNCTION_CALL);" << std::endl;
    // La llamada a la función real en la salida C++ es manejada directamente por CodeGenerator.
    return ss.str();
}

std::string SFMLTranslator::generateVariableDeclaration(const std::string& typeName, const std::string& variableName, const std::string& initialValue) {
    std::stringstream ss;
    ss << getCurrentIndent() << typeName << " " << variableName << " = " << initialValue << ";" << std::endl;

    // Si es puntero y el valor inicial es una dirección (&...)
    if (typeName == "int*" && initialValue.find('&') == 0) {
        ss << getCurrentIndent() << "std::ostringstream oss_" << variableName << ";" << std::endl;
        ss << getCurrentIndent() << "oss_" << variableName << " << " << initialValue << ";" << std::endl;
        ss << getCurrentIndent() << "updateStackFrame(\"" << variableName << "\", oss_" << variableName << ".str());" << std::endl;
    } else {
        ss << getCurrentIndent() << "updateStackFrame(\"" << variableName << "\", std::to_string(" << variableName << "));" << std::endl;
    }

    return ss.str();
}

std::string SFMLTranslator::generateAssignment(const std::string& identifierName, const std::string& expressionCode) {
    std::stringstream ss;
    ss << getCurrentIndent() << "recordStep(\"Assigning to " << identifierName << " = \" + std::to_string(" << expressionCode << "), COLOR_ASSIGNMENT);" << std::endl;
    ss << getCurrentIndent() << identifierName << " = " << expressionCode << ";" << std::endl;
    return ss.str();
}

std::string SFMLTranslator::generateReturnStatement(const std::string& expressionCode, const std::string& functionName) {
    std::stringstream ss;
    std::string returnMsg = "Returning from " + functionName + (expressionCode.empty() ? "" : " (Returns: " + expressionCode + ")");
    ss << getCurrentIndent() << "recordStep(\"" << returnMsg << "\", COLOR_RETURN);" << std::endl;
    ss << getCurrentIndent() << "return " << expressionCode << ";" << std::endl;
    return ss.str();
}

std::string SFMLTranslator::generateIfStatement(const std::string& conditionCode, const std::string& thenBlockCode, const std::string& elseBlockCode) {
    std::stringstream ss;
    ss << getCurrentIndent() << "recordStep(\"Evaluating if (\" + std::to_string(" << conditionCode << ") + \")\", COLOR_HIGHLIGHT);" << std::endl;
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
    // Nota: initCode, conditionCode, updateCode aquí son el código C/C++ sin formato, no formateado con llamadas SFML.
    // CodeGenerator se encarga de generar sus correspondientes llamadas de visualización SFML.
    ss << getCurrentIndent() << "recordStep(\"Entering for loop\", COLOR_HIGHLIGHT);" << std::endl;
    ss << getCurrentIndent() << "for (" << initCode << "; " << conditionCode << "; " << updateCode << ") {" << std::endl;
    increaseIndent();
    ss << bodyCode;
    decreaseIndent();
    ss << getCurrentIndent() << "}" << std::endl;
    return ss.str();
}

std::string SFMLTranslator::generatePrintStatement(const std::string& expressionCode) {
    std::stringstream ss;
    ss << getCurrentIndent() << "recordStep(\"Printing: \" + std::to_string(" << expressionCode << "), COLOR_PRINT);" << std::endl;
    ss << getCurrentIndent() << "std::cout << " << expressionCode << " << std::endl;" << std::endl;
    return ss.str();
}

std::string SFMLTranslator::generateBreakStatement() {
    std::stringstream ss;
    ss << getCurrentIndent() << "recordStep(\"Break statement encountered\", COLOR_HIGHLIGHT);" << std::endl;
    ss << getCurrentIndent() << "break;" << std::endl;
    return ss.str();
}

std::string SFMLTranslator::generateContinueStatement() {
    std::stringstream ss;
    ss << getCurrentIndent() << "recordStep(\"Continue statement encountered\", COLOR_HIGHLIGHT);" << std::endl;
    ss << getCurrentIndent() << "continue;" << std::endl;
    return ss.str();
}

std::string SFMLTranslator::generateFunctionEntry(const std::string& functionName, const std::vector<std::pair<std::string, std::string>>& params) {
    std::stringstream ss;
    ss << getCurrentIndent() << "recordStep(\"Entering function: " << functionName << "\", COLOR_FUNCTION_CALL);" << std::endl;
    ss << getCurrentIndent() << "pushStackFrame();" << std::endl;
    for (const auto& param : params) {
        // Asegurarse de que el valor se convierte a string si es necesario
        ss << getCurrentIndent() << "updateStackFrame(\"" << param.second << "\", std::to_string(" << param.second << "));" << std::endl;
    }
    return ss.str();
}

std::string SFMLTranslator::generateFunctionExit(const std::string& functionName, const std::string& returnValueCode) {
    std::stringstream ss;
    ss << getCurrentIndent() << "recordStep(\"Exiting function: " << functionName << (returnValueCode.empty() ? "" : " (Returns: " + returnValueCode + ")") << "\", COLOR_RETURN);" << std::endl;
    ss << getCurrentIndent() << "popStackFrame();" << std::endl;
    return ss.str();
}

std::string SFMLTranslator::generateVariableUpdate(const std::string& variableName, const std::string& valueCode) {
    std::stringstream ss;
    ss << getCurrentIndent() << "updateStackFrame(\"" << variableName << "\", std::to_string(" << valueCode << "));" << std::endl;
    return ss.str();
}

std::string SFMLTranslator::generateScopeEnter() {
    std::stringstream ss;
    ss << getCurrentIndent() << "// Entrar en un nuevo ámbito genérico" << std::endl;
    ss << getCurrentIndent() << "pushStackFrame();" << std::endl;
    return ss.str();
}

std::string SFMLTranslator::generateScopeExit() {
    std::stringstream ss;
    ss << getCurrentIndent() << "// Salir de un ámbito genérico" << std::endl;
    ss << getCurrentIndent() << "popStackFrame();" << std::endl;
    return ss.str();
}