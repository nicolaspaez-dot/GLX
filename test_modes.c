#include <stdio.h>
#include <stdlib.h>
#include "include/utils.h"

int main() {
    printf("🧪 Probando carga de modos GPU...\n\n");
    
    int num_modes;
    GPU_Mode* modes = load_gpu_modes("../modelo.txt", &num_modes);
    
    if (modes) {
        printf("\n📊 Resumen de modos cargados:\n");
        for (int i = 0; i < num_modes; i++) {
            printf("\n🎯 Modo: %s\n", modes[i].name);
            printf("   Power Limit: %dW\n", modes[i].power_limit);
            printf("   Fan Speed: %d%%\n", modes[i].fan_speed);
            printf("   Clocks: %s MHz\n", modes[i].clocks);
            printf("   Persist Mode: %s\n", modes[i].persist_mode ? "ON" : "OFF");
        }
        
        free(modes);
    } else {
        printf("❌ Error al cargar modos\n");
    }
    
    return 0;
} 