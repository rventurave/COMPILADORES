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

    ss << std::endl;
    ss << "// Objetos y constantes globales de SFML" << std::endl;
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
    ss << "// Estado global para la instantánea de memoria actual (usado durante la grabación)" << std::endl;
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
    ss << "const float MAIN_AREA_Y_OFFSET = 60.f;" << std::endl;
    ss << "const float STACK_AREA_X = 50.f;" << std::endl;
    ss << "const float HEAP_AREA_X = 400.f;" << std::endl;
    ss << "const float MEMORY_AREA_WIDTH = 300.f;" << std::endl;
    ss << "const float MEMORY_AREA_HEIGHT = 400.f;" << std::endl;

    ss << std::endl;
    ss << "const float CELL_HEIGHT = 30.f;" << std::endl;
    ss << "const float CELL_DEFAULT_WIDTH = 80.f;" << std::endl;
    ss << "const float CELL_PADDING = 5.f;" << std::endl;


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
    ss << "int currentStepIndex = 0;" << std::endl; // <--- ¡Añadido: Definición de currentStepIndex!

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
    ss << getCurrentIndent() << "globalWindow->clear(sf::Color::Black);" << std::endl;
    ss << getCurrentIndent() << "displayText(step.description, 10.f, 10.f, step.color, 18);" << std::endl;

    ss << std::endl;
    ss << getCurrentIndent() << "// --- Dibujar Área del Heap ---" << std::endl;
    ss << getCurrentIndent() << "displayText(\"Heap\", HEAP_AREA_X + 10, MAIN_AREA_Y_OFFSET - 25, COLOR_DEFAULT_TEXT, 20);" << std::endl;
    ss << getCurrentIndent() << "drawRectangle(HEAP_AREA_X, MAIN_AREA_Y_OFFSET, MEMORY_AREA_WIDTH, MEMORY_AREA_HEIGHT, sf::Color(150, 150, 150, 100), true, 1.f, sf::Color::White);" << std::endl;

    ss << getCurrentIndent() << "float currentHeapY = MAIN_AREA_Y_OFFSET + CELL_PADDING;" << std::endl;
    ss << getCurrentIndent() << "for (const auto& objPair : step.heapSnapshot) {" << std::endl;
    ss << getCurrentIndent() << "    displayText(objPair.first + \": \" + objPair.second, HEAP_AREA_X + CELL_PADDING, currentHeapY, COLOR_DEFAULT_TEXT);" << std::endl;
    ss << getCurrentIndent() << "    currentHeapY += CELL_HEIGHT + CELL_PADDING;" << std::endl;
    ss << getCurrentIndent() << "}" << std::endl;

    ss << std::endl;
    ss << getCurrentIndent() << "// --- Dibujar Área de la Pila ---" << std::endl;
    ss << getCurrentIndent() << "displayText(\"Stack\", STACK_AREA_X + 10, MAIN_AREA_Y_OFFSET - 25, COLOR_DEFAULT_TEXT, 20);" << std::endl;
    ss << getCurrentIndent() << "drawRectangle(STACK_AREA_X, MAIN_AREA_Y_OFFSET, MEMORY_AREA_WIDTH, MEMORY_AREA_HEIGHT, sf::Color(150, 150, 150, 100), true, 1.f, sf::Color::White);" << std::endl;

    ss << getCurrentIndent() << "float currentStackY = MAIN_AREA_Y_OFFSET + MEMORY_AREA_HEIGHT - CELL_HEIGHT - CELL_PADDING;" << std::endl;

    ss << getCurrentIndent() << "float boxWidth = 0.0f;" << std::endl;
    ss << getCurrentIndent() << "float boxHeight = CELL_HEIGHT;" << std::endl;

    ss << getCurrentIndent() << "// Dibujar los marcos de la pila de abajo hacia arriba (iteración inversa)" << std::endl;
    ss << getCurrentIndent() << "for (int i = static_cast<int>(step.stackSnapshot.size()) - 1; i >= 0; --i) {" << std::endl;
    ss << getCurrentIndent() << "    float currentStackX = STACK_AREA_X + CELL_PADDING;" << std::endl;
    ss << getCurrentIndent() << "    std::string frameLabel = \"main\";" << std::endl; // Predeterminado para el primer marco
    ss << getCurrentIndent() << "    if (i > 0) {" << std::endl;
    ss << getCurrentIndent() << "        frameLabel = \"Func\" + std::to_string(i + 1);" << std::endl; // Nombre genérico para otros marcos de función
    ss << getCurrentIndent() << "    }" << std::endl;

    ss << getCurrentIndent() << "    sf::Color frameLabelColor = sf::Color(100, 200, 100, 200);" << std::endl; // Color para main
    ss << getCurrentIndent() << "    if (i > 0) frameLabelColor = sf::Color(50, 50, 150, 200);" << std::endl; // Color para otras funciones

    ss << getCurrentIndent() << "    sf::Text frameText(frameLabel, globalFont, 16);" << std::endl;
    ss << getCurrentIndent() << "    float frameLabelWidth = std::max(CELL_DEFAULT_WIDTH, frameText.getLocalBounds().width + CELL_PADDING * 2);" << std::endl;

    ss << getCurrentIndent() << "    drawRectangle(currentStackX, currentStackY, frameLabelWidth, CELL_HEIGHT, frameLabelColor, true, 1.f, sf::Color::Black);" << std::endl;
    ss << getCurrentIndent() << "    displayText(frameLabel, currentStackX + CELL_PADDING, currentStackY + CELL_PADDING, sf::Color::Black, 16);" << std::endl;
    ss << getCurrentIndent() << "    currentStackX += frameLabelWidth + CELL_PADDING;" << std::endl;

    ss << getCurrentIndent() << "    for (const auto& varPair : step.stackSnapshot[i]) {" << std::endl;
    ss << getCurrentIndent() << "        std::string varName = varPair.first;" << std::endl;
    ss << getCurrentIndent() << "        std::string varValue = varPair.second;" << std::endl;
    ss << getCurrentIndent() << "        sf::Color varCellColor = sf::Color(200, 200, 200, 200);" << std::endl;
    ss << getCurrentIndent() << "        sf::Color varTextColor = sf::Color::Black;" << std::endl;

    ss << getCurrentIndent() << "        if (varValue.rfind(\"0x\", 0) == 0) {" << std::endl; // Comprobar el prefijo "0x" para punteros
    ss << getCurrentIndent() << "            varCellColor = sf::Color(255, 255, 100, 200);" << std::endl; // Amarillento para punteros
    ss << getCurrentIndent() << "        } else if (varName == \"a\") {" << std::endl; // Ejemplo: resaltar la variable 'a'
    ss << getCurrentIndent() << "            varCellColor = sf::Color(100, 200, 100, 200);" << std::endl; // Verdoso para 'a'
    ss << getCurrentIndent() << "            varTextColor = sf::Color::Black;" << std::endl;
    ss << getCurrentIndent() << "        }" << std::endl;

    ss << getCurrentIndent() << "        sf::Text nameText(varName, globalFont, 16);" << std::endl;
    ss << getCurrentIndent() << "        sf::Text valueText(varValue, globalFont, 16);" << std::endl;
    ss << getCurrentIndent() << "        float requiredWidth = nameText.getLocalBounds().width + valueText.getLocalBounds().width + (CELL_PADDING * 3);" << std::endl;
    ss << getCurrentIndent() << "        boxWidth = std::max(CELL_DEFAULT_WIDTH, requiredWidth);" << std::endl;

    ss << getCurrentIndent() << "        // Asegurarse de que la caja encaje dentro del área de memoria, simple verificación de desbordamiento" << std::endl;
    ss << getCurrentIndent() << "        if (currentStackX + boxWidth > STACK_AREA_X + MEMORY_AREA_WIDTH - CELL_PADDING) {" << std::endl;
    ss << getCurrentIndent() << "            // Si se desborda, simplemente usa el ancho restante para evitar salirse visualmente de los límites." << std::endl;
    ss << getCurrentIndent() << "            // Para una solución más robusta, considera envolver a la siguiente línea o hacer los marcos de la pila más altos." << std::endl;
    ss << getCurrentIndent() << "            boxWidth = (STACK_AREA_X + MEMORY_AREA_WIDTH - CELL_PADDING) - currentStackX;" << std::endl;
    ss << getCurrentIndent() << "            if (boxWidth < CELL_DEFAULT_WIDTH / 2) boxWidth = CELL_DEFAULT_WIDTH / 2;" << std::endl; // Evitar cajas diminutas
    ss << getCurrentIndent() << "        }" << std::endl;

    ss << getCurrentIndent() << "        drawRectangle(currentStackX, currentStackY, boxWidth, CELL_HEIGHT, varCellColor, true, 1.f, sf::Color::Black);" << std::endl;
    ss << getCurrentIndent() << "        displayText(varName, currentStackX + CELL_PADDING, currentStackY + CELL_PADDING, varTextColor, 16);" << std::endl;
    ss << getCurrentIndent() << "        displayText(varValue, currentStackX + nameText.getLocalBounds().width + (CELL_PADDING * 2), currentStackY + CELL_PADDING, varTextColor, 16);" << std::endl;
    ss << getCurrentIndent() << "        currentStackX += boxWidth + CELL_PADDING;" << std::endl;
    ss << getCurrentIndent() << "    }" << std::endl;
    ss << getCurrentIndent() << "    currentStackY -= (CELL_HEIGHT + CELL_PADDING); // Mover hacia arriba para el siguiente marco de la pila" << std::endl;
    ss << getCurrentIndent() << "}" << std::endl;

    ss << std::endl;
    ss << getCurrentIndent() << "// --- Dibujar botones \"Previous\" y \"Next\" ---" << std::endl;
    ss << getCurrentIndent() << "const float BUTTON_WIDTH = 100.f;" << std::endl;
    ss << getCurrentIndent() << "const float BUTTON_HEIGHT = 40.f;" << std::endl;
    ss << getCurrentIndent() << "const float BUTTON_Y = globalWindow->getSize().y - BUTTON_HEIGHT - 20;" << std::endl;

    ss << std::endl;
    ss << getCurrentIndent() << "// Botón Anterior" << std::endl;
    ss << getCurrentIndent() << "const float PREV_BUTTON_X = globalWindow->getSize().x / 2 - BUTTON_WIDTH - 10;" << std::endl;
    ss << getCurrentIndent() << "sf::RectangleShape prevButton(sf::Vector2f(BUTTON_WIDTH, BUTTON_HEIGHT));" << std::endl;
    ss << getCurrentIndent() << "prevButton.setPosition(PREV_BUTTON_X, BUTTON_Y);" << std::endl;
    ss << getCurrentIndent() << "prevButton.setFillColor(currentStepIndex > 0 ? sf::Color(70, 70, 70) : sf::Color(30, 30, 30));" << std::endl;
    ss << getCurrentIndent() << "prevButton.setOutlineThickness(2);" << std::endl;
    ss << getCurrentIndent() << "prevButton.setOutlineColor(sf::Color::White);" << std::endl;
    ss << getCurrentIndent() << "globalWindow->draw(prevButton);" << std::endl;
    ss << getCurrentIndent() << "displayText(\"Previous\", PREV_BUTTON_X + (BUTTON_WIDTH - sf::Text(\"Previous\", globalFont, 18).getLocalBounds().width) / 2, BUTTON_Y + (BUTTON_HEIGHT - 18) / 2, sf::Color::White);" << std::endl;

    ss << std::endl;
    ss << getCurrentIndent() << "// Botón Siguiente" << std::endl;
    ss << getCurrentIndent() << "const float NEXT_BUTTON_X = globalWindow->getSize().x / 2 + 10;" << std::endl;
    ss << getCurrentIndent() << "sf::RectangleShape nextButton(sf::Vector2f(BUTTON_WIDTH, BUTTON_HEIGHT));" << std::endl;
    ss << getCurrentIndent() << "nextButton.setPosition(NEXT_BUTTON_X, BUTTON_Y);" << std::endl;
    ss << getCurrentIndent() << "nextButton.setFillColor(currentStepIndex < simulationHistory.size() - 1 ? sf::Color(70, 70, 70) : sf::Color(30, 30, 30));" << std::endl;
    ss << getCurrentIndent() << "nextButton.setOutlineThickness(2);" << std::endl;
    ss << getCurrentIndent() << "nextButton.setOutlineColor(sf::Color::White);" << std::endl;
    ss << getCurrentIndent() << "globalWindow->draw(nextButton);" << std::endl;
    ss << getCurrentIndent() << "displayText(\"Next\", NEXT_BUTTON_X + (BUTTON_WIDTH - sf::Text(\"Next\", globalFont, 18).getLocalBounds().width) / 2, BUTTON_Y + (BUTTON_HEIGHT - 18) / 2, sf::Color::White);" << std::endl;

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
    ss << getCurrentIndent() << "recordStep(\"Program Started\", COLOR_DEFAULT_TEXT);" << std::endl;
    return ss.str();
}

std::string SFMLTranslator::generateProgramEnd() {
    std::stringstream ss;
    ss << getCurrentIndent() << "// Fin del programa" << std::endl;
    ss << getCurrentIndent() << "recordStep(\"Program Ended\", COLOR_DEFAULT_TEXT);" << std::endl;
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
    ss << getCurrentIndent() << "recordStep(\"Declaring: " << typeName << " " << variableName << (initialValue.empty() ? "" : " = " + initialValue) << "\", COLOR_VARIABLE_DECL);" << std::endl;
    ss << getCurrentIndent() << typeName << " " << variableName;
    if (!initialValue.empty()) {
        ss << " = " << initialValue;
    }
    ss << ";" << std::endl;
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