#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <map>
#include <vector>
#include <memory>

// Estructura para almacenar información de un símbolo (variable, función, etc.)
struct Symbol {
    enum class SymbolType {
        VARIABLE,
        FUNCTION
    } type;
    std::string name;
    std::string dataType; // Ej. "int", "void"
    // Información adicional si es una función
    std::vector<std::string> paramTypes; // Tipos de los parámetros
    // ... más campos según sea necesario (scope, inicializado, etc.)

    Symbol(SymbolType type, const std::string& name, const std::string& dataType)
        : type(type), name(name), dataType(dataType) {}
};

// Clase para manejar la tabla de símbolos (manejo de scopes)
class SymbolTable {
public:
    SymbolTable();

    // Inicia un nuevo scope (ej. al entrar a una función o bloque)
    void enterScope();
    // Sale del scope actual
    void exitScope();

    // Añade un símbolo al scope actual
    bool addSymbol(const std::string& name, std::unique_ptr<Symbol> symbol);

    // Busca un símbolo en el scope actual y en los scopes padres
    // Retorna un puntero al símbolo si lo encuentra, nullptr en caso contrario.
    Symbol* lookupSymbol(const std::string& name);

    // Busca un símbolo solo en el scope actual
    Symbol* lookupSymbolInCurrentScope(const std::string& name);

private:
    // Pila de scopes. Cada scope es un mapa de nombre de símbolo a Symbol.
    std::vector<std::map<std::string, std::unique_ptr<Symbol>>> scopes;
};

#endif // SYMBOL_TABLE_H