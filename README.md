# GLX - GPU Linux Extension

Un lenguaje de programación interpretado para control de rendimiento del sistema, desarrollado en C para laptops gaming con GPU NVIDIA y características Lenovo Legion.

## Descripción

GLX (GPU Linux Extension) es un lenguaje de programación interpretado que permite controlar el rendimiento de tu sistema Linux de manera intuitiva y eficiente. Diseñado específicamente para laptops gaming con GPU NVIDIA y características Lenovo Legion, proporciona control granular sobre CPU, GPU, batería y características RGB.

## Características principales

- **Control de GPU NVIDIA** - Gestión de rendimiento y configuraciones
- **Control de CPU Intel** - Ajuste de rendimiento máximo/mínimo y Turbo Boost
- **Gestión de batería** - Conservación de batería y optimización
- **Control RGB** - Color del botón de encendido y brillo del teclado
- **Modos predefinidos** - Quiet, Balanced y Performance
- **Instalación global** - Usable desde cualquier directorio del sistema
- **Fuzzy matching** - Sugerencias inteligentes para comandos
- **Sistema de variables** - Programación dinámica y reutilizable

## Instalación

### 1. Verificar compatibilidad

Antes de instalar, verifica que tu sistema es compatible:

```bash
git clone https://github.com/nicolaspaez-dot/GLX.git
cd GLX
chmod +x check_compatibility.sh
./check_compatibility.sh
```

### 2. Instalación automática

El script de instalación detecta automáticamente tu distribución y instala las dependencias necesarias:

```bash
./install.sh
```

**Distribuciones soportadas:**
- Arch Linux / Manjaro
- Ubuntu / Debian
- Fedora

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

| Modo | CPU Max | CPU Min | Dynamic Boost | Turbo Boost | Batería | Color Botón | Brillo Teclado |
|------|---------|---------|---------------|-------------|---------|-------------|----------------|
| **Quiet** | 60% | 20% | OFF | OFF | Conservación ON | Azul | 30% |
| **Balanced** | 80% | 40% | ON | ON | Conservación OFF | Blanco | 60% |
| **Performance** | 100% | 60% | ON | ON | Conservación OFF | Rojo | 100% |

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

### Funcionalidades por hardware

| Función | CPU Intel | GPU NVIDIA | Lenovo Legion | Otros |
|---------|-----------|------------|---------------|-------|
| Control CPU | ✅ | ❌ | ❌ | ❌ |
| Control GPU | ❌ | ✅ | ❌ | ❌ |
| Conservación batería | ❌ | ❌ | ✅ | ❌ |
| Color botón encendido | ❌ | ❌ | ✅ | ❌ |
| Brillo teclado | ❌ | ❌ | ✅ | ⚠️ |

## Estructura del proyecto

```
GLX/
├── src/                    # Archivos fuente (.c)
├── include/               # Headers (.h)
├── gx_pruebas/           # Archivos de prueba
├── install.sh            # Script de instalación
├── uninstall.sh          # Script de desinstalación
├── check_compatibility.sh # Verificación de compatibilidad
├── modelo.txt            # Configuraciones de modos
└── README.md             # Este archivo
```

## Solución de problemas

### Error: "nvidia-smi no está disponible"
```bash
# Instalar drivers NVIDIA
sudo pacman -S nvidia nvidia-utils  # Arch
sudo apt install nvidia-driver      # Ubuntu
```

### Error: "legion_cli no está disponible"
```bash
# Para laptops Lenovo Legion
sudo pacman -S linux-headers
yay -S lenovolegionlinux-git
```

### Error: "Intel P-State no está disponible"
- Verifica que tienes un CPU Intel
- Algunas funciones de CPU pueden no funcionar

### Error de compilación
```bash
# Verificar herramientas de compilación
sudo pacman -S gcc make  # Arch
sudo apt install gcc make # Ubuntu
```

### Permisos insuficientes
```bash
# Algunas funciones requieren sudo
sudo gx run mode:performance
```

## Desarrollador

**Nicolás Paez**

- **Email**: nico.paezdelgadillospr@gmail.com
- **GitHub**: [nicolaspaez-dot](https://github.com/nicolaspaez-dot)

## Licencia

Este proyecto está bajo la Licencia Creative Commons Attribution-NonCommercial-NoDerivatives 4.0 International - ver el archivo [LICENSE](LICENSE) para detalles.
