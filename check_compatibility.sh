#!/bin/bash

echo "=== VERIFICACIÓN DE COMPATIBILIDAD GLX ==="
echo ""

# Información del sistema
echo "INFORMACIÓN DEL SISTEMA:"
echo "   Distribución: $(cat /etc/os-release | grep PRETTY_NAME | cut -d'"' -f2)"
echo "   Kernel: $(uname -r)"
echo "   Arquitectura: $(uname -m)"
echo ""

# Verificar CPU
echo "CPU:"
if [ -d "/sys/devices/system/cpu/intel_pstate" ]; then
    echo "   Intel P-State disponible"
    echo "   CPU detectada: $(cat /proc/cpuinfo | grep 'model name' | head -1 | cut -d':' -f2 | sed 's/^[ \t]*//')"
else
    echo "   Intel P-State NO disponible"
    echo "   Las funciones de control de CPU pueden no funcionar"
fi
echo ""

# Verificar GPU NVIDIA
echo "GPU NVIDIA:"
if command -v nvidia-smi >/dev/null 2>&1; then
    echo "   nvidia-smi disponible"
    gpu_info=$(nvidia-smi --query-gpu=name --format=csv,noheader,nounits 2>/dev/null | head -1)
    echo "   GPU detectada: $gpu_info"
else
    echo "   nvidia-smi NO disponible"
    echo "   Las funciones de GPU pueden no funcionar"
fi
echo ""

# Verificar Lenovo Legion
echo "LAPTOP LENOVO LEGION:"
if command -v legion_cli >/dev/null 2>&1; then
    echo "   legion_cli disponible"
    # Verificar si funciona (dependencias de python)
    if python3 -c "import PIL" >/dev/null 2>&1; then
        echo "   Dependencias python (pillow): OK"
        echo "   Funciones específicas de Lenovo disponibles"
    else
        echo "   ⚠️ ERROR: Faltan dependencias de python (pillow)"
        echo "   Las funciones de batería fallarán. Instala: sudo pacman -S python-pillow"
    fi
elif [ -f "/sys/devices/pci0000:00/0000:00:1f.0/PNP0C09:00/platform-profile/platform-profile-0/profile" ]; then
    echo "   platform-profile disponible pero legion_cli no instalado"
    echo "   Instala LenovoLegionLinux para funcionalidad completa:"
    echo "      yay -S lenovolegionlinux-git"
else
    echo "   No se detectó laptop Lenovo Legion"
    echo "   Las funciones específicas de Lenovo no estarán disponibles"
fi
echo ""

# Verificar herramientas de compilación
echo "HERRAMIENTAS DE COMPILACIÓN:"
if command -v gcc >/dev/null 2>&1; then
    echo "   gcc disponible: $(gcc --version | head -1)"
else
    echo "   gcc NO disponible"
fi

if command -v make >/dev/null 2>&1; then
    echo "   make disponible: $(make --version | head -1)"
else
    echo "   make NO disponible"
fi
echo ""

# Verificar permisos sudo
echo "PERMISOS:"
if sudo -n true 2>/dev/null; then
    echo "   Permisos sudo disponibles"
else
    echo "   Permisos sudo requeridos para algunas funciones"
fi
echo ""

# Resumen de compatibilidad
echo "RESUMEN DE COMPATIBILIDAD:"
echo ""

compatibility_score=0
total_checks=5

# CPU check
if [ -d "/sys/devices/system/cpu/intel_pstate" ]; then
    ((compatibility_score++))
    echo "   CPU Intel con P-State"
else
    echo "   CPU Intel con P-State"
fi

# GPU check
if command -v nvidia-smi >/dev/null 2>&1; then
    ((compatibility_score++))
    echo "   GPU NVIDIA"
else
    echo "   GPU NVIDIA"
fi

# Lenovo check
if command -v legion_cli >/dev/null 2>&1 || [ -f "/sys/devices/pci0000:00/0000:00:1f.0/PNP0C09:00/platform-profile/platform-profile-0/profile" ]; then
    ((compatibility_score++))
    echo "   Laptop Lenovo Legion"
else
    echo "   Laptop Lenovo Legion"
fi

# Compilation tools check
if command -v gcc >/dev/null 2>&1 && command -v make >/dev/null 2>&1; then
    ((compatibility_score++))
    echo "    Herramientas de compilación"
else
    echo "    Herramientas de compilación"
fi

# Permissions check
if sudo -n true 2>/dev/null; then
    ((compatibility_score++))
    echo "    Permisos sudo"
else
    echo "     Permisos sudo (requeridos)"
fi

echo ""
echo " PUNTUACIÓN DE COMPATIBILIDAD: $compatibility_score/$total_checks"

if [ $compatibility_score -eq $total_checks ]; then
    echo " ¡COMPATIBILIDAD PERFECTA! GLX funcionará completamente."
elif [ $compatibility_score -ge 3 ]; then
    echo " COMPATIBILIDAD BUENA. GLX funcionará con algunas limitaciones."
elif [ $compatibility_score -ge 2 ]; then
    echo "  COMPATIBILIDAD LIMITADA. Algunas funciones no estarán disponibles."
else
    echo " COMPATIBILIDAD BAJA. GLX puede no funcionar correctamente."
fi

echo ""
echo "💡 RECOMENDACIONES:"
if [ ! -d "/sys/devices/system/cpu/intel_pstate" ]; then
    echo "   - Instala un CPU Intel con soporte P-State"
fi
if ! command -v nvidia-smi >/dev/null 2>&1; then
    echo "   - Instala drivers NVIDIA: sudo pacman -S nvidia nvidia-utils"
fi
if ! command -v legion_cli >/dev/null 2>&1; then
    echo "   - Para laptops Lenovo: yay -S lenovolegionlinux-git"
fi
if ! command -v gcc >/dev/null 2>&1; then
    echo "   - Instala gcc: sudo pacman -S gcc"
fi
if ! command -v make >/dev/null 2>&1; then
    echo "   - Instala make: sudo pacman -S make"
fi
