# Compilador con Visualización de Memoria SFML

Este proyecto es un compilador simple que toma un subconjunto de código similar a C y lo traduce a código C++ con una capa de visualización utilizando la biblioteca SFML. La aplicación SFML generada permite a los usuarios observar la ejecución del programa, la gestión de la pila de llamadas y la asignación de memoria en el heap, paso a paso, a través de una interfaz gráfica interactiva.

---

## Características

**Análisis Léxico y Sintáctico**

- Procesa el código fuente para identificar tokens.
- Construye un Árbol de Sintaxis Abstracta (AST).

**Análisis Semántico**

- Realiza comprobaciones de tipo.
- Gestiona la tabla de símbolos.

**Generación de Código**

- Transforma el AST en código C++ ejecutable.

**Visualización SFML**

- Muestra el estado de la pila de llamadas (Stack) con variables y sus valores.
- Muestra el estado del heap con objetos asignados y sus direcciones/valores.
- Navegación paso a paso mediante botones **Previous** y **Next** para avanzar o retroceder en la ejecución.
- Ventana de visualización de **1000x500 píxeles**.
- Mensajes descriptivos en cada paso de la simulación.

---

## Requisitos Previos

Para construir y ejecutar este proyecto, necesitarás:

- **Un compilador C++**  
  Compatible con C++11 o superior (ej. MinGW-w64 en Windows, g++ en Linux/macOS).

- **CMake**  
  Sistema de automatización de construcción de código abierto.

- **SFML 2.6.1 (o superior)**  
  Biblioteca multimedia simple y rápida.  
  Asegúrate de tener los archivos de desarrollo (headers y librerías) instalados y accesibles.  
  [Descargar SFML](https://www.sfml-dev.org/download.php)

- **Fuentes (Fonts)**  
  El proyecto espera encontrar el archivo `arial.ttf` en la ruta:
  relativa al ejecutable. Puedes colocar cualquier archivo `.ttf` y renombrarlo, o modificar la ruta en `SFMLTranslator.cpp`.

- Asegúrate de que las rutas a los includes y librerías de SFML sean correctas, por ejemplo:

---

## Construcción del Proyecto (el Compilador)

Sigue estos pasos para construir el compilador a partir de su código fuente.

### 1. Clona el repositorio

```bash
git clone https://github.com/rventurave/COMPILADORES.git
cd COMPILADORES

## Construcción del Proyecto (el Compilador)

Sigue estos pasos para construir el compilador a partir de su código fuente.

---

### 2. Crea el directorio de build

```bash
mkdir build
cd build
