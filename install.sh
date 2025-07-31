#!/bin/bash

# Script de instalación para GLX
# Hace que GLX esté disponible globalmente como comando

echo "🚀 Instalando GLX - GPU Linux Extension..."

# Verificar si estamos en el directorio correcto
if [ ! -f "src/main.c" ]; then
    echo "❌ Error: Debes ejecutar este script desde el directorio nico_lang/"
    exit 1
fi

# Compilar el proyecto
echo "📦 Compilando GLX..."
make clean
make

if [ $? -ne 0 ]; then
    echo "❌ Error: La compilación falló"
    exit 1
fi

# Verificar que el ejecutable existe
if [ ! -f "build/gx" ]; then
    echo "❌ Error: No se encontró el ejecutable build/gx"
    exit 1
fi

# Verificar que modelo.txt existe
if [ ! -f "../modelo.txt" ]; then
    echo "❌ Error: No se encontró el archivo ../modelo.txt"
    exit 1
fi

# Crear directorio de instalación si no existe
echo "📁 Creando directorio de instalación..."
sudo mkdir -p /usr/local/bin
sudo mkdir -p /usr/local/share/glx

# Copiar el ejecutable
echo "📋 Copiando GLX a /usr/local/bin/gx..."
sudo cp build/gx /usr/local/bin/gx

# Copiar modelo.txt
echo "📋 Copiando modelo.txt a /usr/local/share/glx/modelo.txt..."
sudo cp ../modelo.txt /usr/local/share/glx/modelo.txt

# Dar permisos de ejecución
echo "🔐 Configurando permisos..."
sudo chmod +x /usr/local/bin/gx
sudo chmod 644 /usr/local/share/glx/modelo.txt

# Verificar la instalación
echo "✅ Verificando instalación..."
if command -v gx >/dev/null 2>&1; then
    echo "🎉 ¡GLX instalado exitosamente!"
    echo ""
    echo "📋 Comandos disponibles:"
    echo "   gx help                    - Mostrar ayuda"
    echo "   gx status                  - Estado del sistema"
    echo "   gx vars                    - Variables definidas"
    echo "   gx run mode:quiet          - Aplicar modo quiet"
    echo "   gx run mode:balanced       - Aplicar modo balanced"
    echo "   gx run mode:performance    - Aplicar modo performance"
    echo "   gx archivo.gx              - Ejecutar archivo GLX"
    echo ""
    echo "🌍 Ahora puedes usar GLX desde cualquier directorio!"
    echo "📁 Archivos instalados:"
    echo "   /usr/local/bin/gx"
    echo "   /usr/local/share/glx/modelo.txt"
else
    echo "❌ Error: La instalación no se completó correctamente"
    exit 1
fi 