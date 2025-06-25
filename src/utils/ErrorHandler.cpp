// src/utils/ErrorHandler.cpp
#include "ErrorHandler.h"
#include <iostream> // Ya incluido, pero lo dejo por claridad
#include <utility>  // Para std::move

ErrorHandler::ErrorHandler() : errorsExist(false) {}

void ErrorHandler::reportError(const std::string& message, int line, int column) {
    messages.emplace_back(message, line, column, true);
    errorsExist = true;
}

void ErrorHandler::reportWarning(const std::string& message, int line, int column) {
    messages.emplace_back(message, line, column, false);
}

bool ErrorHandler::hasErrors() const {
    return errorsExist;
}

const std::vector<CompilerMessage>& ErrorHandler::getMessages() const {
    return messages;
}

// Implementación del nuevo método printMessages
void ErrorHandler::printMessages() const {
    for (const auto& msg : messages) {
        if (msg.isError) {
            std::cerr << "Error";
        } else {
            std::cerr << "Warning";
        }

        if (msg.line != -1) {
            std::cerr << " (" << msg.line;
            if (msg.column != -1) {
                std::cerr << ":" << msg.column;
            }
            std::cerr << ")";
        }
        std::cerr << ": " << msg.message << std::endl;
    }
}


void ErrorHandler::clearMessages() {
    messages.clear();
    errorsExist = false;
}