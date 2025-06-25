// src/code_generator/SFMLTranslator.h
#ifndef SFMLTRANSLATOR_H
#define SFMLTRANSLATOR_H

#include <string>
#include <sstream>
#include <vector>
#include <map> // Necesario para std::map en el SFML generado

// Forward declarations para los nodos del AST, si fueran necesarios directamente aquí.
// Sin embargo, CodeGenerator es quien visita los nodos y llama a SFMLTranslator.

class SFMLTranslator {
public:
    SFMLTranslator();

    // Métodos para el encabezado y pie de página del archivo SFML
    std::string getSFMLHeader();
    std::string getSFMLFooter();

    // Métodos para generar código de visualización
    std::string setupSFML(); // Ya existente, podría necesitar ajustes
    std::string displayText(const std::string& text, float x, float y, const std::string& color_sfml);
    std::string drawRectangle(float x, float y, float width, float height, const std::string& color_sfml);
    std::string visualizeStep(const std::string& description_str, const std::string& color_sfml); // Modificada

    // Métodos para generación de código específico del AST
    std::string generateProgramStart();
    std::string generateProgramEnd();
    std::string generateFunctionDeclaration(const std::string& returnType, const std::string& functionName, const std::string& paramsCode, const std::string& bodyCode);
    std::string generateFunctionCall(const std::string& functionName, const std::string& argsCode);
    std::string generateVariableDeclaration(const std::string& typeName, const std::string& variableName, const std::string& initialValue);
    std::string generateAssignment(const std::string& identifierName, const std::string& expressionCode);
    std::string generateReturnStatement(const std::string& expressionCode, const std::string& functionName); // functionName para mensajes
    std::string generateIfStatement(const std::string& conditionCode, const std::string& thenBlockCode, const std::string& elseBlockCode);
    std::string generateForLoop(const std::string& initCode, const std::string& conditionCode, const std::string& updateCode, const std::string& bodyCode);
    std::string generatePrintStatement(const std::string& expressionCode); // Modificada para manejar múltiples tipos
    std::string generateBreakStatement();
    std::string generateContinueStatement();

    // Nuevos métodos para la gestión de la visualización del stack
    std::string generateFunctionEntry(const std::string& functionName, const std::vector<std::pair<std::string, std::string>>& params); // Añadido vector de params
    std::string generateFunctionExit(const std::string& functionName, const std::string& returnValueCode);
    std::string generateVariableUpdate(const std::string& variableName, const std::string& valueCode);
    std::string generateScopeEnter(); // Para bloques genéricos {}
    std::string generateScopeExit();  // Para bloques genéricos {}

    // Métodos de utilidad para indentación
    void increaseIndent();
    void decreaseIndent();
    std::string getCurrentIndent() const;

private:
    int indentLevel;
    // std::map<std::string, std::string> variableStates; // No se usará para el estado en tiempo de ejecución del C++ generado
};

#endif // SFMLTRANSLATOR_H