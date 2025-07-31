#!/bin/bash

# Script de desinstalación para GLX

echo "🗑️  Desinstalando GLX - GPU Linux Extension..."

# Verificar si GLX está instalado
if [ ! -f "/usr/local/bin/gx" ]; then
    echo "❌ GLX no está instalado en el sistema"
    exit 1
fi

# Eliminar el ejecutable
echo "📋 Eliminando /usr/local/bin/gx..."
sudo rm -f /usr/local/bin/gx

# Eliminar modelo.txt
echo "📋 Eliminando /usr/local/share/glx/modelo.txt..."
sudo rm -f /usr/local/share/glx/modelo.txt

# Eliminar directorio si está vacío
if [ -d "/usr/local/share/glx" ]; then
    sudo rmdir /usr/local/share/glx 2>/dev/null || echo "📁 Directorio /usr/local/share/glx no está vacío, se mantiene"
fi

echo "✅ GLX desinstalado exitosamente!"
echo ""
echo "📋 Archivos eliminados:"
echo "   /usr/local/bin/gx"
echo "   /usr/local/share/glx/modelo.txt" 