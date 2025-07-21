# No incluir cmake_minimum_required, project, ni find_package aquí (ya están en el principal).

# Ejecutable principal
add_executable(C_SFML_Compiler
    main.cpp
    lexer/Lexer.cpp
    lexer/Token.cpp
    parser/AST.cpp
    parser/Parser.cpp
    semantic_analyzer/SemanticAnalyzer.cpp
    semantic_analyzer/SymbolTable.cpp 
    code_generator/CodeGenerator.cpp
    code_generator/SFMLTranslator.cpp
    utils/ErrorHandler.cpp
)

# Directorios de cabeceras
target_include_directories(C_SFML_Compiler PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/lexer
    ${CMAKE_CURRENT_SOURCE_DIR}/parser
    ${CMAKE_CURRENT_SOURCE_DIR}/semantic_analyzer
    ${CMAKE_CURRENT_SOURCE_DIR}/code_generator
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
)

# Enlazar SFML (uso de targets ya encontrados en el padre)
target_link_libraries(C_SFML_Compiler PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
)

# --- No es necesario copiar DLLs en Linux ---
# En Fedora y otros Linux, las bibliotecas SFML compartidas (*.so) se buscarán en rutas estándar (ya las maneja el sistema).
# Si quieres instalar el ejecutable y que las dependencias se gestionen mejor, usa después:
# sudo dnf install SFML
# o instala las bibliotecas necesarias.
