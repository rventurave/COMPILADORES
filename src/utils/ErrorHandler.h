// src/utils/ErrorHandler.h
#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include <string>
#include <vector>
#include <iostream> // Para std::cerr

// Estructura para almacenar un mensaje de compilación (error o advertencia)
struct CompilerMessage {
    std::string message;
    int line;
    int column;
    bool isError; // true si es un error, false si es una advertencia

    CompilerMessage(std::string msg, int l, int c, bool error)
        : message(std::move(msg)), line(l), column(c), isError(error) {}
};

// Clase ErrorHandler: Centraliza el reporte y manejo de errores/advertencias
class ErrorHandler {
public:
    ErrorHandler();

    // Reporta un error. Puede usarse con o sin información de línea/columna.
    // Argumentos: message, line (opcional), column (opcional)
    void reportError(const std::string& message, int line = -1, int column = -1);

    // Reporta una advertencia.
    void reportWarning(const std::string& message, int line = -1, int column = -1);

    // Verifica si se han reportado errores.
    bool hasErrors() const;

    // Obtiene todos los mensajes (errores y advertencias) reportados.
    const std::vector<CompilerMessage>& getMessages() const;

    // Imprime todos los mensajes reportados a la salida de error estándar.
    void printMessages() const; // <--- ¡NUEVO MÉTODO!

    // Limpia todos los mensajes reportados.
    void clearMessages();

private:
    std::vector<CompilerMessage> messages;
    bool errorsExist;
};

#endif // ERRORHANDLER_H1