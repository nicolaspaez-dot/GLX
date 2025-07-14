# 🚀 Guía de Git y GitHub para GLX

## 📋 **Comandos básicos de Git**

### **Verificar estado del repositorio:**
```bash
git status
```

### **Agregar archivos al staging area:**
```bash
git add .                    # Agregar todos los archivos
git add archivo.txt          # Agregar archivo específico
```

### **Hacer commit de los cambios:**
```bash
git commit -m "Descripción de los cambios"
```

### **Subir cambios a GitHub:**
```bash
git push origin feature/GLX
```

### **Ver historial de commits:**
```bash
git log --oneline
```

### **Ver diferencias:**
```bash
git diff                    # Ver cambios no agregados
git diff --staged          # Ver cambios en staging
```

## 🔄 **Flujo de trabajo típico:**

1. **Hacer cambios en el código**
2. **Verificar qué cambió:**
   ```bash
   git status
   ```
3. **Agregar cambios:**
   ```bash
   git add .
   ```
4. **Hacer commit:**
   ```bash
   git commit -m "Descripción clara de los cambios"
   ```
5. **Subir a GitHub:**
   ```bash
   git push origin feature/GLX
   ```

## 🌐 **Enlaces importantes:**

- **Repositorio en GitHub:** https://github.com/nicolaspaez-dot/GLX
- **Rama actual:** `feature/GLX`

## 📁 **Estructura del proyecto:**

```
nico_lang/
├── src/                    # Código fuente
│   ├── main.c             # Punto de entrada
│   ├── lexer.c            # Analizador léxico
│   ├── parser.c           # Analizador sintáctico
│   └── interpreter.c      # Intérprete
├── include/               # Archivos de encabezado
├── gx_programs/           # Archivos de ejemplo
├── build/                 # Ejecutables (no subir a Git)
├── Makefile               # Compilación
└── README.md              # Documentación principal
```

## ⚠️ **Archivos que NO se suben a Git:**

- `build/` (carpeta con ejecutables)
- Archivos temporales
- Archivos de configuración local

## 🎯 **Comandos útiles adicionales:**

### **Crear una nueva rama:**
```bash
git checkout -b nueva-funcionalidad
```

### **Cambiar de rama:**
```bash
git checkout main
git checkout feature/GLX
```

### **Ver todas las ramas:**
```bash
git branch -a
```

### **Actualizar repositorio local:**
```bash
git pull origin feature/GLX
```

## 💡 **Consejos:**

1. **Haz commits frecuentes** con mensajes descriptivos
2. **Usa `git status`** antes de hacer commit
3. **Prueba el código** antes de subir
4. **Usa mensajes de commit claros** como:
   - "Agregar sistema de variables"
   - "Mejorar parser para comentarios"
   - "Arreglar bug en interpreter"

## 🔧 **Si algo sale mal:**

### **Deshacer último commit:**
```bash
git reset --soft HEAD~1
```

### **Descartar cambios en un archivo:**
```bash
git checkout -- archivo.txt
```

### **Ver qué archivos están siendo rastreados:**
```bash
git ls-files
```

---

**📝 Nota:** Esta guía se actualiza automáticamente. Si agregas nuevas funcionalidades, actualiza esta documentación. 