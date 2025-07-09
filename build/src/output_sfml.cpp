#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include <map>
#include <algorithm>

// Objetos y constantes globales de SFML
sf::RenderWindow* globalWindow = nullptr;
sf::Font globalFont;
const float LINE_HEIGHT = 25.f;
const sf::Color COLOR_DEFAULT_TEXT = sf::Color::White;
const sf::Color COLOR_HIGHLIGHT = sf::Color::Yellow;
const sf::Color COLOR_VARIABLE_DECL = sf::Color::Cyan;
const sf::Color COLOR_ASSIGNMENT = sf::Color::Magenta;
const sf::Color COLOR_FUNCTION_CALL = sf::Color::Green;
const sf::Color COLOR_RETURN = sf::Color::Red;
const sf::Color COLOR_PRINT = sf::Color::Blue;

// Estado global para la instantánea de memoria actual (usado durante la grabación)
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
const float MAIN_AREA_Y_OFFSET = 60.f;
const float STACK_AREA_X = 50.f;
const float HEAP_AREA_X = 400.f;
const float MEMORY_AREA_WIDTH = 300.f;
const float MEMORY_AREA_HEIGHT = 400.f;

const float CELL_HEIGHT = 30.f;
const float CELL_DEFAULT_WIDTH = 80.f;
const float CELL_PADDING = 5.f;

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
    recordStep("Program Started", COLOR_DEFAULT_TEXT);
    recordStep("Entering function: main", COLOR_FUNCTION_CALL);
    pushStackFrame();
    {
        recordStep("Declaring: int a = 5", COLOR_VARIABLE_DECL);
        int a = 5;
        updateStackFrame("a", std::to_string(5));
        recordStep("Declaring: int b = 10", COLOR_VARIABLE_DECL);
        int b = 10;
        updateStackFrame("b", std::to_string(10));
        recordStep("Declaring: int sum = (a + b)", COLOR_VARIABLE_DECL);
        int sum = (a + b);
        updateStackFrame("sum", std::to_string((a + b)));
        recordStep("Declaring: int sum2 = ((a + b) + 5)", COLOR_VARIABLE_DECL);
        int sum2 = ((a + b) + 5);
        updateStackFrame("sum2", std::to_string(((a + b) + 5)));
    }
    recordStep("Exiting function: main", COLOR_RETURN);
    popStackFrame();
    // Fin del programa
    recordStep("Program Ended", COLOR_DEFAULT_TEXT);
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
globalWindow->clear(sf::Color::Black);
displayText(step.description, 10.f, 10.f, step.color, 18);

// --- Dibujar Área del Heap ---
displayText("Heap", HEAP_AREA_X + 10, MAIN_AREA_Y_OFFSET - 25, COLOR_DEFAULT_TEXT, 20);
drawRectangle(HEAP_AREA_X, MAIN_AREA_Y_OFFSET, MEMORY_AREA_WIDTH, MEMORY_AREA_HEIGHT, sf::Color(150, 150, 150, 100), true, 1.f, sf::Color::White);
float currentHeapY = MAIN_AREA_Y_OFFSET + CELL_PADDING;
for (const auto& objPair : step.heapSnapshot) {
    displayText(objPair.first + ": " + objPair.second, HEAP_AREA_X + CELL_PADDING, currentHeapY, COLOR_DEFAULT_TEXT);
    currentHeapY += CELL_HEIGHT + CELL_PADDING;
}

// --- Dibujar Área de la Pila ---
displayText("Stack", STACK_AREA_X + 10, MAIN_AREA_Y_OFFSET - 25, COLOR_DEFAULT_TEXT, 20);
drawRectangle(STACK_AREA_X, MAIN_AREA_Y_OFFSET, MEMORY_AREA_WIDTH, MEMORY_AREA_HEIGHT, sf::Color(150, 150, 150, 100), true, 1.f, sf::Color::White);
float currentStackY = MAIN_AREA_Y_OFFSET + MEMORY_AREA_HEIGHT - CELL_HEIGHT - CELL_PADDING;
float boxWidth = 0.0f;
float boxHeight = CELL_HEIGHT;
// Dibujar los marcos de la pila de abajo hacia arriba (iteración inversa)
for (int i = static_cast<int>(step.stackSnapshot.size()) - 1; i >= 0; --i) {
    float currentStackX = STACK_AREA_X + CELL_PADDING;
    std::string frameLabel = "main";
    if (i > 0) {
        frameLabel = "Func" + std::to_string(i + 1);
    }
    sf::Color frameLabelColor = sf::Color(100, 200, 100, 200);
    if (i > 0) frameLabelColor = sf::Color(50, 50, 150, 200);
    sf::Text frameText(frameLabel, globalFont, 16);
    float frameLabelWidth = std::max(CELL_DEFAULT_WIDTH, frameText.getLocalBounds().width + CELL_PADDING * 2);
    drawRectangle(currentStackX, currentStackY, frameLabelWidth, CELL_HEIGHT, frameLabelColor, true, 1.f, sf::Color::Black);
    displayText(frameLabel, currentStackX + CELL_PADDING, currentStackY + CELL_PADDING, sf::Color::Black, 16);
    currentStackX += frameLabelWidth + CELL_PADDING;
    for (const auto& varPair : step.stackSnapshot[i]) {
        std::string varName = varPair.first;
        std::string varValue = varPair.second;
        sf::Color varCellColor = sf::Color(200, 200, 200, 200);
        sf::Color varTextColor = sf::Color::Black;
        if (varValue.rfind("0x", 0) == 0) {
            varCellColor = sf::Color(255, 255, 100, 200);
        } else if (varName == "a") {
            varCellColor = sf::Color(100, 200, 100, 200);
            varTextColor = sf::Color::Black;
        }
        sf::Text nameText(varName, globalFont, 16);
        sf::Text valueText(varValue, globalFont, 16);
        float requiredWidth = nameText.getLocalBounds().width + valueText.getLocalBounds().width + (CELL_PADDING * 3);
        boxWidth = std::max(CELL_DEFAULT_WIDTH, requiredWidth);
        // Asegurarse de que la caja encaje dentro del área de memoria, simple verificación de desbordamiento
        if (currentStackX + boxWidth > STACK_AREA_X + MEMORY_AREA_WIDTH - CELL_PADDING) {
            // Si se desborda, simplemente usa el ancho restante para evitar salirse visualmente de los límites.
            // Para una solución más robusta, considera envolver a la siguiente línea o hacer los marcos de la pila más altos.
            boxWidth = (STACK_AREA_X + MEMORY_AREA_WIDTH - CELL_PADDING) - currentStackX;
            if (boxWidth < CELL_DEFAULT_WIDTH / 2) boxWidth = CELL_DEFAULT_WIDTH / 2;
        }
        drawRectangle(currentStackX, currentStackY, boxWidth, CELL_HEIGHT, varCellColor, true, 1.f, sf::Color::Black);
        displayText(varName, currentStackX + CELL_PADDING, currentStackY + CELL_PADDING, varTextColor, 16);
        displayText(varValue, currentStackX + nameText.getLocalBounds().width + (CELL_PADDING * 2), currentStackY + CELL_PADDING, varTextColor, 16);
        currentStackX += boxWidth + CELL_PADDING;
    }
    currentStackY -= (CELL_HEIGHT + CELL_PADDING); // Mover hacia arriba para el siguiente marco de la pila
}

// --- Dibujar botones "Previous" y "Next" ---
const float BUTTON_WIDTH = 100.f;
const float BUTTON_HEIGHT = 40.f;
const float BUTTON_Y = globalWindow->getSize().y - BUTTON_HEIGHT - 20;

// Botón Anterior
const float PREV_BUTTON_X = globalWindow->getSize().x / 2 - BUTTON_WIDTH - 10;
sf::RectangleShape prevButton(sf::Vector2f(BUTTON_WIDTH, BUTTON_HEIGHT));
prevButton.setPosition(PREV_BUTTON_X, BUTTON_Y);
prevButton.setFillColor(currentStepIndex > 0 ? sf::Color(70, 70, 70) : sf::Color(30, 30, 30));
prevButton.setOutlineThickness(2);
prevButton.setOutlineColor(sf::Color::White);
globalWindow->draw(prevButton);
displayText("Previous", PREV_BUTTON_X + (BUTTON_WIDTH - sf::Text("Previous", globalFont, 18).getLocalBounds().width) / 2, BUTTON_Y + (BUTTON_HEIGHT - 18) / 2, sf::Color::White);

// Botón Siguiente
const float NEXT_BUTTON_X = globalWindow->getSize().x / 2 + 10;
sf::RectangleShape nextButton(sf::Vector2f(BUTTON_WIDTH, BUTTON_HEIGHT));
nextButton.setPosition(NEXT_BUTTON_X, BUTTON_Y);
nextButton.setFillColor(currentStepIndex < simulationHistory.size() - 1 ? sf::Color(70, 70, 70) : sf::Color(30, 30, 30));
nextButton.setOutlineThickness(2);
nextButton.setOutlineColor(sf::Color::White);
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
