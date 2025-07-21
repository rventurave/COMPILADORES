#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>

// Objetos y constantes globales de SFML
sf::RenderWindow* globalWindow = nullptr;
sf::Font globalFont;
const float LINE_HEIGHT = 25.f;
const sf::Color COLOR_DEFAULT_TEXT = sf::Color::Black;
const sf::Color COLOR_HIGHLIGHT = sf::Color::Yellow;
const sf::Color COLOR_VARIABLE_DECL = sf::Color::Cyan;
const sf::Color COLOR_ASSIGNMENT = sf::Color::Magenta;
const sf::Color COLOR_FUNCTION_CALL = sf::Color::Green;
const sf::Color COLOR_RETURN = sf::Color::Red;
const sf::Color COLOR_PRINT = sf::Color::Blue;

// Global state for current memory snapshot (used during recording)
std::vector<std::map<std::string, std::string>> currentStackFrames;
std::map<std::string, std::string> currentHeapObjects;

// --- Variables globales del historial de simulación ---
struct SimulationStep {
std::string description;
sf::Color color;
std::vector<std::map<std::string, std::string>> stackSnapshot;
std::map<std::string, std::string> heapSnapshot;
};
std::vector<SimulationStep> simulationHistory;
extern int currentStepIndex; // Declarado como externo para que CodeGenerator pueda usarlo

// Posiciones y tamaños ajustados para el diseño basado en la imagen
const float PADDING = 20.f;
const float BAR_HEIGHT = 100.f;
const float HEAP_BAR_Y = 50.f;
const float STACK_BAR_Y = HEAP_BAR_Y + BAR_HEIGHT + PADDING;
const float MEMORY_BAR_WIDTH = 1000.f - (2 * PADDING);

const float BOX_HEIGHT = 50.f;
const float BOX_PADDING = 10.f;

// Prototipos de funciones auxiliares (implementadas en getSFMLFooter)
void setupSFML(sf::RenderWindow& window);
void displayText(const std::string& text_str, float x, float y, sf::Color color, unsigned int characterSize = 18);
void drawRectangle(float x, float y, float width, float height, sf::Color color, bool filled, float outlineThickness, sf::Color outlineColor);
void recordStep(const std::string& description, sf::Color color);
void displaySpecificStep(const SimulationStep& step);
void updateStackFrame(const std::string& varName, const std::string& value);
void pushStackFrame();
void popStackFrame();
void updateHeapObject(const std::string& address, const std::string& value);
void run_c_program_simulation(); // Prototipo de la función que contiene el código C simulado
void run_c_program_simulation() {
    // Inicio del programa
    recordStep("Program Started", sf::Color::Black);
    recordStep("Entering function: main", COLOR_FUNCTION_CALL);
    pushStackFrame();
    {
        recordStep("Declaring: int a = 1", COLOR_VARIABLE_DECL);
        int a = 1;
        updateStackFrame("a", std::to_string(1));
        recordStep("Declaring: int b = 2", COLOR_VARIABLE_DECL);
        int b = 2;
        updateStackFrame("b", std::to_string(2));
        recordStep("Declaring: int c = 3", COLOR_VARIABLE_DECL);
        int c = 3;
        updateStackFrame("c", std::to_string(3));
        recordStep("Declaring: int* p = (&a)", COLOR_VARIABLE_DECL);
        int* p = (&a);
        std::ostringstream oss_p;
        oss_p << p;
        updateStackFrame("p", oss_p.str());
        recordStep("Declaring: int* q = (&b)", COLOR_VARIABLE_DECL);
        int* q = (&b);
        std::ostringstream oss_q;
        oss_q << q;
        updateStackFrame("q", oss_q.str());
        recordStep("Assigning to c = " + std::to_string(100), COLOR_ASSIGNMENT);
        c = 100;
        updateStackFrame("c", std::to_string(100));
    }
    recordStep("Exiting function: main", COLOR_RETURN);
    popStackFrame();
    // Fin del programa
    recordStep("Program Ended", sf::Color::Black);
}

// Implementaciones de funciones auxiliares
int currentStepIndex = 0;
void setupSFML(sf::RenderWindow& window) {
globalWindow = &window;
if (!globalFont.loadFromFile("../../resources/arial.ttf")) {
    std::cerr << "Error loading font: ../../resources/arial.ttf" << std::endl;
}
}

void displayText(const std::string& text_str, float x, float y, sf::Color color, unsigned int characterSize) {
if (!globalWindow) return;
sf::Text text(text_str, globalFont, characterSize);
text.setPosition(x, y);
text.setFillColor(color);
globalWindow->draw(text);
}

void drawRectangle(float x, float y, float width, float height, sf::Color color, bool filled, float outlineThickness, sf::Color outlineColor) {
if (!globalWindow) return;
sf::RectangleShape rectangle(sf::Vector2f(width, height));
rectangle.setPosition(x, y);
if (filled) {
    rectangle.setFillColor(color);
} else {
    rectangle.setFillColor(sf::Color::Transparent);
    rectangle.setOutlineThickness(outlineThickness);
    rectangle.setOutlineColor(outlineColor);
}
globalWindow->draw(rectangle);
}

void recordStep(const std::string& description, sf::Color color) {
SimulationStep step;
step.description = description;
step.color = color;
step.stackSnapshot = currentStackFrames; // Copia profunda
step.heapSnapshot = currentHeapObjects;   // Copia profunda
simulationHistory.push_back(step);
}

void displaySpecificStep(const SimulationStep& step) {
if (!globalWindow || !globalWindow->isOpen()) return;
globalWindow->clear(sf::Color(240, 240, 240)); // Fondo gris muy claro para el nuevo diseño
displayText(step.description, PADDING, PADDING / 2, sf::Color::Black, 18);

// --- Dibujar Área del Heap ---
displayText("Heap", PADDING, HEAP_BAR_Y - 25, sf::Color::Black, 20);
drawRectangle(PADDING, HEAP_BAR_Y, MEMORY_BAR_WIDTH, BAR_HEIGHT, sf::Color(210, 210, 210), true, 2.f, sf::Color::Black);
float currentHeapX = PADDING + BOX_PADDING;
for (const auto& objPair : step.heapSnapshot) {
    std::string text = objPair.first + ": " + objPair.second;
    sf::Text tempText(text, globalFont, 16);
    float boxWidth = std::max(80.f, tempText.getLocalBounds().width + (BOX_PADDING * 2));
    // Verificar si la caja se sale del área del heap
    if (currentHeapX + boxWidth > PADDING + MEMORY_BAR_WIDTH - BOX_PADDING) {
        break;
    }
    sf::Color boxColor = sf::Color(255, 255, 150); // Amarillo claro para punteros en heap
    drawRectangle(currentHeapX, HEAP_BAR_Y + (BAR_HEIGHT - BOX_HEIGHT) / 2, boxWidth, BOX_HEIGHT, boxColor, true, 1.f, sf::Color::Black);
    displayText(text, currentHeapX + BOX_PADDING, HEAP_BAR_Y + (BAR_HEIGHT - BOX_HEIGHT) / 2 + BOX_PADDING, sf::Color::Black, 16);
    currentHeapX += boxWidth + BOX_PADDING;
}

// --- Dibujar Área de la Pila ---
displayText("Stack", PADDING, STACK_BAR_Y - 25, sf::Color::Black, 20);
drawRectangle(PADDING, STACK_BAR_Y, MEMORY_BAR_WIDTH, BAR_HEIGHT, sf::Color(210, 210, 210), true, 2.f, sf::Color::Black);
float currentStackX = PADDING + BOX_PADDING;
// Dibujar los marcos de la pila de izquierda a derecha (orden de llamada)
for (size_t i = 0; i < step.stackSnapshot.size(); ++i) {
    std::string frameLabel = (i == 0) ? "main" : "Func" + std::to_string(i + 1);
    // Dibujar el label del marco (main, Func1, etc.)
    sf::Color frameLabelColor = sf::Color(150, 255, 150); // Verde claro para 'main' y otros labels
    sf::Text tempFrameText(frameLabel, globalFont, 16);
    float frameLabelWidth = std::max(80.f, tempFrameText.getLocalBounds().width + (BOX_PADDING * 2));
    if (currentStackX + frameLabelWidth > PADDING + MEMORY_BAR_WIDTH - BOX_PADDING) break;
    drawRectangle(currentStackX, STACK_BAR_Y + (BAR_HEIGHT - BOX_HEIGHT) / 2, frameLabelWidth, BOX_HEIGHT, frameLabelColor, true, 1.f, sf::Color::Black);
    displayText(frameLabel, currentStackX + BOX_PADDING, STACK_BAR_Y + (BAR_HEIGHT - BOX_HEIGHT) / 2 + BOX_PADDING, sf::Color::Black, 16);
    currentStackX += frameLabelWidth + BOX_PADDING;
    // Dibujar las variables dentro del marco de la pila
    for (const auto& varPair : step.stackSnapshot[i]) {
        std::string varName = varPair.first;
        std::string varValue = varPair.second;
        std::string varText = varName;
        sf::Text tempVarNameText(varName, globalFont, 16);
        sf::Text tempVarValueText(varValue, globalFont, 16);
        float nameWidth = tempVarNameText.getLocalBounds().width;
        float valueWidth = tempVarValueText.getLocalBounds().width;
        float boxWidth = std::max(80.f, nameWidth + valueWidth + (BOX_PADDING * 3));
        if (currentStackX + boxWidth > PADDING + MEMORY_BAR_WIDTH - BOX_PADDING) {
            break;
        }
        sf::Color varCellColor;
        if (varValue.rfind("0x", 0) == 0) {
            varCellColor = sf::Color(255, 255, 150); // Amarillo claro para punteros
        } else {
            varCellColor = sf::Color(150, 255, 150); // Verde claro para valores
        }
        drawRectangle(currentStackX, STACK_BAR_Y + (BAR_HEIGHT - BOX_HEIGHT) / 2, boxWidth, BOX_HEIGHT, varCellColor, true, 1.f, sf::Color::Black);
        displayText(varName, currentStackX + BOX_PADDING, STACK_BAR_Y + (BAR_HEIGHT - BOX_HEIGHT) / 2 + BOX_PADDING, sf::Color::Black, 16);
        displayText(varValue, currentStackX + BOX_PADDING + nameWidth + BOX_PADDING, STACK_BAR_Y + (BAR_HEIGHT - BOX_HEIGHT) / 2 + BOX_PADDING, sf::Color::Black, 16);
        currentStackX += boxWidth + BOX_PADDING;
    }
}

// --- Dibujar botones "Previous" y "Next" ---
const float BUTTON_WIDTH = 100.f;
const float BUTTON_HEIGHT = 40.f;
const float BUTTON_Y = globalWindow->getSize().y - BUTTON_HEIGHT - PADDING;

// Botón Anterior
const float PREV_BUTTON_X = (globalWindow->getSize().x / 2) - BUTTON_WIDTH - (BOX_PADDING * 2);
sf::RectangleShape prevButton(sf::Vector2f(BUTTON_WIDTH, BUTTON_HEIGHT));
prevButton.setPosition(PREV_BUTTON_X, BUTTON_Y);
prevButton.setFillColor(currentStepIndex > 0 ? sf::Color(70, 70, 70) : sf::Color(30, 30, 30));
prevButton.setOutlineThickness(2);
prevButton.setOutlineColor(sf::Color::Black);
globalWindow->draw(prevButton);
displayText("Previous", PREV_BUTTON_X + (BUTTON_WIDTH - sf::Text("Previous", globalFont, 18).getLocalBounds().width) / 2, BUTTON_Y + (BUTTON_HEIGHT - 18) / 2, sf::Color::White);

// Botón Siguiente
const float NEXT_BUTTON_X = (globalWindow->getSize().x / 2) + (BOX_PADDING * 2);
sf::RectangleShape nextButton(sf::Vector2f(BUTTON_WIDTH, BUTTON_HEIGHT));
nextButton.setPosition(NEXT_BUTTON_X, BUTTON_Y);
nextButton.setFillColor(currentStepIndex < simulationHistory.size() - 1 ? sf::Color(70, 70, 70) : sf::Color(30, 30, 30));
nextButton.setOutlineThickness(2);
nextButton.setOutlineColor(sf::Color::Black);
globalWindow->draw(nextButton);
displayText("Next", NEXT_BUTTON_X + (BUTTON_WIDTH - sf::Text("Next", globalFont, 18).getLocalBounds().width) / 2, BUTTON_Y + (BUTTON_HEIGHT - 18) / 2, sf::Color::White);
globalWindow->display();
}

void updateStackFrame(const std::string& varName, const std::string& value) {
if (!currentStackFrames.empty()) {
    currentStackFrames.back()[varName] = value;
}
}

void pushStackFrame() {
currentStackFrames.emplace_back();
}

void popStackFrame() {
if (!currentStackFrames.empty()) {
    currentStackFrames.pop_back();
}
}

void updateHeapObject(const std::string& address, const std::string& value) {
currentHeapObjects[address] = value;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(1000, 500), "C to SFML Compiler Visualization");
    setupSFML(window);
    window.setFramerateLimit(60);

    // --- Primera pasada: Ejecutar la simulación C para registrar todos los pasos ---
    run_c_program_simulation();
    currentStepIndex = 0; // Comienza en el primer paso registrado

    // --- Bucle principal de eventos SFML para la navegación ---
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    const float BUTTON_WIDTH = 100.f;
                    const float BUTTON_HEIGHT = 40.f;
                    const float BUTTON_Y = window.getSize().y - BUTTON_HEIGHT - 20;

                    // Botón Siguiente
                    const float NEXT_BUTTON_X = window.getSize().x / 2 + 10;
                    sf::FloatRect nextButtonBounds(NEXT_BUTTON_X, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT);
                    if (nextButtonBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                        if (currentStepIndex < simulationHistory.size() - 1) {
                            currentStepIndex++;
                            }
                        }

                        // Botón Anterior
                        const float PREV_BUTTON_X = window.getSize().x / 2 - BUTTON_WIDTH - 10;
                        sf::FloatRect prevButtonBounds(PREV_BUTTON_X, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT);
                        if (prevButtonBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                            if (currentStepIndex > 0) {
                                currentStepIndex--;
                                }
                            }
                        }
                    }
                }
                if (!simulationHistory.empty()) {
                    displaySpecificStep(simulationHistory[currentStepIndex]);
                }
                sf::sleep(sf::milliseconds(10)); // Pequeño sleep para reducir el uso de CPU
            }
            return 0;
}
