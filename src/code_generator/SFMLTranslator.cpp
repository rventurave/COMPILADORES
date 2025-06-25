#include "SFMLTranslator.h"

// Constructor
SFMLTranslator::SFMLTranslator() : indentLevel(0), indentString("    ") {} // 4 espacios por nivel

// Aumenta el nivel de indentación
void SFMLTranslator::increaseIndent() {
    indentLevel++;
}

// Disminuye el nivel de indentación
void SFMLTranslator::decreaseIndent() {
    if (indentLevel > 0) {
        indentLevel--;
    }
}

// Obtiene la cadena de indentación actual
std::string SFMLTranslator::getCurrentIndent() {
    std::string indent = "";
    for (int i = 0; i < indentLevel; ++i) {
        indent += indentString;
    }
    return indent;
}

// Genera el encabezado estándar para un archivo SFML C++
std::string SFMLTranslator::getSFMLHeader() {
    std::stringstream ss;
    ss << "#include <SFML/Graphics.hpp>" << std::endl;
    ss << "#include <SFML/Window.hpp>" << std::endl;
    ss << "#include <SFML/System.hpp>" << std::endl;
    ss << "#include <iostream>" << std::endl;
    ss << "#include <string>" << std::endl;
    ss << "#include <vector>" << std::endl;
    ss << "#include <sstream>" << std::endl;
    ss << std::endl;
    ss << "// Colores de visualización" << std::endl;
    ss << "#define COLOR_VARIABLE_DECL sf::Color(100, 100, 250)" << std::endl; // Azul claro
    ss << "#define COLOR_ASSIGNMENT sf::Color(250, 150, 100)"      << std::endl; // Naranja claro
    ss << "#define COLOR_CONTROL_FLOW sf::Color(100, 250, 100)"     << std::endl; // Verde claro
    ss << "#define COLOR_FUNCTION_CALL sf::Color(250, 100, 200)"    << std::endl; // Rosa claro
    ss << "#define COLOR_PRINT_OUTPUT sf::Color(200, 200, 50)"     << std::endl; // Amarillo verdoso
    ss << "#define COLOR_DEFAULT_TEXT sf::Color::White"           << std::endl;
    ss << std::endl;
    ss << "// Dimensiones de visualización (simuladas)" << std::endl;
    ss << "const float LINE_HEIGHT = 30.f;" << std::endl;
    ss << "float currentY = 50.f;" << std::endl;
    ss << "sf::Font font;" << std::endl;
    ss << "sf::RenderWindow* globalWindow = nullptr;" << std::endl;
    ss << std::endl;
    ss << "void setupSFML(sf::RenderWindow& window);" << std::endl;
    ss << "void displayText(const std::string& text, float x, float y, sf::Color color = COLOR_DEFAULT_TEXT);" << std::endl;
    ss << "void drawRectangle(float x, float y, float width, float height, sf::Color color);" << std::endl;
    ss << "void visualizeStep(const std::string& description, sf::Color color = sf::Color::Cyan);" << std::endl;
    ss << std::endl;
    ss << "void setupSFML(sf::RenderWindow& window) {" << std::endl;
    ss << "    globalWindow = &window;" << std::endl;
    
    ss << "    if (!font.loadFromFile(\"../../resources/arial.ttf\")) {" << std::endl;
    ss << "        std::cerr << \"Error cargando fuente arial.ttf\" << std::endl;" << std::endl;
    ss << "    }" << std::endl;
    ss << "}" << std::endl;
    ss << std::endl;
    ss << "void displayText(const std::string& text, float x, float y, sf::Color color) {" << std::endl;
    ss << "    sf::Text sfText;" << std::endl;
    ss << "    sfText.setFont(font);" << std::endl;
    ss << "    sfText.setString(text);" << std::endl;
    ss << "    sfText.setCharacterSize(20);" << std::endl;
    ss << "    sfText.setFillColor(color);" << std::endl;
    ss << "    sfText.setPosition(x, y);" << std::endl;
    ss << "    if (globalWindow) {" << std::endl;
    ss << "        globalWindow->draw(sfText);" << std::endl;
    ss << "    }" << std::endl;
    ss << "}" << std::endl;
    ss << std::endl;
    ss << "void drawRectangle(float x, float y, float width, float height, sf::Color color) {" << std::endl;
    ss << "    sf::RectangleShape rect(sf::Vector2f(width, height));" << std::endl;
    ss << "    rect.setPosition(x, y);" << std::endl;
    ss << "    rect.setFillColor(color);" << std::endl;
    ss << "    if (globalWindow) {" << std::endl;
    ss << "        globalWindow->draw(rect);" << std::endl;
    ss << "    }" << std::endl;
    ss << "}" << std::endl;
    ss << std::endl;
    ss << "void visualizeStep(const std::string& description, sf::Color color) {" << std::endl;
    ss << "    if (!globalWindow) return;" << std::endl;
    ss << "    globalWindow->clear(sf::Color::Black);" << std::endl;
    ss << "    displayText(description, 10.f, 10.f, color);" << std::endl;
    ss << "    // Aquí se podrían dibujar las variables actuales, el stack, etc." << std::endl;
    ss << "    globalWindow->display();" << std::endl;
    ss << "    sf::Event event;" << std::endl;
    ss << "    while (globalWindow->pollEvent(event)) {" << std::endl;
    ss << "        if (event.type == sf::Event::Closed) {" << std::endl;
    ss << "            globalWindow->close();" << std::endl;
    ss << "        }" << std::endl;
    ss << "    }" << std::endl;
    ss << "    // Pequeño retardo para ver la visualización, en un sistema real esperarías una entrada de usuario" << std::endl;
    ss << "    sf::sleep(sf::milliseconds(500));" << std::endl; // Retardo de 0.5 segundos
    ss << "}" << std::endl;
    ss << std::endl;
    return ss.str();
}

// Genera código SFML para el seguimiento de variables
std::string SFMLTranslator::generateVariableDeclaration(const std::string& type, const std::string& name, const std::string& initialValue) {
    std::stringstream ss;
    ss << getCurrentIndent() << "// Visualizando: Declaración de variable " << type << " " << name << std::endl;
    ss << getCurrentIndent() << "visualizeStep(\"Declarando: " << type << " " << name << (initialValue.empty() ? "" : " = " + initialValue) << ";\", COLOR_VARIABLE_DECL);" << std::endl;
    ss << getCurrentIndent() << type << " " << name;
    if (!initialValue.empty()) {
        ss << " = " << initialValue;
    }
    ss << ";" << std::endl;
    return ss.str();
}

std::string SFMLTranslator::generateAssignment(const std::string& variableName, const std::string& expression) {
    std::stringstream ss;
    ss << getCurrentIndent() << "// Visualizando: Asignación a " << variableName << std::endl;
    ss << getCurrentIndent() << "visualizeStep(\"Asignando a " << variableName << ": " << expression << "\", COLOR_ASSIGNMENT);" << std::endl;
    ss << getCurrentIndent() << variableName << " = " << expression << ";" << std::endl;
    return ss.str();
}

// Genera código SFML para el seguimiento de control de flujo
std::string SFMLTranslator::generateIfStart(const std::string& condition) {
    std::stringstream ss;
    ss << getCurrentIndent() << "// Visualizando: Inicio de IF" << std::endl;
    ss << getCurrentIndent() << "visualizeStep(\"Evaluando IF: " << condition << "\", COLOR_CONTROL_FLOW);" << std::endl;
    ss << getCurrentIndent() << "if (" << condition << ") {" << std::endl;
    increaseIndent();
    return ss.str();
}

std::string SFMLTranslator::generateElseStart() {
    std::stringstream ss;
    decreaseIndent();
    ss << getCurrentIndent() << "} else {" << std::endl;
    ss << getCurrentIndent() << "// Visualizando: Inicio de ELSE" << std::endl;
    ss << getCurrentIndent() << "visualizeStep(\"Entrando al bloque ELSE\", COLOR_CONTROL_FLOW);" << std::endl;
    increaseIndent();
    return ss.str();
}

std::string SFMLTranslator::generateBlockEnd() {
    std::stringstream ss;
    decreaseIndent();
    ss << getCurrentIndent() << "}" << std::endl;
    return ss.str();
}

std::string SFMLTranslator::generateForStart(const std::string& init, const std::string& condition, const std::string& increment) {
    std::stringstream ss;
    ss << getCurrentIndent() << "// Visualizando: Inicio de FOR" << std::endl;
    ss << getCurrentIndent() << "visualizeStep(\"Iniciando bucle FOR: " << init << "; " << condition << "; " << increment << "\", COLOR_CONTROL_FLOW);" << std::endl;
    ss << getCurrentIndent() << "for (" << init << " " << condition << "; " << increment << ") {" << std::endl;
    increaseIndent();
    return ss.str();
}

// Genera código SFML para la visualización de printf
std::string SFMLTranslator::generatePrint(const std::string& formatString, const std::vector<std::string>& arguments) {
    std::stringstream ss;
    std::string argList = "";
    for (const auto& arg : arguments) {
        if (!argList.empty()) argList += ", ";
        argList += arg;
    }
    std::string escapedFormatString = formatString;
    // Escapar comillas si la cadena de formato ya incluye comillas
    size_t pos = escapedFormatString.find('"');
    while(pos != std::string::npos) {
        escapedFormatString.replace(pos, 1, "\\\"");
        pos = escapedFormatString.find('"', pos + 2);
    }
    ss << getCurrentIndent() << "// Visualizando: printf" << std::endl;
    ss << getCurrentIndent() << "{" << std::endl; // Usamos un bloque para encapsular la ss temporal
    increaseIndent();
    ss << getCurrentIndent() << "std::stringstream printf_output_ss;" << std::endl;
    ss << getCurrentIndent() << "printf_output_ss << \"" << escapedFormatString << "\"";
    for (const auto& arg : arguments) {
        ss << " << " << arg;
    }
    ss << ";" << std::endl;
    ss << getCurrentIndent() << "visualizeStep(\"printf: \" + printf_output_ss.str(), COLOR_PRINT_OUTPUT);" << std::endl;
    decreaseIndent();
    ss << getCurrentIndent() << "}" << std::endl;
    return ss.str();
}

// Genera código SFML para llamadas a funciones (visualización)
std::string SFMLTranslator::generateFunctionCall(const std::string& functionName, const std::vector<std::string>& arguments) {
    std::stringstream ss;
    std::string argList = "";
    for (const auto& arg : arguments) {
        if (!argList.empty()) argList += ", ";
        argList += arg;
    }

    ss << getCurrentIndent() << "// Visualizando: Llamada a función " << functionName << std::endl;
    ss << getCurrentIndent() << "visualizeStep(\"Llamando a función: " << functionName << "(" << argList << ")\", COLOR_FUNCTION_CALL);" << std::endl;

    // Evita llamada recursiva a main
    if (functionName == "main") {
        ss << getCurrentIndent() << "// Evitando llamada recursiva a main()" << std::endl;
    } else {
        ss << getCurrentIndent() << functionName << "(" << argList << ");" << std::endl;
    }

    return ss.str();
}

std::string SFMLTranslator::generateFunctionEntry(const std::string& functionName, const std::vector<std::pair<std::string, std::string>>& params) {
    std::stringstream ss;
    std::string paramList = "";
    for (const auto& param : params) {
        if (!paramList.empty()) paramList += ", ";
        paramList += param.first + " " + param.second;
    }
    ss << getCurrentIndent() << "// Visualizando: Entrada a función " << functionName << std::endl;
    ss << getCurrentIndent() << "visualizeStep(\"Entrando a función: " << functionName << "(" << paramList << ")\", COLOR_FUNCTION_CALL);" << std::endl;
    return ss.str();
}

std::string SFMLTranslator::generateFunctionExit(const std::string& functionName, const std::string& returnValue) {
    std::stringstream ss;
    ss << getCurrentIndent() << "// Visualizando: Salida de función " << functionName << std::endl;
    std::string exitMsg = "Saliendo de función: " + functionName;
    if (!returnValue.empty()) {
        exitMsg += " (Retorna: " + returnValue + ")";
    }
    ss << getCurrentIndent() << "visualizeStep(\"" << exitMsg << "\", COLOR_FUNCTION_CALL);" << std::endl;
    return ss.str();
}


// Genera el inicio de la función main de SFML
std::string SFMLTranslator::generateSFMLMainStart() {
    std::stringstream ss;
    ss << std::endl;
    ss << "int main() {" << std::endl;
    increaseIndent();
    ss << getCurrentIndent() << "sf::RenderWindow window(sf::VideoMode(800, 600), \"C to SFML Visualizer\");" << std::endl;
    ss << getCurrentIndent() << "setupSFML(window);" << std::endl;
    ss << getCurrentIndent() << "window.setFramerateLimit(60);" << std::endl;
    ss << std::endl;
    ss << getCurrentIndent() << "while (window.isOpen()) {" << std::endl;
    increaseIndent();
    ss << getCurrentIndent() << "sf::Event event;" << std::endl;
    ss << getCurrentIndent() << "while (window.pollEvent(event)) {" << std::endl;
    increaseIndent();
    ss << getCurrentIndent() << "if (event.type == sf::Event::Closed) {" << std::endl;
    increaseIndent();
    ss << getCurrentIndent() << "window.close();" << std::endl;
    decreaseIndent();
    ss << getCurrentIndent() << "}" << std::endl;
    decreaseIndent();
    ss << getCurrentIndent() << "}" << std::endl;
    ss << std::endl;
    ss << getCurrentIndent() << "// --- Inicio de la simulación del código C ---" << std::endl;
    return ss.str();
}

// Genera el fin de la función main de SFML
std::string SFMLTranslator::generateSFMLMainEnd() {
    std::stringstream ss;
    ss << getCurrentIndent() << "// --- Fin de la simulación del código C ---" << std::endl;
    decreaseIndent();
    ss << getCurrentIndent() << "}" << std::endl; // Cierre del while(window.isOpen())
    ss << getCurrentIndent() << "return 0;" << std::endl;
    decreaseIndent();
    ss << "}" << std::endl; // Cierre del main
    return ss.str();
}

// Métodos auxiliares para la generación de código SFML visual (prototipos, no usados directamente por CodeGenerator)
std::string SFMLTranslator::generateVisualComment(const std::string& comment) {
    return "// " + comment + "\n";
}

std::string SFMLTranslator::generateTextDisplay(const std::string& text, float x, float y) {
    std::stringstream ss;
    ss << "displayText(\"" << text << "\", " << x << "f, " << y << "f);" << std::endl;
    return ss.str();
}

std::string SFMLTranslator::generateShapeDisplay(const std::string& shapeType, float x, float y, float width, float height, const std::string& color) {
    std::stringstream ss;
    if (shapeType == "rectangle") {
        ss << "drawRectangle(" << x << "f, " << y << "f, " << width << "f, " << height << "f, " << color << ");" << std::endl;
    }
    return ss.str();
}