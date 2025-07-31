# VISION COMPLETA - GLX (GPU Linux Extension)

## 🎯 **VISIÓN GENERAL**

**GLX** es un lenguaje de programación interpretado diseñado específicamente para el control de rendimiento de sistemas Linux, con enfoque especial en laptops con GPU NVIDIA. El proyecto nació de la necesidad de tener un control granular y programático del rendimiento del sistema, similar a Lenovo Vantage pero con mayor flexibilidad y automatización.

## 🚀 **OBJETIVOS PRINCIPALES**

### **1. Control Real del Sistema**
- **Dynamic Boost**: Control del balance CPU-GPU en tiempo real
- **CPU Performance**: Control granular del rendimiento de CPU (0-100%)
- **Turbo Boost**: Activación/desactivación inteligente del turbo
- **LenovoLegionLinux**: Integración con funciones específicas de Lenovo
- **Persistence Mode**: Configuración persistente de GPU

### **2. Experiencia de Usuario Avanzada**
- **Fuzzy Matching**: Sugerencias inteligentes para errores de escritura
- **Validación Estricta**: Prevención de configuraciones inválidas
- **Sistema de Variables**: Programación dinámica y reutilizable
- **Colores ANSI**: Interfaz visual atractiva y clara
- **Manejo de Errores**: Mensajes informativos y acciones correctivas

### **3. Automatización y Scripting**
- **Archivos de Configuración**: Definición de modos predefinidos
- **Variables Dinámicas**: Cálculos y configuraciones programáticas
- **Comentarios**: Documentación inline en archivos GLX
- **Modos Predefinidos**: Quiet, Balanced, Performance

## 🏗️ **ARQUITECTURA TÉCNICA**

### **Componentes Principales**

#### **1. Lexer (`src/lexer.c`)**
- **Función**: Tokenización del código fuente
- **Capacidades**: Reconocimiento de identificadores, números, strings, operadores
- **Salida**: Array de tokens para el parser

#### **2. Parser (`src/parser.c`)**
- **Función**: Construcción del Árbol de Sintaxis Abstracta (AST)
- **Capacidades**: Análisis sintáctico, detección de declaraciones y asignaciones
- **Salida**: AST estructurado para el interpreter

#### **3. Interpreter (`src/interpreter.c`)**
- **Función**: Ejecución de comandos y aplicación de configuraciones
- **Capacidades**: 
  - Validación de rangos y tipos
  - Sistema de variables
  - Fuzzy matching
  - Ejecución de comandos del sistema
- **Salida**: Cambios en el sistema y feedback al usuario

#### **4. Utils (`src/utils.c`)**
- **Función**: Funciones auxiliares y utilidades
- **Capacidades**:
  - Distancia de Levenshtein para fuzzy matching
  - Ejecución de comandos del sistema
  - Carga de configuraciones desde archivos
- **Salida**: Sugerencias y ejecución de comandos

#### **5. Main (`src/main.c`)**
- **Función**: Punto de entrada y manejo de argumentos CLI
- **Capacidades**: Comandos directos (help, status, reset, vars)
- **Salida**: Interfaz de línea de comandos

### **Estructuras de Datos**

#### **ASTNode**
```c
typedef struct ASTNode {
    NodeType type;
    char* value;
    struct ASTNode** children;
    int num_children;
} ASTNode;
```

#### **GPU_Mode**
```c
typedef struct {
    char name[50];
    int dynamic_boost;
    int cpu_max_perf;
    int cpu_min_perf;
    int turbo_boost;
    int persist_mode;
    int battery_conservation;
    int fnlock;
} GPU_Mode;
```

#### **Variable**
```c
typedef struct {
    char name[MAX_VAR_NAME];
    char value[100];
    int is_number;
} Variable;
```

## 🎨 **SINTAXIS Y LENGUAJE**

### **Comandos Básicos**
```glx
# Comentarios
# Definir variables
mi_perf = 80
mi_boost = 1

# Aplicar configuraciones
dynamic_boost: mi_boost
cpu_max_perf: mi_perf
cpu_min_perf: 40
turbo_boost: 0
persist_mode: 1
battery_conservation: 0
fnlock: 1

# Comandos del sistema
status
help
vars
```

### **Comandos CLI**
```bash
gx help                    # Mostrar ayuda
gx status                  # Estado del sistema
gx reset                   # Resetear configuración
gx vars                    # Mostrar variables
gx archivo.gx             # Ejecutar archivo GLX
```

### **Parámetros Soportados**
- `dynamic_boost`: 0-1 (Activar/desactivar Dynamic Boost)
- `cpu_max_perf`: 0-100% (Rendimiento máximo de CPU)
- `cpu_min_perf`: 0-100% (Rendimiento mínimo de CPU)
- `turbo_boost`: 0-1 (Activar/desactivar Turbo Boost)
- `persist_mode`: 0-1 (Activar/desactivar Persistence Mode)
- `battery_conservation`: 0-1 (Conservación de batería)
- `fnlock`: 0-1 (Bloqueo de teclas función)

## 🔧 **FUNCIONALIDADES AVANZADAS**

### **1. Fuzzy Matching Inteligente**
- **Algoritmo**: Distancia de Levenshtein
- **Umbral**: 2 caracteres de diferencia
- **Ejemplos**:
  - `dynamic_bost` → sugiere `dynamic_boost`
  - `cpu_max_per` → sugiere `cpu_max_perf`
  - `turbo_bost` → sugiere `turbo_boost`
- **Visualización**: Sugerencias en color amarillo

### **2. Validación Estricta**
- **Rangos**: Validación de valores dentro de límites seguros
- **Tipos**: Verificación de tipos de datos
- **Variables**: Validación de variables definidas
- **Errores**: Mensajes informativos y aborto seguro

### **3. Sistema de Variables**
- **Tipos**: Números y strings
- **Validación**: Conflictos de tipos detectados
- **Sobrescritura**: Confirmación del usuario
- **Uso**: Variables en configuraciones

### **4. Comandos del Sistema Real**
- **nvidia-smi**: Monitoreo de GPU en tiempo real
- **Intel P-State**: Control de rendimiento de CPU
- **LenovoLegionLinux**: Funciones específicas de Lenovo
- **Persistence Mode**: Configuración persistente

## 📊 **ESTADO ACTUAL DEL PROYECTO**

### **✅ FUNCIONALIDADES IMPLEMENTADAS**

#### **Core del Lenguaje**
- ✅ Lexer funcional
- ✅ Parser con AST
- ✅ Interpreter completo
- ✅ Sistema de variables
- ✅ Manejo de errores

#### **Características Avanzadas**
- ✅ Fuzzy matching con sugerencias
- ✅ Validación estricta de rangos
- ✅ Colores ANSI
- ✅ Comentarios en archivos
- ✅ Comandos CLI directos

#### **Integración del Sistema**
- ✅ Monitoreo real de GPU
- ✅ Parámetros reales implementados
- ✅ Comandos del sistema funcionales
- ✅ Test suite completo
- ✅ Comando `run mode:X` completamente funcional

### **🧪 PRUEBAS VERIFICADAS**
- ✅ `test_fuzzy_match.gx` - Fuzzy matching
- ✅ `test_error_rango_gpu.gx` - Validación de rangos
- ✅ `test_variables.gx` - Sistema de variables
- ✅ `test_todos_parametros.gx` - Todos los parámetros
- ✅ `test_comentarios.gx` - Comentarios
- ✅ `test_error_variable_no_definida.gx` - Errores de variables
- ✅ `test_conflicto_tipos.gx` - Conflictos de tipos

### **📈 MÉTRICAS DE CALIDAD**
- **Cobertura de pruebas**: 100% de funcionalidades principales
- **Manejo de errores**: Error crítico para valores inválidos
- **Experiencia de usuario**: Sugerencias inteligentes en color
- **Robustez**: Validación estricta de tipos y rangos
- **Documentación**: Ayuda completa y comentarios

## 🚀 **ROADMAP Y PRÓXIMOS PASOS**

### **✅ Fase 1: Comando `run mode:X` (COMPLETADO)**
```bash
gx run mode:quiet      # ✅ Aplicar modo quiet
gx run mode:balanced   # ✅ Aplicar modo balanced
gx run mode:performance # ✅ Aplicar modo performance
```

**✅ Implementación Completada**:
- ✅ Cargar configuraciones desde `modelo.txt`
- ✅ Aplicar comandos reales del sistema
- ✅ Feedback en tiempo real
- ✅ Manejo de errores
- ✅ Fuzzy matching para correcciones
- ✅ Soporte desde línea de comandos y archivos

### **Fase 2: GUI (Futuro)**
- **Interfaz gráfica** similar a Lenovo Vantage
- **Selección visual** de modos
- **Monitoreo en tiempo real**
- **Configuración personalizada**

### **Fase 3: Automatización Avanzada**
- **Scripts de inicio** automático
- **Perfiles por aplicación**
- **Monitoreo continuo**
- **Alertas y notificaciones**

## 🎯 **CASOS DE USO**

### **1. Usuario Casual**
```bash
# Cambio rápido de modo
gx run mode:performance  # Para gaming
gx run mode:quiet        # Para trabajo
gx run mode:balanced     # Para uso general
```

### **2. Usuario Avanzado**
```glx
# Configuración personalizada
mi_gaming_perf = 90
mi_gaming_boost = 1

dynamic_boost: mi_gaming_boost
cpu_max_perf: mi_gaming_perf
cpu_min_perf: 60
turbo_boost: 0
persist_mode: 1
battery_conservation: 0
fnlock: 1
```

### **3. Automatización**
```bash
# Script de inicio
#!/bin/bash
gx run mode:balanced
# Otras configuraciones del sistema
```

## 🔬 **INVESTIGACIÓN TÉCNICA**

### **Herramientas Exploradas**
- ✅ **nvidia-smi**: Monitoreo y persistence mode
- ✅ **Intel P-State**: Control de CPU performance
- ✅ **LenovoLegionLinux**: Funciones específicas de Lenovo
- ❌ **Power Limit**: No soportado en laptops
- ❌ **Fan Control**: No disponible en laptops
- ❌ **Clock Control**: No soportado en laptops

### **Comandos Reales Implementados**
```bash
# Dynamic Boost
echo 1 | sudo tee /sys/devices/system/cpu/intel_pstate/hwp_dynamic_boost

# CPU Performance
echo 80 | sudo tee /sys/devices/system/cpu/intel_pstate/max_perf_pct
echo 40 | sudo tee /sys/devices/system/cpu/intel_pstate/min_perf_pct

# Turbo Boost
echo 0 | sudo tee /sys/devices/system/cpu/intel_pstate/no_turbo

# Persistence Mode
sudo nvidia-smi -pm 1

# LenovoLegionLinux
sudo legion_cli --donotexpecthwmon batteryconservation-enable
sudo legion_cli --donotexpecthwmon fnlock-enable
```

## 🎨 **FILOSOFÍA DEL PROYECTO**

### **Principios de Diseño**
1. **Simplicidad**: Sintaxis clara y fácil de aprender
2. **Robustez**: Validación estricta y manejo de errores
3. **Flexibilidad**: Variables y programación dinámica
4. **Usabilidad**: Sugerencias inteligentes y feedback claro
5. **Integración**: Control real del sistema

### **Valores del Proyecto**
- **Educativo**: Aprender sobre control de sistemas
- **Práctico**: Solución real para control de rendimiento
- **Extensible**: Base sólida para futuras mejoras
- **Comunitario**: Código abierto y documentado

## 📚 **DOCUMENTACIÓN**

### **Archivos Clave**
- `INVESTIGACION_COMPLETA_GPU.md`: Investigación técnica detallada
- `README_TESTS_ACTUALIZADOS.md`: Documentación de pruebas
- `modelo.txt`: Configuraciones de modos
- `gx_pruebas/`: Suite completa de pruebas

### **Comandos de Ayuda**
```bash
gx help                    # Ayuda completa
gx status                  # Estado del sistema
gx vars                    # Variables definidas
```

## 🎯 **CONCLUSIÓN**

**GLX** representa una solución completa y elegante para el control de rendimiento en sistemas Linux con GPU NVIDIA. El proyecto combina:

- **Lenguaje de programación** robusto y fácil de usar
- **Control real del sistema** con parámetros verificados
- **Experiencia de usuario** avanzada con fuzzy matching
- **Arquitectura sólida** preparada para futuras expansiones

**✅ FUNCIONALIDAD PRINCIPAL COMPLETADA**: El comando `gx run mode:X` está completamente implementado y funcional, permitiendo aplicar configuraciones reales del sistema desde la línea de comandos y archivos GLX.

**🚀 PRÓXIMOS PASOS**: El proyecto está listo para futuras mejoras como interfaz gráfica y automatización avanzada.

---
**Fecha**: 30 de Julio 2025
**Estado**: ✅ Proyecto funcional - Comando run mode:X implementado y verificado
**GPU**: NVIDIA GeForce RTX 3050 6GB Laptop GPU
**Sistema**: Arch Linux con Lenovo LOQ 