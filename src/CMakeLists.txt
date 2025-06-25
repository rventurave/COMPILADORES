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

# Enlazar SFML
target_link_libraries(C_SFML_Compiler PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
)

# Copiar DLLs automáticamente al lado del ejecutable tras compilar (para Windows)
add_custom_command(TARGET C_SFML_Compiler POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/SFML-2.6.1/bin/sfml-graphics-2.dll"
        "C:/SFML-2.6.1/bin/sfml-window-2.dll"
        "C:/SFML-2.6.1/bin/sfml-system-2.dll"
        "C:/SFML-2.6.1/bin/sfml-audio-2.dll"
        $<TARGET_FILE_DIR:C_SFML_Compiler>
)
