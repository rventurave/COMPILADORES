#include "SymbolTable.h"

// Constructor: Inicializa la tabla de símbolos con un scope global.
SymbolTable::SymbolTable() {
    enterScope(); // El primer scope es el global
}

// Entra en un nuevo scope.
void SymbolTable::enterScope() {
    scopes.emplace_back(); // Añade un nuevo mapa vacío a la pila de scopes
}

// Sale del scope actual.
void SymbolTable::exitScope() {
    if (!scopes.empty()) {
        scopes.pop_back(); // Elimina el scope superior de la pila
    }
    // Podrías añadir un error si se intenta salir de un scope vacío,
    // pero en un uso correcto, esto no debería ocurrir.
}

// Añade un símbolo al scope actual.
bool SymbolTable::addSymbol(const std::string& name, std::unique_ptr<Symbol> symbol) {
    if (scopes.empty()) {
        // Esto no debería suceder si siempre se entra en un scope al inicio.
        return false;
    }
    // Comprueba si el símbolo ya existe en el scope actual
    if (scopes.back().count(name)) {
        return false; // Símbolo ya existe en el scope actual
    }
    scopes.back()[name] = std::move(symbol);
    return true;
}

// Busca un símbolo en el scope actual y en los scopes padres.
Symbol* SymbolTable::lookupSymbol(const std::string& name) {
    // Itera desde el scope actual hacia arriba (hacia el scope global)
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        if (it->count(name)) {
            return it->at(name).get(); // Devuelve el puntero crudo al Symbol
        }
    }
    return nullptr; // Símbolo no encontrado
}

// Busca un símbolo solo en el scope actual.
Symbol* SymbolTable::lookupSymbolInCurrentScope(const std::string& name) {
    if (scopes.empty()) {
        return nullptr;
    }
    auto& currentScope = scopes.back();
    if (currentScope.count(name)) {
        return currentScope.at(name).get();
    }
    return nullptr;
}