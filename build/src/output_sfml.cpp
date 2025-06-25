#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

// Colores de visualización
#define COLOR_VARIABLE_DECL sf::Color(100, 100, 250)
#define COLOR_ASSIGNMENT sf::Color(250, 150, 100)
#define COLOR_CONTROL_FLOW sf::Color(100, 250, 100)
#define COLOR_FUNCTION_CALL sf::Color(250, 100, 200)
#define COLOR_PRINT_OUTPUT sf::Color(200, 200, 50)
#define COLOR_DEFAULT_TEXT sf::Color::White

// Dimensiones de visualización (simuladas)
const float LINE_HEIGHT = 30.f;
float currentY = 50.f;
sf::Font font;
sf::RenderWindow* globalWindow = nullptr;

void setupSFML(sf::RenderWindow& window);
void displayText(const std::string& text, float x, float y, sf::Color color = COLOR_DEFAULT_TEXT);
void drawRectangle(float x, float y, float width, float height, sf::Color color);
void visualizeStep(const std::string& description, sf::Color color = sf::Color::Cyan);

void setupSFML(sf::RenderWindow& window) {
    globalWindow = &window;
    if (!font.loadFromFile("../../resources/arial.ttf")) {
        std::cerr << "Error cargando fuente arial.ttf" << std::endl;
    }
}

void displayText(const std::string& text, float x, float y, sf::Color color) {
    sf::Text sfText;
    sfText.setFont(font);
    sfText.setString(text);
    sfText.setCharacterSize(20);
    sfText.setFillColor(color);
    sfText.setPosition(x, y);
    if (globalWindow) {
        globalWindow->draw(sfText);
    }
}

void drawRectangle(float x, float y, float width, float height, sf::Color color) {
    sf::RectangleShape rect(sf::Vector2f(width, height));
    rect.setPosition(x, y);
    rect.setFillColor(color);
    if (globalWindow) {
        globalWindow->draw(rect);
    }
}

void visualizeStep(const std::string& description, sf::Color color) {
    if (!globalWindow) return;
    globalWindow->clear(sf::Color::Black);
    displayText(description, 10.f, 10.f, color);
    // Aquí se podrían dibujar las variables actuales, el stack, etc.
    globalWindow->display();
    sf::Event event;
    while (globalWindow->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            globalWindow->close();
        }
    }
    // Pequeño retardo para ver la visualización, en un sistema real esperarías una entrada de usuario
    sf::sleep(sf::milliseconds(500));
}


int fo() {
    // Visualizando: Entrada a función fo
    visualizeStep("Entrando a función: fo()", COLOR_FUNCTION_CALL);
    {
        // Visualizando: Declaración de variable int x
        visualizeStep("Declarando: int x = 5;", COLOR_VARIABLE_DECL);
        int x = 5;
        // Visualizando: Declaración de variable int y
        visualizeStep("Declarando: int y = 10;", COLOR_VARIABLE_DECL);
        int y = 10;
        // Visualizando: Declaración de variable int suma
        visualizeStep("Declarando: int suma = x + y;", COLOR_VARIABLE_DECL);
        int suma = x + y;
        return 0;
        // Visualizando: Salida de función current_function_name_placeholder
        visualizeStep("Saliendo de función: current_function_name_placeholder (Retorna: 0)", COLOR_FUNCTION_CALL);
    }
}

int main() {
    // Visualizando: Entrada a función main
    visualizeStep("Entrando a función: main()", COLOR_FUNCTION_CALL);
    {
        // Visualizando: Llamada a función fo
        visualizeStep("Llamando a función: fo()", COLOR_FUNCTION_CALL);
        fo();
        return 0;
        // Visualizando: Salida de función current_function_name_placeholder
        visualizeStep("Saliendo de función: current_function_name_placeholder (Retorna: 0)", COLOR_FUNCTION_CALL);
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "C to SFML Visualizer");
    setupSFML(window);
    window.setFramerateLimit(60);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        // --- Inicio de la simulación del código C ---
                // Visualizando: Llamada a función main
                visualizeStep("Llamando a función: main()", COLOR_FUNCTION_CALL);
                // Evitando llamada recursiva a main()
                // --- Fin de la simulación del código C ---
            }
            return 0;
}
