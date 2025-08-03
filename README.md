# GLX - GPU Linux Extension

> **Un lenguaje de programación interpretado para control de rendimiento del sistema, desarrollado por un estudiante de 18 años**

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Made with C](https://img.shields.io/badge/Made%20with-C-blue.svg)](https://www.cprogramming.com/)
[![Linux](https://img.shields.io/badge/Linux-Arch%20Linux-red.svg)](https://archlinux.org/)

## ¿Qué es GLX?

**GLX** (GPU Linux Extension) es un lenguaje de programación interpretado escrito en C que permite controlar el rendimiento de tu sistema Linux de manera intuitiva y eficiente. Diseñado específicamente para laptops gaming con GPU NVIDIA y características Lenovo Legion.

### Características principales

- **Control de GPU NVIDIA** - Gestión de rendimiento y configuraciones
- **Control de CPU Intel** - Ajuste de rendimiento máximo/mínimo y Turbo Boost
- **Gestión de batería** - Conservación de batería y optimización
- **Modos predefinidos** - Quiet, Balanced y Performance
- **Instalación global** - Usable desde cualquier directorio del sistema
- **Fuzzy matching** - Sugerencias inteligentes para comandos
- **Sistema de variables** - Programación dinámica y reutilizable

## Instalación

### Requisitos previos
```bash
# En Arch Linux / Manjaro
sudo pacman -S gcc make python-pillow

# En Ubuntu / Debian
sudo apt install gcc make python3-pil
```

### Instalación rápida
```bash
git clone https://github.com/nicolaspaez-dot/GLX.git
cd GLX
./install.sh
```

Ahora puedes usar `gx` desde cualquier directorio.

## Uso básico

### Comandos principales
```bash
gx help                    # Mostrar ayuda
gx status                  # Estado del sistema
gx vars                    # Variables definidas
gx run mode:quiet          # Modo silencioso (bajo rendimiento)
gx run mode:balanced       # Modo equilibrado
gx run mode:performance    # Modo máximo rendimiento
```

### Ejemplo de archivo GLX
```bash
# archivo.gx
# Configuración personalizada
mi_potencia = 85
mi_turbo = 1

# Aplicar configuraciones
cpu_max_perf: mi_potencia
turbo_boost: mi_turbo
dynamic_boost: 1
```

Ejecutar: `gx archivo.gx`

## Modos disponibles

| Modo | CPU Max | CPU Min | Dynamic Boost | Turbo Boost | Batería |
|------|---------|---------|---------------|-------------|---------|
| **Quiet** | 60% | 20% | OFF | OFF | Conservación ON |
| **Balanced** | 80% | 40% | ON | ON | Conservación OFF |
| **Performance** | 100% | 60% | ON | ON | Conservación OFF |

## Arquitectura técnica

GLX está construido con una arquitectura modular:

```
Archivo .gx → Lexer → Parser → AST → Interpreter → Comandos del sistema
```

- **Lexer**: Tokenización del código fuente
- **Parser**: Construcción del árbol sintáctico (AST)
- **Interpreter**: Ejecución de comandos del sistema
- **Utils**: Funciones auxiliares y fuzzy matching

## Compatibilidad

### Hardware soportado
- **GPU**: NVIDIA GeForce RTX 3050 Laptop (y similares)
- **CPU**: Intel con P-State
- **Laptop**: Lenovo Legion (con LenovoLegionLinux)
- **Otros**: Compatible con sistemas Linux similares

### Distribuciones Linux
- Arch Linux / Manjaro
- Ubuntu / Debian
- Fedora
- Otras distribuciones con kernel Linux

## Estructura del proyecto

```
GLX/
├── src/            # Archivos fuente (.c)
├── include/        # Headers (.h)
├── gx_pruebas/     # Archivos de prueba
├── install.sh      # Script de instalación
├── uninstall.sh    # Script de desinstalación
├── modelo.txt      # Configuraciones de modos
└── README.md       # Este archivo
```

## Sobre el desarrollador

Soy un estudiante de 18 años que quiso crear algo útil para la comunidad. Este proyecto nació de mi curiosidad por entender cómo funcionan los lenguajes de programación y mi necesidad de tener un control más granular sobre el rendimiento de mi laptop gaming.

### ¿Por qué GLX?
- **Aprendizaje**: Quería entender la implementación de lenguajes interpretados
- **Necesidad**: Mi laptop necesitaba un control de rendimiento más flexible
- **Desafío**: Crear algo útil desde cero con C
- **Compartir**: Contribuir a la comunidad open source

## Contacto

**Email**: nico.paezdelgadillospr@gmail.com

## Licencia

Este proyecto está bajo la Licencia MIT - ver el archivo [LICENSE](LICENSE) para detalles.

---

*Desarrollado con dedicación por un estudiante de 18 años* 
