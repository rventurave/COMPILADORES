#ifndef SFML_TRANSLATOR_H
#define SFML_TRANSLATOR_H

#include <string>
#include <vector>
#include <sstream>
#include <map>

// Clase SFMLTranslator: Contiene lógica para traducir elementos C a pseudocódigo o visualizaciones SFML.
class SFMLTranslator {
public:
    SFMLTranslator();

    // Inicia el archivo de código SFML con includes y funciones básicas
    std::string getSFMLHeader();

    // Genera código SFML para el seguimiento de variables
    std::string generateVariableDeclaration(const std::string& type, const std::string& name, const std::string& initialValue = "");
    std::string generateAssignment(const std::string& variableName, const std::string& expression);

    // Genera código SFML para el seguimiento de control de flujo
    std::string generateIfStart(const std::string& condition);
    std::string generateElseStart();
    std::string generateBlockEnd(); // Para cerrar bloques if/else/for
    std::string generateForStart(const std::string& init, const std::string& condition, const std::string& increment);

    // Genera código SFML para la visualización de printf
    std::string generatePrint(const std::string& formatString, const std::vector<std::string>& arguments);

    // Genera código SFML para llamadas a funciones (visualización)
    std::string generateFunctionCall(const std::string& functionName, const std::vector<std::string>& arguments);
    std::string generateFunctionEntry(const std::string& functionName, const std::vector<std::pair<std::string, std::string>>& params);
    std::string generateFunctionExit(const std::string& functionName, const std::string& returnValue = "");

    // Genera el inicio y fin de la visualización SFML
    std::string generateSFMLMainStart();
    std::string generateSFMLMainEnd();

    // Gestión de indentación para el código generado
    void increaseIndent();
    void decreaseIndent();
    std::string getCurrentIndent();

private:
    int indentLevel;
    std::string indentString;

    // Mapa para almacenar el estado de las variables para la visualización
    // (nombre_variable -> valor_actual) - esto sería simplificado
    std::map<std::string, std::string> variableStates;

    // Métodos auxiliares para la generación de código SFML visual
    std::string generateVisualComment(const std::string& comment);
    std::string generateTextDisplay(const std::string& text, float x, float y);
    std::string generateShapeDisplay(const std::string& shapeType, float x, float y, float width, float height, const std::string& color);
};

#endif // SFML_TRANSLATOR_H