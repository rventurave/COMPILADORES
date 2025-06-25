#include "Parser.h"

// Constructor
Parser::Parser(const std::vector<Token>& tokens)
    : tokens(tokens), currentTokenIndex(0) {}

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
    ErrorHandler::reportError(errorMessage, peek().line, peek().column);
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
            Token typeToken = peek();
            if (peek(1).type == TokenType::IDENTIFIER) {
                // Verificar si es una declaración de función o variable
                // Esto es una simplificación, en un compilador completo se necesitaría más "lookahead"
                if (peek(2).type == TokenType::LPAREN) { // Parece una función (tipo ID LPAREN)
                    programNode->children.push_back(parseFunctionDeclaration());
                } else { // Asumimos declaración de variable global
                    programNode->children.push_back(parseDeclarationStatement());
                }
            } else {
                ErrorHandler::reportError("Identificador esperado después del tipo.", typeToken.line, typeToken.column);
                consume(); // Intentar recuperarse avanzando
            }
        } else {
            ErrorHandler::reportError("Declaración de función o variable global esperada.", peek().line, peek().column);
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

    auto funcDecl = std::make_unique<FunctionDeclarationNode>(functionName.value, returnType.value);

    expect(TokenType::LPAREN, "Se esperaba '(' después del nombre de la función.");
    // Aquí iría el parseo de parámetros, por ahora lo simplificamos
    while (peek().type != TokenType::RPAREN && peek().type != TokenType::END_OF_FILE) {
        // Ejemplo simple de parseo de parámetros: int a, int b
        // Se esperaría un tipo y luego un identificador
        if (peek().type == TokenType::KEYWORD_INT) {
            Token paramType = consume(); // int
            Token paramName = expect(TokenType::IDENTIFIER, "Se esperaba un nombre de parámetro.");
            if (paramName.type != TokenType::UNKNOWN) {
                funcDecl->parameters.push_back({paramType.value, paramName.value});
            }
            if (peek().type == TokenType::COMMA) {
                consume(); // Consumir la coma
            } else if (peek().type != TokenType::RPAREN) {
                ErrorHandler::reportError("Se esperaba ',' o ')' después del parámetro.", peek().line, peek().column);
                // Recuperación de error: avanzar para evitar bucle infinito
                if (peek().type != TokenType::RPAREN) consume();
            }
        } else {
            // Si el token no es un tipo de parámetro esperado, asumimos que no hay más parámetros válidos
            // y reportamos un error o intentamos consumir para no quedarnos atascados.
            ErrorHandler::reportError("Tipo de parámetro esperado (ej. 'int').", peek().line, peek().column);
            // Intentar avanzar para no entrar en bucle infinito
            if (peek().type != TokenType::RPAREN) {
                 consume();
            }
        }
    }
    expect(TokenType::RPAREN, "Se esperaba ')' después de la lista de parámetros.");

    // Parsear el cuerpo de la función (bloque de sentencias)
    funcDecl->children.push_back(parseBlockStatement());
    if (!funcDecl->children.back()) return nullptr; // Si el bloque es nulo, hubo un error

    return funcDecl;
}

// <blockStatement> ::= "{" { <statement> | <declarationStatement> }* "}"
std::unique_ptr<ASTNode> Parser::parseBlockStatement() {
    expect(TokenType::LBRACE, "Se esperaba '{' para el bloque de código.");
    if (peek().type == TokenType::UNKNOWN) return nullptr; // Error de recuperación

    auto block = std::make_unique<BlockStatementNode>();

    while (peek().type != TokenType::RBRACE && !isAtEnd()) {
        if (peek().type == TokenType::KEYWORD_INT) { // Declaración de variable local
            block->children.push_back(parseDeclarationStatement());
        } else { // Otra sentencia
            block->children.push_back(parseStatement());
        }
        if (!block->children.back()) { // Si el parseo de la sentencia falló
            // Intentar avanzar para recuperarse del error y no entrar en bucle infinito
            // Esto es una recuperación básica. Una más robusta implicaría más lógica.
            if (!isAtEnd() && peek().type != TokenType::RBRACE) {
                consume();
            }
        }
    }

    expect(TokenType::RBRACE, "Se esperaba '}' para cerrar el bloque de código.");
    return block;
}

// <statement> ::= <assignmentStatement> ";"
//               | <ifStatement>
//               | <forStatement>
//               | <returnStatement> ";"
//               | <printStatement> ";"
//               | <functionCall> ";" (si no es printf)
std::unique_ptr<ASTNode> Parser::parseStatement() {
    switch (peek().type) {
        case TokenType::IDENTIFIER: {
            if (peek(1).type == TokenType::ASSIGN) { // Asignación
                auto assign = parseAssignmentStatement();
                expect(TokenType::SEMICOLON, "Se esperaba ';' después de la sentencia de asignación.");
                return assign;
            } else if (peek(1).type == TokenType::LPAREN) { // Llamada a función (no printf)
                auto funcCall = parseFunctionCall();
                expect(TokenType::SEMICOLON, "Se esperaba ';' después de la llamada a función.");
                return funcCall;
            }
            break;
        }
        case TokenType::KEYWORD_IF:
            return parseIfStatement();
        case TokenType::KEYWORD_FOR:
            return parseForStatement();
        case TokenType::KEYWORD_RETURN: {
            auto ret = parseReturnStatement();
            expect(TokenType::SEMICOLON, "Se esperaba ';' después de la sentencia return.");
            return ret;
        }
        case TokenType::KEYWORD_PRINTF: {
            auto print = parsePrintStatement();
            expect(TokenType::SEMICOLON, "Se esperaba ';' después de la sentencia printf.");
            return print;
        }
        case TokenType::LBRACE: // Un bloque de sentencias independiente
            return parseBlockStatement();
        default:
            ErrorHandler::reportError("Sentencia inesperada.", peek().line, peek().column);
            consume(); // Intenta recuperarse
            return nullptr;
    }
    ErrorHandler::reportError("Sentencia inválida o incompleta.", peek().line, peek().column);
    consume(); // Intenta recuperarse
    return nullptr;
}

// <declarationStatement> ::= "int" IDENTIFIER [ "=" <expression> ] ";"
std::unique_ptr<ASTNode> Parser::parseDeclarationStatement() {
    Token typeToken = expect(TokenType::KEYWORD_INT, "Se esperaba el tipo 'int' para la declaración de variable.");
    if (typeToken.type == TokenType::UNKNOWN) return nullptr;

    Token varName = expect(TokenType::IDENTIFIER, "Se esperaba un nombre de variable.");
    if (varName.type == TokenType::UNKNOWN) return nullptr;

    std::unique_ptr<ASTNode> initializer = nullptr;
    if (match(TokenType::ASSIGN)) {
        initializer = parseExpression();
    }

    expect(TokenType::SEMICOLON, "Se esperaba ';' después de la declaración de variable.");
    return std::make_unique<VariableDeclarationNode>(typeToken.value, varName.value, std::move(initializer));
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
    if (peek().type == TokenType::KEYWORD_INT) { // Declaración
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
        increment = parseAssignmentStatement(); // Solo asignaciones simples para incremento
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

    auto printNode = std::make_unique<PrintStatementNode>(formatStringToken.value);

    while (match(TokenType::COMMA)) {
        auto arg = parseExpression();
        if (arg) {
            printNode->arguments.push_back(std::move(arg));
        } else {
            ErrorHandler::reportError("Expresión de argumento esperada en printf.", peek().line, peek().column);
            // Intenta recuperar si hay error en la expresión
            if (peek().type != TokenType::RPAREN) consume();
        }
    }

    expect(TokenType::RPAREN, "Se esperaba ')' después de los argumentos de printf.");
    return printNode;
}

// <functionCall> ::= IDENTIFIER "(" [ <argumentList> ] ")"
std::unique_ptr<ASTNode> Parser::parseFunctionCall() {
    Token funcName = expect(TokenType::IDENTIFIER, "Se esperaba un nombre de función para la llamada.");
    if (funcName.type == TokenType::UNKNOWN) return nullptr;

    auto funcCallNode = std::make_unique<FunctionCallNode>(funcName.value);

    expect(TokenType::LPAREN, "Se esperaba '(' para la llamada a función.");
    if (peek().type == TokenType::UNKNOWN) return nullptr;

    // Parsear argumentos
    while (peek().type != TokenType::RPAREN && peek().type != TokenType::END_OF_FILE) {
        auto arg = parseExpression();
        if (arg) {
            funcCallNode->arguments.push_back(std::move(arg));
        } else {
            ErrorHandler::reportError("Expresión de argumento esperada en la llamada a función.", peek().line, peek().column);
            // Intenta recuperar si hay error en la expresión
            if (peek().type != TokenType::RPAREN) consume();
        }
        if (peek().type == TokenType::COMMA) {
            consume();
        } else if (peek().type != TokenType::RPAREN) {
            ErrorHandler::reportError("Se esperaba ',' o ')' después de los argumentos de la función.", peek().line, peek().column);
            // Recuperación de error: avanzar para evitar bucle infinito
            if (peek().type != TokenType::RPAREN) consume();
        }
    }

    expect(TokenType::RPAREN, "Se esperaba ')' para cerrar la llamada a función.");
    return funcCallNode;
}

// <expression> ::= <equalityExpression>
std::unique_ptr<ASTNode> Parser::parseExpression() {
    return parseEqualityExpression();
}

// <equalityExpression> ::= <comparisonExpression> ( ( "==" | "!=" ) <comparisonExpression> )*
std::unique_ptr<ASTNode> Parser::parseEqualityExpression() {
    auto expr = parseComparisonExpression();

    while (peek().type == TokenType::EQUAL_EQUAL || peek().type == TokenType::NOT_EQUAL) {
        Token op = consume();
        auto right = parseComparisonExpression();
        if (!right) return nullptr;
        expr = std::make_unique<BinaryExpressionNode>(std::move(expr), std::move(right), op.value);
    }
    return expr;
}

// <comparisonExpression> ::= <additiveExpression> ( ( ">" | ">=" | "<" | "<=" ) <additiveExpression> )*
std::unique_ptr<ASTNode> Parser::parseComparisonExpression() {
    auto expr = parseAdditiveExpression();

    while (peek().type == TokenType::GREATER_THAN || peek().type == TokenType::GREATER_EQUAL ||
           peek().type == TokenType::LESS_THAN || peek().type == TokenType::LESS_EQUAL) {
        Token op = consume();
        auto right = parseAdditiveExpression();
        if (!right) return nullptr;
        expr = std::make_unique<BinaryExpressionNode>(std::move(expr), std::move(right), op.value);
    }
    return expr;
}

// <additiveExpression> ::= <multiplicativeExpression> ( ( "+" | "-" ) <multiplicativeExpression> )*
std::unique_ptr<ASTNode> Parser::parseAdditiveExpression() {
    auto expr = parseMultiplicativeExpression();

    while (peek().type == TokenType::PLUS || peek().type == TokenType::MINUS) {
        Token op = consume();
        auto right = parseMultiplicativeExpression();
        if (!right) return nullptr;
        expr = std::make_unique<BinaryExpressionNode>(std::move(expr), std::move(right), op.value);
    }
    return expr;
}

// <multiplicativeExpression> ::= <primaryExpression> ( ( "*" | "/" ) <primaryExpression> )*
std::unique_ptr<ASTNode> Parser::parseMultiplicativeExpression() {
    auto expr = parsePrimaryExpression();

    while (peek().type == TokenType::MULTIPLY || peek().type == TokenType::DIVIDE) {
        Token op = consume();
        auto right = parsePrimaryExpression();
        if (!right) return nullptr;
        expr = std::make_unique<BinaryExpressionNode>(std::move(expr), std::move(right), op.value);
    }
    return expr;
}

// <primaryExpression> ::= INTEGER_LITERAL
//                       | STRING_LITERAL
//                       | IDENTIFIER
//                       | "(" <expression> ")"
//                       | <functionCall> (si no es printf)
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
            if (!expr) return nullptr;
            expect(TokenType::RPAREN, "Se esperaba ')' después de la expresión entre paréntesis.");
            return expr;
        }
        default:
            ErrorHandler::reportError("Expresión primaria inesperada.", peek().line, peek().column);
            consume(); // Intenta recuperarse
            return nullptr;
    }
}