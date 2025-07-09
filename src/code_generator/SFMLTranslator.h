#ifndef SFMLTRANSLATOR_H
#define SFMLTRANSLATOR_H

#include <string>
#include <vector>
#include <map>
#include <sstream>  // Para std::stringstream
#include <utility> // Para std::pair

class SFMLTranslator {
public:
    SFMLTranslator();

    void increaseIndent();
    void decreaseIndent();
    std::string getCurrentIndent() const;

    // Partes de generación de código SFML
    std::string getSFMLHeader();
    std::string getSFMLFooter();

    // Pasos de visualización específicos
    std::string generateProgramStart();
    std::string generateProgramEnd();
    std::string generateFunctionDeclaration(const std::string& returnType, const std::string& functionName, const std::string& paramsCode, const std::string& bodyCode);
    std::string generateFunctionCall(const std::string& functionName, const std::string& argsCode);
    std::string generateVariableDeclaration(const std::string& typeName, const std::string& variableName, const std::string& initialValue);
    std::string generateAssignment(const std::string& identifierName, const std::string& expressionCode);
    std::string generateReturnStatement(const std::string& expressionCode, const std::string& functionName);
    std::string generateIfStatement(const std::string& conditionCode, const std::string& thenBlockCode, const std::string& elseBlockCode);
    std::string generateForLoop(const std::string& initCode, const std::string& conditionCode, const std::string& updateCode, const std::string& bodyCode);
    std::string generatePrintStatement(const std::string& expressionCode);
    std::string generateBreakStatement();
    std::string generateContinueStatement();

    // Manipulación de pila y heap para visualización
    std::string generateFunctionEntry(const std::string& functionName, const std::vector<std::pair<std::string, std::string>>& params);
    std::string generateFunctionExit(const std::string& functionName, const std::string& returnValueCode);
    std::string generateVariableUpdate(const std::string& variableName, const std::string& valueCode);
    std::string generateScopeEnter();
    std::string generateScopeExit();

private:
    int indentLevel;
};

#endif // SFMLTRANSLATOR_H