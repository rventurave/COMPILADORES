#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include <map>

// Global SFML objects and constants
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

// Global state for Stack Visualization
std::vector<std::map<std::string, std::string>> stackFrames;
std::map<std::string, std::string> heapObjects;
const float STACK_RECT_X = 50.f;
const float STACK_RECT_Y = 80.f;
const float STACK_RECT_WIDTH = 300.f;
const float STACK_RECT_HEIGHT = 400.f;
const float HEAP_RECT_X = 400.f;
const float HEAP_RECT_Y = 80.f;
const float HEAP_RECT_WIDTH = 300.f;
const float HEAP_RECT_HEIGHT = 400.f;

// Helper functions prototypes (defined in getSFMLFooter)
void setupSFML(sf::RenderWindow& window);
void displayText(const std::string& text_str, float x, float y, sf::Color color);
void drawRectangle(float x, float y, float width, float height, sf::Color color, bool filled, float outlineThickness, sf::Color outlineColor);
void visualizeStep(const std::string& description, sf::Color color);
std::string generateProgramStart();
int fo() {
    {
        visualizeStep("Declaring: int x = 5", COLOR_VARIABLE_DECL);
        int x = 5;
        if (!stackFrames.empty()) {
            stackFrames.back()["x"] = std::to_string(5);
        }
        visualizeStep("Declaring: int y = 10", COLOR_VARIABLE_DECL);
        int y = 10;
        if (!stackFrames.empty()) {
            stackFrames.back()["y"] = std::to_string(10);
        }
        visualizeStep("Declaring: int suma = (x + y)", COLOR_VARIABLE_DECL);
        int suma = (x + y);
        if (!stackFrames.empty()) {
            stackFrames.back()["suma"] = std::to_string((x + y));
        }
        visualizeStep("Returning from fo with value: 0", COLOR_RETURN);
        return 0;
        visualizeStep("Exiting function: fo (Retorna: 0)", COLOR_RETURN);
        // Exit stack frame for fo
        if (!stackFrames.empty()) {
            stackFrames.pop_back();
        }
    }
}

void run_c_program_simulation() {
    // Program Start
    visualizeStep("Program Started", COLOR_DEFAULT_TEXT);
    visualizeStep("Entering function: global_scope", COLOR_FUNCTION_CALL);
    // Enter new stack frame for global_scope
    stackFrames.emplace_back();
    visualizeStep("Exiting function: global_scope", COLOR_RETURN);
    // Exit stack frame for global_scope
    if (!stackFrames.empty()) {
        stackFrames.pop_back();
    }
    // Program End
    visualizeStep("Program Ended", COLOR_DEFAULT_TEXT);
}

// Helper functions implementations
void setupSFML(sf::RenderWindow& window) {
globalWindow = &window;
if (!globalFont.loadFromFile("../../resources/arial.ttf")) {
    std::cerr << "Error loading font: ../../resources/arial.ttf" << std::endl;
}
}

void displayText(const std::string& text_str, float x, float y, sf::Color color) {
if (!globalWindow) return;
sf::Text text(text_str, globalFont, 18);
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

void visualizeStep(const std::string& description, sf::Color color) {
if (!globalWindow || !globalWindow->isOpen()) return;
globalWindow->clear(sf::Color::Black);
displayText(description, 10.f, 10.f, color);
// Draw Stack Rectangle
drawRectangle(STACK_RECT_X, STACK_RECT_Y, STACK_RECT_WIDTH, STACK_RECT_HEIGHT, sf::Color(50, 50, 150, 100), true, 0.f, sf::Color::Black);
displayText("Stack:", STACK_RECT_X + 10, STACK_RECT_Y + 5, sf::Color::White);
// Draw Stack Variables
float currentVariableY = STACK_RECT_Y + LINE_HEIGHT + 10;
for (size_t i = 0; i < stackFrames.size(); ++i) {
    displayText("Scope " + std::to_string(i + 1) + ":", STACK_RECT_X + 20, currentVariableY, sf::Color::Yellow);
    currentVariableY += LINE_HEIGHT;
    for (const auto& varPair : stackFrames[i]) {
        displayText(varPair.first + " = " + varPair.second, STACK_RECT_X + 40, currentVariableY, COLOR_DEFAULT_TEXT);
        currentVariableY += LINE_HEIGHT;
    }
}
// Draw Heap Rectangle (Placeholder)
drawRectangle(HEAP_RECT_X, HEAP_RECT_Y, HEAP_RECT_WIDTH, HEAP_RECT_HEIGHT, sf::Color(150, 50, 50, 100), true, 0.f, sf::Color::Black);
displayText("Heap:", HEAP_RECT_X + 10, HEAP_RECT_Y + 5, sf::Color::White);
float currentHeapY = HEAP_RECT_Y + LINE_HEIGHT + 10;
for (const auto& objPair : heapObjects) {
    displayText(objPair.first + ": " + objPair.second, HEAP_RECT_X + 20, currentHeapY, COLOR_DEFAULT_TEXT);
    currentHeapY += LINE_HEIGHT;
}
globalWindow->display();
sf::Event event;
while (globalWindow->pollEvent(event)) {
    if (event.type == sf::Event::Closed) {
        globalWindow->close();
    }
}
sf::sleep(sf::milliseconds(500));
}

int main() {
    sf::RenderWindow window(sf::VideoMode(900, 600), "C to SFML Compiler Visualization");
    setupSFML(window);
    window.setFramerateLimit(60);

    // --- Inicio de la simulación del código C ---
    run_c_program_simulation(); // Llama a la lógica del programa C simulado
    // --- Fin de la simulación del código C ---

    // Mantener la ventana abierta después de que la simulación termine,
    // esperando que el usuario la cierre.
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        sf::sleep(sf::milliseconds(50)); // Pequeño sleep para reducir el uso de CPU
    }
    return 0;
}
