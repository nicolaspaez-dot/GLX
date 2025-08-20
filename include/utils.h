#ifndef UTILS_H
#define UTILS_H


// Listas de palabras válidas para fuzzy match
extern const char* modos_validos[];
extern const int num_modos;
extern const char* parametros_validos[];
extern const int num_parametros;
extern const char* comandos_gpu_validos[];
extern const int num_comandos_gpu;

int levenshtein(const char* s1, const char* s2);
const char* sugerir_palabra(const char* palabra, const char** lista, int cantidad, int max_distancia);

// Función para ejecutar comandos del sistema y capturar su salida
char* execute_system_command(const char* command);

typedef struct {
    char name[50];
    int dynamic_boost;
    int cpu_max_perf;
    int cpu_min_perf;
    int turbo_boost;
    int persist_mode;
    int battery_conservation;
    int fnlock;
    char rgb_color[20];      // Color RGB: "blue", "white", "red"
    int rgb_brightness;      // Brillo RGB: 0-100
} GPU_Mode;

GPU_Mode* load_gpu_modes(const char* filename, int* num_modes);

// Función para controlar RGB del teclado
int set_rgb_color(const char* color, int brightness);

// Función para obtener el color actual del botón de encendido
const char* get_current_power_button_color(void);

#endif // UTILS_H