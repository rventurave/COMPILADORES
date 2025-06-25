#include "ErrorHandler.h"

// Inicialización de los miembros estáticos
std::vector<CompilerMessage> ErrorHandler::messages;
bool ErrorHandler::foundError = false;

// Reporta un error de compilación.
void ErrorHandler::reportError(const std::string& message, int line, int column) {
    messages.emplace_back(CompilerMessage::MessageType::ERROR, message, line, column);
    foundError = true;
    std::cerr << "ERROR";
    if (line != -1) {
        std::cerr << " (Línea: " << line;
        if (column != -1) {
            std::cerr << ", Columna: " << column;
        }
        std::cerr << ")";
    }
    std::cerr << ": " << message << std::endl;
}

// Reporta una advertencia de compilación.
void ErrorHandler::reportWarning(const std::string& message, int line, int column) {
    messages.emplace_back(CompilerMessage::MessageType::WARNING, message, line, column);
    std::cerr << "ADVERTENCIA";
    if (line != -1) {
        std::cerr << " (Línea: " << line;
        if (column != -1) {
            std::cerr << ", Columna: " << column;
        }
        std::cerr << ")";
    }
    std::cerr << ": " << message << std::endl;
}

// Obtiene todos los mensajes (errores y advertencias) acumulados.
const std::vector<CompilerMessage>& ErrorHandler::getMessages() {
    return messages;
}

// Limpia todos los mensajes.
void ErrorHandler::clearMessages() {
    messages.clear();
    foundError = false;
}

// Retorna true si hay al menos un error.
bool ErrorHandler::hasErrors() {
    return foundError;
}