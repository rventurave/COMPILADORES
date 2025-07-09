Compilador con Visualización de Memoria SFML
Este proyecto es un compilador simple que toma un subconjunto de código similar a C y lo traduce a código C++ con una capa de visualización utilizando la biblioteca SFML. La aplicación SFML generada permite a los usuarios observar la ejecución del programa, la gestión de la pila de llamadas y la asignación de memoria en el heap, paso a paso, a través de una interfaz gráfica interactiva.

Características
Análisis Léxico y Sintáctico: Procesa el código fuente para identificar tokens y construir un Árbol de Sintaxis Abstracta (AST).

Análisis Semántico: Realiza comprobaciones de tipo y gestión de la tabla de símbolos.

Generación de Código: Transforma el AST en código C++ ejecutable.

Visualización SFML:

Muestra el estado de la pila de llamadas (Stack) con variables y sus valores.

Muestra el estado del heap con objetos asignados y sus direcciones/valores.

Navegación Paso a Paso: Botones "Previous" y "Next" para avanzar o retroceder en la ejecución del programa y observar los cambios en la memoria.

Ventana de visualización de 1000x500 píxeles.

Mensajes de descripción de cada paso de la simulación.

Requisitos Previos
Para construir y ejecutar este proyecto, necesitarás:

Un compilador C++: Compatible con C++11 o superior (por ejemplo, MinGW-w64 en Windows, g++ en Linux/macOS).

CMake: Un sistema de automatización de construcción de código abierto.

SFML 2.6.1 (o superior): La biblioteca multimedia simple y rápida. Asegúrate de tener los archivos de desarrollo (headers y librerías) instalados y accesibles.

Puedes descargar SFML desde su sitio web oficial.

Asegúrate de que la ruta a los includes (-IC:/SFML-2.6.1/include) y las librerías (-LC:/SFML-2.6.1/lib) sea correcta para tu instalación.

Una fuente (font) arial.ttf: El proyecto espera encontrarla en ../../resources/arial.ttf relativa al ejecutable. Puedes colocar cualquier archivo .ttf y renombrarlo, o modificar la ruta en SFMLTranslator.cpp.

Construcción del Proyecto (el Compilador)
Sigue estos pasos para construir el compilador a partir de su código fuente:

Clonar el repositorio:

git clone <URL_DEL_REPOSITORIO>
cd COMPILADORES

Crear un directorio de construcción:

mkdir build
cd build

Configurar CMake:

cmake ..

Si CMake no encuentra SFML automáticamente, es posible que necesites especificar la ruta a tu instalación de SFML:

cmake -DCMAKE_PREFIX_PATH="C:/SFML-2.6.1" ..

(Ajusta la ruta C:/SFML-2.6.1 a tu instalación real de SFML).

Compilar el proyecto:

cmake --build .

Esto compilará el ejecutable del compilador (por ejemplo, C_SFML_Compiler.exe o C_SFML_Compiler).

Uso del Compilador y Generación de la Visualización
Una vez que el compilador esté construido, puedes usarlo para generar el archivo output_sfml.cpp a partir de tu código fuente C.

Prepara tu código fuente C: Coloca tu archivo de código C de entrada (por ejemplo, input.txt o programa.c) en el directorio build/src/ o el directorio donde esperas que el compilador lo lea.

Ejecuta el compilador:

./src/C_SFML_Compiler <ruta_a_tu_archivo_c_de_entrada>

Por ejemplo:

./src/C_SFML_Compiler src/input.txt

Esto generará un archivo llamado output_sfml.cpp en el mismo directorio.

Compila el archivo de visualización SFML:

g++ output_sfml.cpp -o output_sfml.exe -IC:/SFML-2.6.1/include -LC:/SFML-2.6.1/lib -lsfml-graphics -lsfml-window -lsfml-system

Importante: Asegúrate de que las rutas a los includes (-I) y las librerías (-L) de SFML sean correctas para tu sistema.

Ejecuta la visualización:

./output_sfml.exe

Se abrirá una ventana de SFML que mostrará la simulación de tu programa C.

Estructura del Proyecto
CMakeLists.txt: Archivo de configuración principal de CMake.

src/: Contiene el código fuente del compilador.

src/CMakeLists.txt: Archivo de configuración de CMake para el módulo src.

src/main.cpp: Punto de entrada del compilador.

src/lexer/: Contiene el analizador léxico (Lexer.h, Lexer.cpp, Token.h, Token.cpp).

src/parser/: Contiene el analizador sintáctico (Parser.h, Parser.cpp, AST.h, AST.cpp).

src/semantic_analyzer/: Contiene el analizador semántico (SemanticAnalyzer.h, SemanticAnalyzer.cpp, SymbolTable.h, SymbolTable.cpp).

src/code_generator/: Contiene el generador de código.

CodeGenerator.h, CodeGenerator.cpp: Lógica principal de generación de código.

SFMLTranslator.h, SFMLTranslator.cpp: Contiene las funciones para generar el código SFML para la visualización.

src/utils/: Utilidades como el manejador de errores (ErrorHandler.h, ErrorHandler.cpp).

build/: Directorio de construcción (generado por CMake). Aquí se encuentran los ejecutables y los archivos intermedios.

build/src/output_sfml.cpp: El archivo C++ generado por el compilador, que utiliza SFML para la visualización.

resources/: (Debes crear este directorio si no existe) Contendrá recursos como el archivo de fuente arial.ttf.

Contribuciones
Las contribuciones son bienvenidas. Por favor, abre un "issue" o envía un "pull request" si tienes mejoras o correcciones.

Licencia
Este proyecto está bajo la Licencia MIT. Consulta el archivo LICENSE para más detalles.