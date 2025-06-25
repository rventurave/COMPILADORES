// src/semantic_analyzer/SymbolTable.h
#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <string>
#include <vector>
#include <map>
#include <memory> // Para std::unique_ptr
#include <utility> // Para std::move en constructores de Symbol

// Enumeración para el tipo de símbolo (variable, función, etc.)
enum class SymbolType { // <--- ¡Asegúrate de que este enum esté definido!
    VARIABLE,
    FUNCTION
};

// Estructura Symbol: Representa una entrada en la tabla de símbolos.
struct Symbol {
    std::string name;
    SymbolType symbolType; // Si es VARIABLE o FUNCTION
    std::string dataType; // Tipo de dato (ej. "int", "void")

    // Para funciones, los parámetros se almacenan aquí (tipo y nombre)
    std::vector<std::pair<std::string, std::string>> parameters; // <--- ¡NUEVO MIEMBRO!

    // Constructor para variables
    Symbol(std::string name, SymbolType symbolType, std::string dataType)
        : name(std::move(name)), symbolType(symbolType), dataType(std::move(dataType)) {}

    // Constructor para funciones (incluye parámetros)
    Symbol(std::string name, SymbolType symbolType, std::string dataType,
           std::vector<std::pair<std::string, std::string>> params)
        : name(std::move(name)), symbolType(symbolType), dataType(std::move(dataType)),
          parameters(std::move(params)) {}
};

// Clase SymbolTable: Gestiona ámbitos y símbolos (variables, funciones, etc.)
class SymbolTable {
public:
    SymbolTable();

    // Entra a un nuevo ámbito.
    void enterScope();

    // Sale del ámbito actual.
    void exitScope();

    // Añade un símbolo al ámbito actual. Retorna true si se añadió con éxito, false si ya existe.
    // Ahora espera un unique_ptr<Symbol>
    bool addSymbol(std::unique_ptr<Symbol> symbol); // <--- ¡FIRMA MODIFICADA!

    // Busca un símbolo solo en el ámbito actual.
    Symbol* lookupSymbolInCurrentScope(const std::string& name);

    // Busca un símbolo en todos los ámbitos activos (desde el actual hasta el global).
    Symbol* lookupSymbol(const std::string& name);

private:
    std::vector<std::map<std::string, std::unique_ptr<Symbol>>> scopes;
};

#endif // SYMBOLTABLE_H