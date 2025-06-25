// src/semantic_analyzer/SymbolTable.cpp
#include "SymbolTable.h"
#include <algorithm> // Para std::reverse
#include <iostream> // Para depuración

SymbolTable::SymbolTable() {
    enterScope(); // Entrar en el ámbito global por defecto al construir la tabla
}

void SymbolTable::enterScope() {
    scopes.emplace_back(); // Añadir un nuevo mapa (ámbito) al vector
}

void SymbolTable::exitScope() {
    if (!scopes.empty()) {
        scopes.pop_back(); // Eliminar el ámbito actual
    } else {
        // Esto no debería ocurrir en un programa bien formado
        std::cerr << "Error: Intentando salir de un ámbito vacío." << std::endl;
    }
}

// Implementación de addSymbol: Ahora toma std::unique_ptr<Symbol>
bool SymbolTable::addSymbol(std::unique_ptr<Symbol> symbol) {
    if (scopes.empty()) {
        std::cerr << "Error: No hay ámbitos para añadir símbolos." << std::endl;
        return false;
    }
    std::string name = symbol->name; // Obtener el nombre del símbolo
    if (scopes.back().count(name) > 0) {
        // Símbolo ya existe en el ámbito actual
        return false;
    }
    scopes.back()[name] = std::move(symbol); // Mover el unique_ptr al mapa
    return true;
}

Symbol* SymbolTable::lookupSymbolInCurrentScope(const std::string& name) {
    if (scopes.empty()) {
        return nullptr;
    }
    auto it = scopes.back().find(name);
    if (it != scopes.back().end()) {
        return it->second.get(); // Devolver el puntero raw
    }
    return nullptr;
}

Symbol* SymbolTable::lookupSymbol(const std::string& name) {
    // Buscar desde el ámbito actual hacia el global
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto symbolIt = it->find(name);
        if (symbolIt != it->end()) {
            return symbolIt->second.get();
        }
    }
    return nullptr;
}