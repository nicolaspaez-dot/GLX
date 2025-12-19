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

# Verificar dependencias del sistema
echo "Verificando dependencias del sistema..."

# Función para detectar el gestor de paquetes
detect_package_manager() {
    if command -v pacman >/dev/null 2>&1; then
        echo "pacman"
    elif command -v apt >/dev/null 2>&1; then
        echo "apt"
    elif command -v dnf >/dev/null 2>&1; then
        echo "dnf"
    else
        echo "unknown"
    fi
}

# Función para instalar paquetes
install_package() {
    local package=$1
    local pkg_manager=$(detect_package_manager)
    
    case $pkg_manager in
        "pacman")
            echo "Instalando $package con pacman..."
            sudo pacman -S --noconfirm $package
            ;;
        "apt")
            echo "Instalando $package con apt..."
            sudo apt update && sudo apt install -y $package
            ;;
        "dnf")
            echo "Instalando $package con dnf..."
            sudo dnf install -y $package
            ;;
        *)
            echo "No se pudo determinar el gestor de paquetes"
            return 1
            ;;
    esac
}

# Verificar e instalar gcc
if ! command -v gcc >/dev/null 2>&1; then
    echo "gcc no está instalado"
    echo "Instalando gcc..."
    install_package "gcc"
    if [ $? -ne 0 ]; then
        echo "Error: No se pudo instalar gcc"
        exit 1
    fi
else
    echo "gcc ya está instalado"
fi

# Verificar e instalar make
if ! command -v make >/dev/null 2>&1; then
    echo "make no está instalado"
    echo "Instalando make..."
    install_package "make"
    if [ $? -ne 0 ]; then
        echo "Error: No se pudo instalar make"
        exit 1
    fi
else
    echo "make ya está instalado"
fi

# Verificar nvidia-smi y nvidia-settings (para GPUs NVIDIA)
if ! command -v nvidia-smi >/dev/null 2>&1; then
    echo "nvidia-smi no está disponible"
    echo "Instalando drivers NVIDIA..."
    pkg_manager=$(detect_package_manager)
    case $pkg_manager in
        "pacman")
            sudo pacman -S --noconfirm nvidia nvidia-utils nvidia-settings
            ;;
        "apt")
            sudo apt update && sudo apt install -y nvidia-driver nvidia-utils nvidia-settings
            ;;
        "dnf")
            sudo dnf install -y nvidia-driver nvidia-utils nvidia-settings
            ;;
    esac
    echo "Es posible que necesites reiniciar para que los drivers NVIDIA funcionen"
else
    echo "nvidia-smi ya está instalado"
fi

# Verificar nvidia-settings específicamente
if ! command -v nvidia-settings >/dev/null 2>&1; then
    echo "nvidia-settings no está disponible"
    echo "Instalando nvidia-settings..."
    pkg_manager=$(detect_package_manager)
    case $pkg_manager in
        "pacman")
            sudo pacman -S --noconfirm nvidia-settings
            ;;
        "apt")
            sudo apt install -y nvidia-settings
            ;;
        "dnf")
            sudo dnf install -y nvidia-settings
            ;;
    esac
else
    echo "nvidia-settings ya está instalado"
fi

# Verificar legion_cli (para laptops Lenovo)
if ! command -v legion_cli >/dev/null 2>&1; then
    echo "legion_cli no está disponible"
    echo "Intentando instalar LenovoLegionLinux..."
    pkg_manager=$(detect_package_manager)
    case $pkg_manager in
        "pacman")
            # Verificar si yay está disponible
            if command -v yay >/dev/null 2>&1; then
                echo "Instalando LenovoLegionLinux con yay..."
                sudo pacman -S --noconfirm linux-headers python-pillow
                yay -S --noconfirm lenovolegionlinux-git
            else
                echo "yay no está disponible. Instala manualmente:"
                echo "   sudo pacman -S linux-headers python-pillow"
                echo "   yay -S lenovolegionlinux-git"
            fi
            ;;
        "apt")
            echo "Instalando dependencias de python..."
            sudo apt install -y python3-pil
            echo "LenovoLegionLinux debe instalarse manualmente desde GitHub"
            echo "   Visita: https://github.com/johnfanv2/LenovoLegionLinux"
            ;;
        "dnf")
            echo "Instalando dependencias de python..."
            sudo dnf install -y python3-pillow
            echo "LenovoLegionLinux debe instalarse manualmente desde GitHub"
            echo "   Visita: https://github.com/johnfanv2/LenovoLegionLinux"
            ;;
    esac
    echo "Algunas funciones específicas de Lenovo pueden no funcionar si no instalaste lenovolegionlinux"
else
    echo "legion_cli ya está instalado"
    # Verificar dependencias de python incluso si legion_cli está instalado
    if ! python3 -c "import PIL" >/dev/null 2>&1; then
        echo "Faltan dependencias de python (pillow). Instalando..."
        pkg_manager=$(detect_package_manager)
        case $pkg_manager in
            "pacman") sudo pacman -S --noconfirm python-pillow ;;
            "apt") sudo apt install -y python3-pil ;;
            "dnf") sudo dnf install -y python3-pillow ;;
        esac
    fi
fi

# Verificar archivos del sistema necesarios
echo "Verificando archivos del sistema..."

# Verificar intel_pstate
if [ ! -d "/sys/devices/system/cpu/intel_pstate" ]; then
    echo "/sys/devices/system/cpu/intel_pstate no está disponible"
    echo "   Esto puede indicar que no tienes un CPU Intel con P-State"
    echo "   Las funciones de control de CPU pueden no funcionar"
else
    echo "Intel P-State disponible"
fi

# Verificar platform-profile (para Lenovo)
if [ ! -f "/sys/devices/pci0000:00/0000:00:1f.0/PNP0C09:00/platform-profile/platform-profile-0/profile" ]; then
    echo "platform-profile no está disponible"
    echo "   El control del color del botón de encendido puede no funcionar"
else
    echo "platform-profile disponible"
fi

echo "Dependencias verificadas. Continuando con la instalación..."

# Crear directorio build si no existe
mkdir -p build

# Compilar el proyecto
echo "Compilando GLX..."
make clean
make

if [ $? -ne 0 ]; then
    echo "Error: La compilación falló"
    echo "Verifica que tienes todas las dependencias instaladas"
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
    echo ""
    echo "Nota: Algunas funciones pueden requerir permisos sudo"
    echo ""
    echo "Si instalaste drivers NVIDIA, considera reiniciar el sistema"
else
    echo "Error: La instalación no se completó correctamente"
    exit 1
fi 