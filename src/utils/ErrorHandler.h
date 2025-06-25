#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <string>
#include <vector>
#include <iostream>

// Estructura para almacenar información sobre un error o advertencia.
struct CompilerMessage {
    enum class MessageType {
        ERROR,
        WARNING
    } type;
    std::string message;
    int line;
    int column;

    CompilerMessage(MessageType type, const std::string& msg, int l, int c)
        : type(type), message(msg), line(l), column(c) {}
};

// Clase estática ErrorHandler para reportar y almacenar errores/advertencias.
class ErrorHandler {
public:
    // Reporta un error de compilación.
    static void reportError(const std::string& message, int line = -1, int column = -1);

    // Reporta una advertencia de compilación.
    static void reportWarning(const std::string& message, int line = -1, int column = -1);

    // Obtiene todos los mensajes (errores y advertencias) acumulados.
    static const std::vector<CompilerMessage>& getMessages();

    // Limpia todos los mensajes.
    static void clearMessages();

    // Retorna true si hay al menos un error.
    static bool hasErrors();

private:
    // Lista estática para almacenar todos los mensajes.
    static std::vector<CompilerMessage> messages;
    static bool foundError; // Bandera para indicar si se encontró al menos un error.
};

#endif // ERROR_HANDLER_H