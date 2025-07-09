#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include <map>
#include <algorithm>

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

// Global state for current memory snapshot (used during recording)
std::vector<std::map<std::string, std::string>> currentStackFrames;
std::map<std::string, std::string> currentHeapObjects;

// --- Simulation History Globals ---
struct SimulationStep {
    std::string description;
    sf::Color color;
    std::vector<std::map<std::string, std::string>> stackSnapshot;
    std::map<std::string, std::string> heapSnapshot;
};
std::vector<SimulationStep> simulationHistory;
int currentStepIndex = 0;

// Adjusted positions and sizes for layout based on the image
const float MAIN_AREA_Y_OFFSET = 60.f;
const float STACK_AREA_X = 50.f;
const float HEAP_AREA_X = 400.f;
const float MEMORY_AREA_WIDTH = 300.f;
const float MEMORY_AREA_HEIGHT = 400.f;

const float CELL_HEIGHT = 30.f;
const float CELL_DEFAULT_WIDTH = 80.f;
const float CELL_PADDING = 5.f;

// Helper functions prototypes (implemented in getSFMLFooter)
void setupSFML(sf::RenderWindow& window);
void displayText(const std::string& text_str, float x, float y, sf::Color color, unsigned int characterSize = 18);
void drawRectangle(float x, float y, float width, float height, sf::Color color, bool filled, float outlineThickness, sf::Color outlineColor);
void recordStep(const std::string& description, sf::Color color);
void displaySpecificStep(const SimulationStep& step);
void updateStackFrame(const std::string& varName, const std::string& value);
void pushStackFrame();
void popStackFrame();
void updateHeapObject(const std::string& address, const std::string& value);
void run_c_program_simulation();
void run_c_program_simulation() {
    // Program Start
    recordStep("Program Started", COLOR_DEFAULT_TEXT);
    recordStep("Entering function: main", COLOR_FUNCTION_CALL);
    pushStackFrame();
    {
        // Enter new generic scope
        pushStackFrame();
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
        recordStep("Returning from main with value: sum", COLOR_RETURN);
        recordStep("Exiting function: main (Retorna: sum)", COLOR_RETURN);
        popStackFrame();
        // Exit generic scope
        popStackFrame();
    }
    recordStep("Exiting function: main", COLOR_RETURN);
    popStackFrame();
    // Program End
    recordStep("Program Ended", COLOR_DEFAULT_TEXT);
}

// Helper functions implementations
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
step.stackSnapshot = currentStackFrames; // Deep copy
step.heapSnapshot = currentHeapObjects;   // Deep copy
simulationHistory.push_back(step);
}

void displaySpecificStep(const SimulationStep& step) {
if (!globalWindow || !globalWindow->isOpen()) return;
globalWindow->clear(sf::Color::Black);
displayText(step.description, 10.f, 10.f, step.color);

// --- Draw Heap Area ---
displayText("Heap", HEAP_AREA_X + 10, MAIN_AREA_Y_OFFSET - 25, COLOR_DEFAULT_TEXT, 20);
drawRectangle(HEAP_AREA_X, MAIN_AREA_Y_OFFSET, MEMORY_AREA_WIDTH, MEMORY_AREA_HEIGHT, sf::Color(150, 150, 150, 100), true, 1.f, sf::Color::White);

float currentHeapY = MAIN_AREA_Y_OFFSET + CELL_PADDING;
for (const auto& objPair : step.heapSnapshot) {
    displayText(objPair.first + ": " + objPair.second, HEAP_AREA_X + CELL_PADDING, currentHeapY, COLOR_DEFAULT_TEXT);
    currentHeapY += CELL_HEIGHT + CELL_PADDING;
}

// --- Draw Stack Area ---
displayText("Stack", STACK_AREA_X + 10, MAIN_AREA_Y_OFFSET - 25, COLOR_DEFAULT_TEXT, 20);
drawRectangle(STACK_AREA_X, MAIN_AREA_Y_OFFSET, MEMORY_AREA_WIDTH, MEMORY_AREA_HEIGHT, sf::Color(150, 150, 150, 100), true, 1.f, sf::Color::White);

float currentStackY = MAIN_AREA_Y_OFFSET + MEMORY_AREA_HEIGHT - CELL_HEIGHT - CELL_PADDING;

float boxWidth = 0.0f;
float boxHeight = CELL_HEIGHT;

// Draw stack frames from bottom-up (reverse iteration)
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

        if (currentStackX + boxWidth > STACK_AREA_X + MEMORY_AREA_WIDTH - CELL_PADDING) {
            // Handle overflow for a single row per frame. For multi-row, redesign needed.
            // For simplicity, this example does not implement multi-row drawing per frame.
        }

        drawRectangle(currentStackX, currentStackY, boxWidth, CELL_HEIGHT, varCellColor, true, 1.f, sf::Color::Black);
        displayText(varName, currentStackX + CELL_PADDING, currentStackY + CELL_PADDING, varTextColor, 16);
        displayText(varValue, currentStackX + nameText.getLocalBounds().width + (CELL_PADDING * 2), currentStackY + CELL_PADDING, varTextColor, 16);
        currentStackX += boxWidth + CELL_PADDING;
    }
    currentStackY -= (CELL_HEIGHT + CELL_PADDING); // Move up for the next stack frame
}

// --- Draw "Previous" and "Next" Buttons ---
const float BUTTON_WIDTH = 100.f;
const float BUTTON_HEIGHT = 40.f;
const float BUTTON_Y = globalWindow->getSize().y - BUTTON_HEIGHT - 20;

// Previous Button
const float PREV_BUTTON_X = globalWindow->getSize().x / 2 - BUTTON_WIDTH - 10;
sf::RectangleShape prevButton(sf::Vector2f(BUTTON_WIDTH, BUTTON_HEIGHT));
prevButton.setPosition(PREV_BUTTON_X, BUTTON_Y);
prevButton.setFillColor(currentStepIndex > 0 ? sf::Color(70, 70, 70) : sf::Color(30, 30, 30));
prevButton.setOutlineThickness(2);
prevButton.setOutlineColor(sf::Color::White);
globalWindow->draw(prevButton);
displayText("Previous", PREV_BUTTON_X + (BUTTON_WIDTH - sf::Text("Previous", globalFont, 18).getLocalBounds().width) / 2, BUTTON_Y + (BUTTON_HEIGHT - 18) / 2, sf::Color::White);

// Next Button
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
    sf::RenderWindow window(sf::VideoMode(900, 600), "C to SFML Compiler Visualization");
    setupSFML(window);
    window.setFramerateLimit(60);

    // --- First Pass: Run C simulation to record all steps ---
    run_c_program_simulation();
    currentStepIndex = 0; // Start at the first recorded step

    // --- Main SFML Event Loop for Navigation ---
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
                    const float BUTTON_Y = window.getSize().y - BUTTON_WIDTH/2 - 20;

                    const float NEXT_BUTTON_X = window.getSize().x / 2 + 10;
                    sf::FloatRect nextButtonBounds(NEXT_BUTTON_X, BUTTON_Y, BUTTON_WIDTH, 40);
                    if (nextButtonBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                        if (currentStepIndex < simulationHistory.size() - 1) {
                            currentStepIndex++;
                            displaySpecificStep(simulationHistory[currentStepIndex]);
                        }
                    }

                    const float PREV_BUTTON_X = window.getSize().x / 2 - BUTTON_WIDTH - 10;
                    sf::FloatRect prevButtonBounds(PREV_BUTTON_X, BUTTON_Y, BUTTON_WIDTH, 40);
                    if (prevButtonBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                        if (currentStepIndex > 0) {
                            currentStepIndex--;
                            displaySpecificStep(simulationHistory[currentStepIndex]);
                        }
                    }
                }
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) {
                    if (currentStepIndex < simulationHistory.size() - 1) {
                        currentStepIndex++;
                        displaySpecificStep(simulationHistory[currentStepIndex]);
                    }
                } else if (event.key.code == sf::Keyboard::Left) {
                    if (currentStepIndex > 0) {
                        currentStepIndex--;
                        displaySpecificStep(simulationHistory[currentStepIndex]);
                    }
                }
            }
        }
        // Always display the current step, even if no event happened, to ensure buttons are drawn etc.
        if (!simulationHistory.empty()) {
            displaySpecificStep(simulationHistory[currentStepIndex]);
        }
        sf::sleep(sf::milliseconds(10)); // Small sleep to reduce CPU usage
    }
    return 0;
}
