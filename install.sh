#!/bin/bash

echo "Instalando GLX - GPU Linux Extension..."

# Verificar si estamos en el directorio correcto
if [ ! -f "src/main.c" ]; then
    echo "Error: No se encontró src/main.c"
    echo "Asegúrate de estar en el directorio raíz del proyecto GLX"
    exit 1
fi

# Verificar si modelo.txt existe
if [ ! -f "modelo.txt" ]; then
    echo "Error: No se encontró modelo.txt"
    echo "Asegúrate de que modelo.txt esté en el directorio raíz"
    exit 1
fi

# Crear directorio build si no existe
mkdir -p build

# Compilar el proyecto
echo "Compilando GLX..."
make clean
make

if [ $? -ne 0 ]; then
    echo "Error: La compilación falló"
    exit 1
fi

# Verificar que el ejecutable se creó
if [ ! -f "build/gx" ]; then
    echo "Error: No se pudo crear el ejecutable build/gx"
    exit 1
fi

# Crear directorio para archivos de configuración
sudo mkdir -p /usr/local/share/glx

# Copiar el ejecutable a /usr/local/bin
echo "Instalando ejecutable en /usr/local/bin/gx..."
sudo cp build/gx /usr/local/bin/gx

# Copiar modelo.txt a /usr/local/share/glx
echo "Instalando configuración en /usr/local/share/glx/modelo.txt..."
sudo cp modelo.txt /usr/local/share/glx/modelo.txt

# Dar permisos de ejecución
sudo chmod +x /usr/local/bin/gx

echo "Verificando instalación..."

# Verificar que se puede ejecutar desde cualquier lugar
if command -v gx >/dev/null 2>&1; then
    echo "GLX instalado exitosamente!"
    echo ""
    echo "Uso:"
    echo "  gx status                    - Ver estado del sistema"
    echo "  gx run mode:quiet           - Aplicar modo silencioso"
    echo "  gx run mode:balanced        - Aplicar modo equilibrado"
    echo "  gx run mode:performance     - Aplicar modo rendimiento"
    echo "  gx help                     - Ver ayuda"
    echo ""
    echo "Ejemplo: gx run mode:quiet"
else
    echo "Error: La instalación no se completó correctamente"
    exit 1
fi 