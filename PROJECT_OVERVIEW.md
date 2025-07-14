# Estructura del Proyecto y Funcionamiento General

## Estructura del Proyecto

- **`src/`**: Contiene el código fuente principal.
  - **`main.c`**: Punto de entrada. Lee un archivo `.gx`, tokeniza cada línea, la parsea a un AST y la interpreta.
  - **`lexer.c`**: Analizador léxico. Convierte líneas de texto en tokens.
  - **`parser.c`**: Analizador sintáctico. Convierte tokens en un árbol de sintaxis abstracta (AST).
  - **`interpreter.c`**: Intérprete. Ejecuta los comandos del AST, modificando el estado simulado de la GPU y gestionando variables.
  - **Archivos `.h`**: Declaraciones de funciones y estructuras para modularidad.
  - **Otros archivos (`runtime.c`, `utils.c`)**: Actualmente vacíos, probablemente para futuras utilidades o funcionalidades.

- **`include/`**: Archivos de cabecera para exponer las interfaces de los módulos principales.

- **`gx_programs/`**: Ejemplos de programas escritos en GLX, como `ejemplo.gx`, que muestra cómo configurar la GPU y usar variables.

- **`build/`**: Carpeta para ejecutables generados (no se sube a Git).

- **`Makefile`**: Script para compilar el proyecto fácilmente.

- **`README.md`**: Explica brevemente el propósito y cómo compilar/ejecutar el proyecto.

- **`GITHUB_GUIDE.md`**: Guía de uso de Git y estructura del proyecto.

---

## Funcionamiento General

1. **Compilación**: Se compila con `make` en la raíz del proyecto.
2. **Ejecución**: El ejecutable lee un archivo `.gx` (por defecto, `gx_programs/ejemplo.gx`).
3. **Fases del procesamiento**:
   - **Lexer**: Divide cada línea en tokens (palabras, símbolos como `:` o `=`).
   - **Parser**: Construye un AST a partir de los tokens.
   - **Interpreter**: Ejecuta los comandos del AST, permitiendo:
     - Cambiar modo de la GPU (`modo: quiet`)
     - Ajustar parámetros (`power_limit: 80`)
     - Consultar estado (`status`)
     - Definir y usar variables (`mi_potencia = 85`, `power_limit: mi_potencia`)
     - Comandos especiales (`reset`, `vars`)
     - Manejo de comandos desconocidos (los reporta como tal). 