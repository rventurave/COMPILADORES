// src/parser/Parser.cpp
#include "Parser.h"
#include <iostream> // Para depuración, si es necesario
#include <utility> // Para std::move en constructores

// Constructor - Ahora recibe ErrorHandler
Parser::Parser(const std::vector<Token>& tokens, ErrorHandler& errorHandler)
    : tokens(tokens), currentTokenIndex(0), errorHandler(errorHandler) {}

// Mira el token en la posición actual + offset sin avanzar
Token Parser::peek(int offset) {
    if (currentTokenIndex + offset >= tokens.size()) {
        return Token(TokenType::END_OF_FILE, "", 0, 0); // Token de fin de archivo
    }
    return tokens[currentTokenIndex + offset];
}

// Consume el token actual y avanza al siguiente
Token Parser::consume() {
    if (!isAtEnd()) {
        return tokens[currentTokenIndex++];
    }
    return Token(TokenType::END_OF_FILE, "", 0, 0); // Error o final
}

// Consume el token actual si su tipo coincide con 'type'
bool Parser::match(TokenType type) {
    if (peek().type == type) {
        consume();
        return true;
    }
    return false;
}

// Espera un token de un tipo específico, si no lo encuentra, reporta un error.
Token Parser::expect(TokenType type, const std::string& errorMessage) {
    if (peek().type == type) {
        return consume();
    }
    // CORRECCIÓN AQUÍ: Orden de argumentos (mensaje, línea, columna)
    errorHandler.reportError(errorMessage, peek().line, peek().column);
    // Para recuperación de errores, se podría avanzar o insertar un token fantasma
    // Por simplicidad, por ahora simplemente devolvemos un token de error.
    return Token(TokenType::UNKNOWN, "", peek().line, peek().column);
}

// Verifica si se ha llegado al final de los tokens
bool Parser::isAtEnd() {
    return peek().type == TokenType::END_OF_FILE;
}

// -------------------------------------------------------------------------------------------------
// Métodos de Parseo
// -------------------------------------------------------------------------------------------------

// Método principal para iniciar el análisis y construir el AST.
std::unique_ptr<ASTNode> Parser::parse() {
    return parseProgram();
}

// <program> ::= { <functionDeclaration> | <declarationStatement> }* EOF
std::unique_ptr<ASTNode> Parser::parseProgram() {
    auto programNode = std::make_unique<ProgramNode>();

    while (!isAtEnd() && peek().type != TokenType::END_OF_FILE) {
        // Asumimos que una declaración de tipo seguida por un identificador
        // es el inicio de una declaración de función o de variable global.
        if (peek().type == TokenType::KEYWORD_INT || peek().type == TokenType::KEYWORD_VOID) {
            // Lookahead para distinguir entre declaración de función y de variable global
            if (peek(1).type == TokenType::IDENTIFIER) {
                if (peek(2).type == TokenType::LPAREN) { // Parece una función (tipo ID LPAREN)
                    programNode->functionDeclarations.push_back(parseFunctionDeclaration());
                } else { // Asumimos declaración de variable global
                    programNode->statements.push_back(parseDeclarationStatement());
                }
            } else {
                // CORRECCIÓN AQUÍ: Orden de argumentos
                errorHandler.reportError("Identificador esperado después del tipo.", peek().line, peek().column);
                consume(); // Intentar recuperarse avanzando
            }
        } else {
            // CORRECCIÓN AQUÍ: Orden de argumentos
            errorHandler.reportError("Declaración de función o variable global esperada.", peek().line, peek().column);
            consume(); // Intentar recuperarse
        }
    }
    return programNode;
}

// <functionDeclaration> ::= ( "int" | "void" ) IDENTIFIER "(" [ <parameterList> ] ")" <blockStatement>
std::unique_ptr<ASTNode> Parser::parseFunctionDeclaration() {
    Token returnType = expect(TokenType::KEYWORD_INT, "Se esperaba un tipo de retorno (int o void).");
    if (returnType.type == TokenType::UNKNOWN) return nullptr; // Error de recuperación

    Token functionName = expect(TokenType::IDENTIFIER, "Se esperaba un nombre de función.");
    if (functionName.type == TokenType::UNKNOWN) return nullptr;

    // Inicializar funcDecl con parámetros vacíos y cuerpo nulo por ahora
    auto funcDecl = std::make_unique<FunctionDeclarationNode>(functionName.value, returnType.value, std::vector<std::pair<std::string, std::string>>{}, nullptr);

    expect(TokenType::LPAREN, "Se esperaba '(' después del nombre de la función.");
    // Aquí iría el parseo de parámetros
    while (peek().type != TokenType::RPAREN && peek().type != TokenType::END_OF_FILE) {
        if (peek().type == TokenType::KEYWORD_INT || peek().type == TokenType::KEYWORD_VOID) { // Permite void también para parámetros
            Token paramType = consume();
            Token paramName = expect(TokenType::IDENTIFIER, "Se esperaba un nombre de parámetro.");
            if (paramName.type != TokenType::UNKNOWN) {
                funcDecl->parameters.push_back({paramType.value, paramName.value});
            }
            if (peek().type == TokenType::COMMA) {
                consume(); // Consumir la coma
            } else if (peek().type != TokenType::RPAREN) {
                // CORRECCIÓN AQUÍ: Orden de argumentos
                errorHandler.reportError("Se esperaba ',' o ')' después del parámetro.", peek().line, peek().column);
                if (peek().type != TokenType::RPAREN) consume();
            }
        } else {
            // CORRECCIÓN AQUÍ: Orden de argumentos
            errorHandler.reportError("Tipo de parámetro esperado (ej. 'int').", peek().line, peek().column);
            if (peek().type != TokenType::RPAREN) {
                 consume();
            }
        }
    }
    expect(TokenType::RPAREN, "Se esperaba ')' después de la lista de parámetros.");

    // Parsear el cuerpo de la función (bloque de sentencias) y asignarlo al miembro 'body'
    funcDecl->body = parseBlockStatement();
    if (!funcDecl->body) return nullptr; // Si el bloque es nulo, hubo un error

    return funcDecl;
}

// <blockStatement> ::= "{" { <statement> | <declarationStatement> }* "}"
std::unique_ptr<ASTNode> Parser::parseBlockStatement() {
    expect(TokenType::LBRACE, "Se esperaba '{' para el bloque de código.");
    if (peek().type == TokenType::UNKNOWN) return nullptr; // Error de recuperación

    std::vector<std::unique_ptr<ASTNode>> statementsInBlock;

    while (peek().type != TokenType::RBRACE && !isAtEnd()) {
        if (peek().type == TokenType::KEYWORD_INT || peek().type == TokenType::KEYWORD_VOID) { // Declaración de variable local
            statementsInBlock.push_back(parseDeclarationStatement());
        } else { // Otra sentencia
            statementsInBlock.push_back(parseStatement());
        }
        if (!statementsInBlock.back()) { // Si el parseo de la sentencia falló
            // Intentar avanzar para recuperarse del error y no entrar en bucle infinito
            if (!isAtEnd() && peek().type != TokenType::RBRACE) {
                consume(); // Solo consumir si no es RBRACE o EOF para evitar bucle
            } else {
                break; // Salir si estamos al final o encontramos RBRACE
            }
        }
    }

    expect(TokenType::RBRACE, "Se esperaba '}' para cerrar el bloque de código.");
    return std::make_unique<BlockStatementNode>(std::move(statementsInBlock));
}

// <statement> ::= <assignmentStatement> ";"
//               | <ifStatement>
//               | <forStatement>
//               | <returnStatement> ";"
//               | <printStatement> ";"
//               | <functionCall> ";"
//               | <blockStatement>
std::unique_ptr<ASTNode> Parser::parseStatement() {
    // Si la sentencia actual es un bloque, llamarlo directamente
    if (peek().type == TokenType::LBRACE) {
        return parseBlockStatement();
    }

    std::unique_ptr<ASTNode> stmt = nullptr;
    TokenType currentType = peek().type;
    TokenType nextType = peek(1).type;

    if (currentType == TokenType::IDENTIFIER) {
        if (nextType == TokenType::ASSIGN) { // Asignación
            stmt = parseAssignmentStatement();
        } else if (nextType == TokenType::LPAREN) { // Llamada a función
            stmt = parseFunctionCall();
        }
    } else if (currentType == TokenType::KEYWORD_IF) {
        stmt = parseIfStatement();
    } else if (currentType == TokenType::KEYWORD_FOR) {
        stmt = parseForStatement();
    } else if (currentType == TokenType::KEYWORD_RETURN) {
        stmt = parseReturnStatement();
    } else if (currentType == TokenType::KEYWORD_PRINTF) {
        stmt = parsePrintStatement();
    }

    if (stmt) {
        // Las sentencias como if, for, y bloques no terminan con ';'.
        // Solo las sentencias de expresión (asignación, llamada a función, return, print) lo hacen.
        if (stmt->type == ASTNodeType::AssignmentStatement ||
            stmt->type == ASTNodeType::ReturnStatement ||
            stmt->type == ASTNodeType::FunctionCall ||
            stmt->type == ASTNodeType::VariableDeclaration || // Si la declaración se maneja aquí
            stmt->type == ASTNodeType::PrintStatement) {
            expect(TokenType::SEMICOLON, "Se esperaba ';' después de la sentencia.");
        }
        return stmt;
    }

    // CORRECCIÓN AQUÍ: Orden de argumentos
    errorHandler.reportError("Sentencia inválida o incompleta o token inesperado.", peek().line, peek().column);
    consume(); // Intenta recuperarse
    return nullptr;
}

// <declarationStatement> ::= "int" IDENTIFIER [ "=" <expression> ] ";"
std::unique_ptr<ASTNode> Parser::parseDeclarationStatement() {
    Token typeToken = expect(TokenType::KEYWORD_INT, "Se esperaba el tipo 'int' para la declaración de variable.");
    if (typeToken.type == TokenType::UNKNOWN) return nullptr;

    // NUEVO: Verificar si el siguiente token es '*'
    std::string typeName = typeToken.value;
    if (peek().type == TokenType::MULTIPLY) {
        consume(); // Consume '*'
        typeName += "*";
    }

    Token varName = expect(TokenType::IDENTIFIER, "Se esperaba un nombre de variable.");
    if (varName.type == TokenType::UNKNOWN) return nullptr;

    std::unique_ptr<ASTNode> initializer = nullptr;
    if (match(TokenType::ASSIGN)) {
        initializer = parseExpression();
    }

    expect(TokenType::SEMICOLON, "Se esperaba ';' después de la declaración de variable.");
    return std::make_unique<VariableDeclarationNode>(typeName, varName.value, std::move(initializer));
}

// <assignmentStatement> ::= IDENTIFIER "=" <expression>
std::unique_ptr<ASTNode> Parser::parseAssignmentStatement() {
    Token identifier = expect(TokenType::IDENTIFIER, "Se esperaba un identificador para la asignación.");
    if (identifier.type == TokenType::UNKNOWN) return nullptr;

    expect(TokenType::ASSIGN, "Se esperaba '=' para la asignación.");
    if (peek().type == TokenType::UNKNOWN) return nullptr;

    auto expr = parseExpression();
    if (!expr) return nullptr;

    return std::make_unique<AssignmentStatementNode>(identifier.value, std::move(expr));
}

// <ifStatement> ::= "if" "(" <expression> ")" <statement> [ "else" <statement> ]
std::unique_ptr<ASTNode> Parser::parseIfStatement() {
    expect(TokenType::KEYWORD_IF, "Se esperaba 'if'.");
    if (peek().type == TokenType::UNKNOWN) return nullptr;

    expect(TokenType::LPAREN, "Se esperaba '(' después de 'if'.");
    if (peek().type == TokenType::UNKNOWN) return nullptr;

    auto condition = parseExpression();
    if (!condition) return nullptr;

    expect(TokenType::RPAREN, "Se esperaba ')' después de la condición del 'if'.");
    if (peek().type == TokenType::UNKNOWN) return nullptr;

    auto thenBlock = parseStatement(); // Puede ser un solo stmt o un block
    if (!thenBlock) return nullptr;

    std::unique_ptr<ASTNode> elseBlock = nullptr;
    if (match(TokenType::KEYWORD_ELSE)) {
        elseBlock = parseStatement();
    }

    return std::make_unique<IfStatementNode>(std::move(condition), std::move(thenBlock), std::move(elseBlock));
}

// <forStatement> ::= "for" "(" ( <declarationStatement> | <assignmentStatement> | ";" ) <expression> ";" <assignmentStatement> ")" <statement>
std::unique_ptr<ASTNode> Parser::parseForStatement() {
    expect(TokenType::KEYWORD_FOR, "Se esperaba 'for'.");
    if (peek().type == TokenType::UNKNOWN) return nullptr;

    expect(TokenType::LPAREN, "Se esperaba '(' después de 'for'.");
    if (peek().type == TokenType::UNKNOWN) return nullptr;

    // Inicialización del bucle for
    std::unique_ptr<ASTNode> initialization = nullptr;
    if (peek().type == TokenType::KEYWORD_INT || peek().type == TokenType::KEYWORD_VOID) { // Declaración
        initialization = parseDeclarationStatement(); // consume el ';'
    } else if (peek().type == TokenType::IDENTIFIER && peek(1).type == TokenType::ASSIGN) { // Asignación
        initialization = parseAssignmentStatement();
        expect(TokenType::SEMICOLON, "Se esperaba ';' después de la inicialización en for.");
    } else { // Puede estar vacío, solo consumir ';'
        expect(TokenType::SEMICOLON, "Se esperaba ';' después de la inicialización en for.");
    }

    // Condición del bucle for
    std::unique_ptr<ASTNode> condition = nullptr;
    if (peek().type != TokenType::SEMICOLON) {
        condition = parseExpression();
    }
    expect(TokenType::SEMICOLON, "Se esperaba ';' después de la condición en for.");

    // Incremento del bucle for
    std::unique_ptr<ASTNode> increment = nullptr;
    if (peek().type != TokenType::RPAREN) {
        // En for, el incremento puede ser una asignación o llamada a función
        if (peek().type == TokenType::IDENTIFIER && peek(1).type == TokenType::ASSIGN) {
            increment = parseAssignmentStatement();
        } else if (peek().type == TokenType::IDENTIFIER && peek(1).type == TokenType::LPAREN) {
            increment = parseFunctionCall(); // Asume que llamadas a función también pueden ser incrementos
        } else {
            // CORRECCIÓN AQUÍ: Orden de argumentos
            errorHandler.reportError("Expresión de incremento esperada en for.", peek().line, peek().column);
        }
    }
    expect(TokenType::RPAREN, "Se esperaba ')' después del incremento en for.");

    auto body = parseStatement();
    if (!body) return nullptr;

    return std::make_unique<ForStatementNode>(std::move(initialization), std::move(condition), std::move(increment), std::move(body));
}

// <returnStatement> ::= "return" [ <expression> ] ";"
std::unique_ptr<ASTNode> Parser::parseReturnStatement() {
    expect(TokenType::KEYWORD_RETURN, "Se esperaba 'return'.");
    if (peek().type == TokenType::UNKNOWN) return nullptr;

    std::unique_ptr<ASTNode> expr = nullptr;
    // Si el siguiente token no es un ';' o '}', significa que hay una expresión de retorno
    if (peek().type != TokenType::SEMICOLON && peek().type != TokenType::RBRACE) {
        expr = parseExpression();
    }
    return std::make_unique<ReturnStatementNode>(std::move(expr));
}

// <printStatement> ::= "printf" "(" STRING_LITERAL { "," <expression> }* ")"
std::unique_ptr<ASTNode> Parser::parsePrintStatement() {
    expect(TokenType::KEYWORD_PRINTF, "Se esperaba 'printf'.");
    if (peek().type == TokenType::UNKNOWN) return nullptr;

    expect(TokenType::LPAREN, "Se esperaba '(' después de 'printf'.");
    if (peek().type == TokenType::UNKNOWN) return nullptr;

    Token formatStringToken = expect(TokenType::STRING_LITERAL, "Se esperaba una cadena de formato para printf.");
    if (formatStringToken.type == TokenType::UNKNOWN) return nullptr;

    std::vector<std::unique_ptr<ASTNode>> printArgs;
    // Si hay una coma después de la cadena de formato, entonces hay argumentos.
    if (peek().type == TokenType::COMMA) {
        consume(); // Consumir la primera coma
        while (peek().type != TokenType::RPAREN && peek().type != TokenType::END_OF_FILE) {
            auto arg = parseExpression();
            if (arg) {
                printArgs.push_back(std::move(arg));
            } else {
                // CORRECCIÓN AQUÍ: Orden de argumentos
                errorHandler.reportError("Expresión de argumento esperada en printf.", peek().line, peek().column);
                if (peek().type != TokenType::RPAREN) consume();
            }
            if (peek().type == TokenType::COMMA) {
                consume();
            } else {
                break; // No más comas, salir del bucle de argumentos
            }
        }
    }

    expect(TokenType::RPAREN, "Se esperaba ')' después de los argumentos de printf.");
    return std::make_unique<PrintStatementNode>(formatStringToken.value, std::move(printArgs));
}

// <functionCall> ::= IDENTIFIER "(" [ <argumentList> ] ")"
std::unique_ptr<ASTNode> Parser::parseFunctionCall() {
    Token funcName = expect(TokenType::IDENTIFIER, "Se esperaba un nombre de función para la llamada.");
    if (funcName.type == TokenType::UNKNOWN) return nullptr;

    std::vector<std::unique_ptr<ASTNode>> arguments;

    expect(TokenType::LPAREN, "Se esperaba '(' para la llamada a función.");
    if (peek().type == TokenType::UNKNOWN) return nullptr;

    // Parsear argumentos
    while (peek().type != TokenType::RPAREN && peek().type != TokenType::END_OF_FILE) {
        auto arg = parseExpression();
        if (arg) {
            arguments.push_back(std::move(arg));
        } else {
            // CORRECCIÓN AQUÍ: Orden de argumentos
            errorHandler.reportError("Expresión de argumento esperada en la llamada a función.", peek().line, peek().column);
            if (peek().type != TokenType::RPAREN) consume();
        }
        if (peek().type == TokenType::COMMA) {
            consume();
        } else if (peek().type != TokenType::RPAREN) {
            // CORRECCIÓN AQUÍ: Orden de argumentos
            errorHandler.reportError("Se esperaba ',' o ')' después de los argumentos de la función.", peek().line, peek().column);
            if (peek().type != TokenType::RPAREN) consume();
        }
    }

    expect(TokenType::RPAREN, "Se esperaba ')' para cerrar la llamada a función.");
    return std::make_unique<FunctionCallNode>(funcName.value, std::move(arguments));
}

// <expression> ::= <equalityExpression>
std::unique_ptr<ASTNode> Parser::parseExpression() {
    return parseEqualityExpression();
}

// <equalityExpression> ::= <comparisonExpression> ( ( "==" | "!=" ) <comparisonExpression> )*
std::unique_ptr<ASTNode> Parser::parseEqualityExpression() {
    auto expr = parseComparisonExpression();
    if (!expr) return nullptr; // Manejar el caso de expresión nula

    while (peek().type == TokenType::EQUAL_EQUAL || peek().type == TokenType::NOT_EQUAL) {
        Token op = consume();
        auto right = parseComparisonExpression();
        if (!right) {
            // CORRECCIÓN AQUÍ: Orden de argumentos
            errorHandler.reportError("Expresión derecha esperada para operador de igualdad.", peek().line, peek().column);
            return nullptr;
        }
        expr = std::make_unique<BinaryExpressionNode>(std::move(expr), std::move(right), op.value);
    }
    return expr;
}

// <comparisonExpression> ::= <additiveExpression> ( ( ">" | ">=" | "<" | "<=" ) <additiveExpression> )*
std::unique_ptr<ASTNode> Parser::parseComparisonExpression() {
    auto expr = parseAdditiveExpression();
    if (!expr) return nullptr;

    while (peek().type == TokenType::GREATER_THAN || peek().type == TokenType::GREATER_EQUAL ||
           peek().type == TokenType::LESS_THAN || peek().type == TokenType::LESS_EQUAL) {
        Token op = consume();
        auto right = parseAdditiveExpression();
        if (!right) {
            // CORRECCIÓN AQUÍ: Orden de argumentos
            errorHandler.reportError("Expresión derecha esperada para operador de comparación.", peek().line, peek().column);
            return nullptr;
        }
        expr = std::make_unique<BinaryExpressionNode>(std::move(expr), std::move(right), op.value);
    }
    return expr;
}

// <additiveExpression> ::= <multiplicativeExpression> ( ( "+" | "-" ) <multiplicativeExpression> )*
std::unique_ptr<ASTNode> Parser::parseAdditiveExpression() {
    auto expr = parseMultiplicativeExpression();
    if (!expr) return nullptr;

    while (peek().type == TokenType::PLUS || peek().type == TokenType::MINUS) {
        Token op = consume();
        auto right = parseMultiplicativeExpression();
        if (!right) {
            // CORRECCIÓN AQUÍ: Orden de argumentos
            errorHandler.reportError("Expresión derecha esperada para operador aditivo.", peek().line, peek().column);
            return nullptr;
        }
        expr = std::make_unique<BinaryExpressionNode>(std::move(expr), std::move(right), op.value);
    }
    return expr;
}

// <multiplicativeExpression> ::= <primaryExpression> ( ( "*" | "/" ) <primaryExpression> )*
std::unique_ptr<ASTNode> Parser::parseMultiplicativeExpression() {
    auto expr = parsePrimaryExpression();
    if (!expr) return nullptr;

    while (peek().type == TokenType::MULTIPLY || peek().type == TokenType::DIVIDE) {
        Token op = consume();
        auto right = parsePrimaryExpression();
        if (!right) {
            // CORRECCIÓN AQUÍ: Orden de argumentos
            errorHandler.reportError("Expresión derecha esperada para operador multiplicativo.", peek().line, peek().column);
            return nullptr;
        }
        expr = std::make_unique<BinaryExpressionNode>(std::move(expr), std::move(right), op.value);
    }
    return expr;
}

// <primaryExpression> ::= INTEGER_LITERAL
//                       | STRING_LITERAL
//                       | IDENTIFIER
//                       | "(" <expression> ")"
//                       | <functionCall>
//                       | "-" <primaryExpression> (para negación unaria)
std::unique_ptr<ASTNode> Parser::parsePrimaryExpression() {
    switch (peek().type) {
        case TokenType::INTEGER_LITERAL:
            return std::make_unique<LiteralNode>(consume().value);
        case TokenType::STRING_LITERAL:
            return std::make_unique<LiteralNode>(consume().value);
        case TokenType::IDENTIFIER:
            // Si el identificador es seguido por '(', es una llamada a función
            if (peek(1).type == TokenType::LPAREN) {
                return parseFunctionCall();
            }
            return std::make_unique<IdentifierNode>(consume().value);
        case TokenType::LPAREN: {
            consume(); // Consume '('
            auto expr = parseExpression();
            if (!expr) {
                // CORRECCIÓN AQUÍ: Orden de argumentos
                errorHandler.reportError("Expresión esperada dentro de paréntesis.", peek().line, peek().column);
                return nullptr;
            }
            expect(TokenType::RPAREN, "Se esperaba ')' después de la expresión entre paréntesis.");
            return expr;
        }
        case TokenType::MINUS: { // Para manejar negación unaria (ej. -5)
            Token op = consume(); // consume el '-'
            auto operand = parsePrimaryExpression(); // El operando de la negación
            if (!operand) {
                // CORRECCIÓN AQUÍ: Orden de argumentos
                errorHandler.reportError("Operando esperado para operador unario '-'.", peek().line, peek().column);
                return nullptr;
            }
            return std::make_unique<UnaryExpressionNode>(op.value, std::move(operand));
        }
        case TokenType::MULTIPLY: { // Desreferenciación de puntero: *p
            Token op = consume(); // consume '*'
            auto operand = parsePrimaryExpression();
            if (!operand) {
                errorHandler.reportError("Operando esperado para operador unario '*'.", peek().line, peek().column);
                return nullptr;
            }
            return std::make_unique<UnaryExpressionNode>(op.value, std::move(operand));
        }
        case TokenType::AMPERSAND: { // NUEVO: operador '&'
            Token op = consume(); // consume '&'
            auto operand = parsePrimaryExpression();
            if (!operand) {
                errorHandler.reportError("Operando esperado para operador unario '&'.", peek().line, peek().column);
                return nullptr;
            }
            return std::make_unique<UnaryExpressionNode>(op.value, std::move(operand));
        }
        default:
            // CORRECCIÓN AQUÍ: Orden de argumentos
            errorHandler.reportError("Expresión primaria inesperada.", peek().line, peek().column);
            consume(); // Intenta recuperarse
            return nullptr;
    }
}