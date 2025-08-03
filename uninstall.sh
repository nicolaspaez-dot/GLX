#!/bin/bash

echo "Desinstalando GLX - GPU Linux Extension..."

# Verificar si GLX está instalado
if [ ! -f "/usr/local/bin/gx" ]; then
    echo "GLX no está instalado en /usr/local/bin/gx"
    exit 0
fi

# Remover el ejecutable
echo "Removiendo ejecutable..."
sudo rm -f /usr/local/bin/gx

# Remover archivos de configuración
echo "Removiendo archivos de configuración..."
sudo rm -rf /usr/local/share/glx

# Verificar que se removió completamente
if [ ! -f "/usr/local/bin/gx" ] && [ ! -d "/usr/local/share/glx" ]; then
    echo "GLX desinstalado exitosamente!"
else
    echo "Advertencia: Algunos archivos podrían no haberse removido completamente"
    echo "Verifica manualmente:"
    echo "  /usr/local/bin/gx"
    echo "  /usr/local/share/glx/"
fi 