#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "semantic_analyzer/SemanticAnalyzer.h"
#include "code_generator/CodeGenerator.h"
#include "code_generator/SFMLTranslator.h"
#include "utils/ErrorHandler.h"

int main(int argc, char* argv[]) {
    std::cout << "Iniciando el Compilador C a SFML..." << std::endl;

    std::string inputFilePath = "../../input.txt";// Asume que input.txt está en el directorio raíz del proyecto

    // 1. Leer el código fuente
    std::ifstream inputFile(inputFilePath);
    if (!inputFile.is_open()) {
        ErrorHandler::reportError("Error: No se pudo abrir el archivo de entrada: " + inputFilePath, -1, -1);
        return 1;
    }
    std::stringstream buffer;
    buffer << inputFile.rdbuf();
    std::string sourceCode = buffer.str();
    inputFile.close();

    std::cout << "--- Código Fuente Cargado ---" << std::endl;
    std::cout << sourceCode << std::endl;
    std::cout << "-----------------------------" << std::endl;

    // 2. Fase de Análisis Léxico (Lexer)
    Lexer lexer(sourceCode);
    std::vector<Token> tokens = lexer.tokenize();

    std::cout << "\n--- Tokens Generados ---" << std::endl;
    for (const auto& token : tokens) {
        std::cout << "Tipo: " << static_cast<int>(token.type)
                  << ", Valor: '" << token.value << "'"
                  << ", Línea: " << token.line
                  << ", Columna: " << token.column << std::endl;
    }
    std::cout << "------------------------" << std::endl;

    // 3. Fase de Análisis Sintáctico (Parser)
    Parser parser(tokens);
    std::unique_ptr<ASTNode> rootNode = parser.parse();

    if (!rootNode) {
        ErrorHandler::reportError("Error: Fallo en el análisis sintáctico.", -1, -1);
        return 1;
    }
    std::cout << "\n--- Árbol de Sintaxis Abstracta (AST) Generado ---" << std::endl;
    // Aquí podrías implementar una función para imprimir el AST
    // Por ahora, solo confirmamos que se generó
    std::cout << "AST raíz creado." << std::endl;
    std::cout << "------------------------------------------------" << std::endl;

    // 4. Fase de Análisis Semántico
    SymbolTable symbolTable;
    SemanticAnalyzer semanticAnalyzer(symbolTable);
    if (!semanticAnalyzer.analyze(rootNode.get())) {
        ErrorHandler::reportError("Error: Fallo en el análisis semántico.", -1, -1);
        return 1;
    }
    std::cout << "\n--- Análisis Semántico Completado ---" << std::endl;
    std::cout << "El código es semánticamente válido." << std::endl;
    std::cout << "-------------------------------------" << std::endl;

    // 5. Fase de Generación de Código
    SFMLTranslator sfmlTranslator;
    CodeGenerator codeGenerator(sfmlTranslator);
    std::string sfmlCode = codeGenerator.generate(rootNode.get());

    if (sfmlCode.empty()) {
        ErrorHandler::reportError("Error: Fallo en la generación de código SFML.", -1, -1);
        return 1;
    }
    std::cout << "\n--- Código SFML Generado ---" << std::endl;
    std::cout << sfmlCode << std::endl;
    std::cout << "----------------------------" << std::endl;

    // Puedes guardar el código SFML generado en un archivo si lo deseas
    std::ofstream outputFile("output_sfml.cpp");
    if (outputFile.is_open()) {
        outputFile << sfmlCode;
        outputFile.close();
        std::cout << "\nCódigo SFML guardado en output_sfml.cpp" << std::endl;
    } else {
        ErrorHandler::reportWarning("Advertencia: No se pudo guardar el código SFML generado en output_sfml.cpp");
    }

    std::cout << "\nCompilador C a SFML finalizado." << std::endl;

    return 0;
}