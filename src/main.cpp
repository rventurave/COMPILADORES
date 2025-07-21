// src/main.cpp
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <memory> // For std::unique_ptr
#include <vector> // Necesario para std::vector<Token>

#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "semantic_analyzer/SemanticAnalyzer.h"
#include "code_generator/CodeGenerator.h"
#include "utils/ErrorHandler.h" // Assuming ErrorHandler is used

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file.c>" << std::endl;
        return 1;
    }

    std::string inputFileName = argv[1];
    std::ifstream inputFile(inputFileName);

    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open input file '" << inputFileName << "'" << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << inputFile.rdbuf();
    std::string sourceCode = buffer.str();

    ErrorHandler errorHandler; // Create an error handler instance

    // 1. Lexical Analysis
    Lexer lexer(sourceCode, errorHandler); // Pasa errorHandler al lexer
    std::vector<Token> tokens = lexer.tokenize();
    // --- DEBUG: Imprimir tokens léxicos ---
    std::cout << "\n=== TOKENS GENERADOS ===" << std::endl;
    for (const auto& token : tokens) {
        std::cout << "Token: '" << token.value 
                << "' | Tipo: " << static_cast<int>(token.type)
                << " | Línea: " << token.line
                << " Col: " << token.column << std::endl;
    }
    std::cout << "=========================\n" << std::endl;

    if (errorHandler.hasErrors()) {
        errorHandler.printMessages();
        return 1;
    }

    // 2. Syntactic Analysis (Parsing)
    Parser parser(tokens, errorHandler); // Pasa errorHandler al parser
    std::unique_ptr<ASTNode> programAST = parser.parse();

    if (errorHandler.hasErrors()) {
        errorHandler.printMessages();
        return 1;
    }

    ProgramNode* programNode = dynamic_cast<ProgramNode*>(programAST.get());

    if (!programNode) {
        errorHandler.reportError("Error interno: El parser no devolvió un ProgramNode válido como raíz del AST.", 0, 0);
        errorHandler.printMessages();
        return 1;
    }

    // 3. Semantic Analysis
    SemanticAnalyzer semanticAnalyzer(errorHandler); // Pasa errorHandler al analizador semántico
    semanticAnalyzer.analyze(programNode); // Pasa el ProgramNode*

    if (errorHandler.hasErrors()) {
        errorHandler.printMessages();
        return 1;
    }

    // --- CORRECCIÓN AQUÍ ---
    // Pasa la instancia de errorHandler al constructor de CodeGenerator
    CodeGenerator codeGenerator(errorHandler); // <--- ¡CAMBIO AQUÍ!
    // --- FIN CORRECCIÓN ---

    std::string generatedSFMLCode = codeGenerator.generate(programNode); // Pasa el ProgramNode*

    // Guarda el código C++ SFML generado en un archivo
    std::ofstream outputFile("output_sfml.cpp");
    if (outputFile.is_open()) {
        outputFile << generatedSFMLCode;
        outputFile.close();
        std::cout << "Generated SFML code saved to output_sfml.cpp" << std::endl;
        std::cout << "Compile and run output_sfml.cpp with SFML libraries: " << std::endl;
        std::cout << "g++ output_sfml.cpp -o output_sfml -lsfml-graphics -lsfml-window -lsfml-system" << std::endl;
    } else {
        std::cerr << "Error: Could not open output_sfml.cpp for writing." << std::endl;
        return 1;
    }

    return 0;
}