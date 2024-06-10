#include <stdio.h>
#include <string.h>

// Estructuras para las canciones
typedef struct {
    int minutos;
    int segundos;
} Duracion;

typedef struct {
    char artista[100];
    char titulo[100];
    Duracion duracion;  // Duración con minutos y segundos
    float tamano;  // en KB
} Cancion;

// Estructura para las imágenes
typedef struct {
    char nombre[100];
    int ancho;  // en píxeles
    int alto;   // en píxeles
    float tamano;  // en KB
} Imagen;

// Estructura para datos de una persona
typedef struct {
    char nombre[50];
    char apellido[50];
    int edad;
} Persona;

// Estructura para datos de una persona con fecha de nacimiento
typedef struct {
    int dia;
    int mes;
    int ano;
} Fecha;

typedef struct {
    char nombre[50];
    Fecha nacimiento;
} PersonaConFecha;

// Funciones para la gestión de canciones
void agregarCancion(Cancion canciones[], int *contador) {
    if (*contador >= 100) {
        printf("La colección de canciones está llena.\n");
        return;
    }

    printf("Ingrese el artista: ");
    getchar();  // Limpiar el buffer
    fgets(canciones[*contador].artista, 100, stdin);
    canciones[*contador].artista[strcspn(canciones[*contador].artista, "\n")] = 0;

    printf("Ingrese el título: ");
    fgets(canciones[*contador].titulo, 100, stdin);
    canciones[*contador].titulo[strcspn(canciones[*contador].titulo, "\n")] = 0;

    printf("Ingrese la duración en minutos y segundos (ej. 3 45): ");
    scanf("%d %d", &canciones[*contador].duracion.minutos, &canciones[*contador].duracion.segundos);

    printf("Ingrese el tamaño del fichero en KB: ");
    scanf("%f", &canciones[*contador].tamano);

    (*contador)++;
}

void mostrarTitulosCanciones(Cancion canciones[], int contador) {
    for (int i = 0; i < contador; i++) {
        printf("Título: %s\n", canciones[i].titulo);
    }
}

void buscarCancion(Cancion canciones[], int contador, char *texto) {
    for (int i = 0; i < contador; i++) {
        if (strstr(canciones[i].artista, texto) != NULL || strstr(canciones[i].titulo, texto) != NULL) {
            printf("Canción encontrada:\nArtista: %s\nTítulo: %s\nDuración: %d:%d\nTamaño: %.2f KB\n",
                   canciones[i].artista, canciones[i].titulo, canciones[i].duracion.minutos, canciones[i].duracion.segundos, canciones[i].tamano);
        }
    }
}

// Funciones para la gestión de imágenes
void agregarImagen(Imagen imagenes[], int *contador) {
    if (*contador >= 700) {
        printf("La capacidad de almacenamiento de imágenes está llena.\n");
        return;
    }

    printf("Ingrese el nombre: ");
    getchar();  // Limpiar el buffer
    fgets(imagenes[*contador].nombre, 100, stdin);
    imagenes[*contador].nombre[strcspn(imagenes[*contador].nombre, "\n")] = 0;

    printf("Ingrese el ancho en píxeles: ");
    scanf("%d", &imagenes[*contador].ancho);

    printf("Ingrese el alto en píxeles: ");
    scanf("%d", &imagenes[*contador].alto);

    printf("Ingrese el tamaño en KB: ");
    scanf("%f", &imagenes[*contador].tamano);

    (*contador)++;
}

void verTodasLasFichas(Imagen imagenes[], int contador) {
    for (int i = 0; i < contador; i++) {
        printf("%d. Nombre: %s\n", i + 1, imagenes[i].nombre);
    }
}

void buscarFicha(Imagen imagenes[], int contador, char *nombre) {
    for (int i = 0; i < contador; i++) {
        if (strstr(imagenes[i].nombre, nombre) != NULL) {
            printf("Ficha encontrada:\nNombre: %s\nAncho: %d píxeles\nAlto: %d píxeles\nTamaño: %.2f KB\n",
                   imagenes[i].nombre, imagenes[i].ancho, imagenes[i].alto, imagenes[i].tamano);
        }
    }
}

// Función para mostrar datos de una persona
void mostrarPersona(Persona persona) {
    printf("%s, %s, %d\n", persona.nombre, persona.apellido, persona.edad);
}

// Funciones para la gestión de personas con fecha de nacimiento
void imprimirPersonaConFecha(PersonaConFecha persona) {
    printf("%s, Nacimiento: %02d/%02d/%04d\n", persona.nombre, persona.nacimiento.dia, persona.nacimiento.mes, persona.nacimiento.ano);
}

int main() {
    // Variables y arreglos para cada funcionalidad
    Cancion canciones[100];
    int contadorCanciones = 0;
    
    Imagen imagenes[700];
    int contadorImagenes = 0;
    
    Persona persona;
    
    PersonaConFecha personas[8];
    int mesBusqueda;

    int opcionPrincipal;
    int opcionSubmenu;

    do {
        printf("\nMenu Principal:\n1. Gestionar canciones\n2. Gestionar imágenes\n3. Almacenar datos de una persona\n4. Gestionar datos de 8 personas con búsqueda por mes de nacimiento\n5. Salir\nOpción: ");
        scanf("%d", &opcionPrincipal);

        switch (opcionPrincipal) {
            case 1:
                do {
                    printf("\nMenu Canciones:\n1. Añadir nueva canción\n2. Mostrar títulos de todas las canciones\n3. Buscar canción\n4. Volver al menú principal\nOpción: ");
                    scanf("%d", &opcionSubmenu);

                    switch (opcionSubmenu) {
                        case 1:
                            agregarCancion(canciones, &contadorCanciones);
                            break;
                        case 2:
                            mostrarTitulosCanciones(canciones, contadorCanciones);
                            break;
                        case 3:
                            {
                                char texto[100];
                                printf("Ingrese el texto para buscar: ");
                                getchar();  // Limpiar el buffer
                                fgets(texto, 100, stdin);
                                texto[strcspn(texto, "\n")] = 0;
                                buscarCancion(canciones, contadorCanciones, texto);
                            }
                            break;
                        case 4:
                            printf("Volviendo al menú principal...\n");
                            break;
                        default:
                            printf("Opción no válida.\n");
                    }
                } while (opcionSubmenu != 4);
                break;

            case 2:
                do {
                    printf("\nMenu Imágenes:\n1. Añadir nueva ficha\n2. Ver todas las fichas\n3. Buscar ficha\n4. Volver al menú principal\nOpción: ");
                    scanf("%d", &opcionSubmenu);

                    switch (opcionSubmenu) {
                        case 1:
                            agregarImagen(imagenes, &contadorImagenes);
                            break;
                        case 2:
                            verTodasLasFichas(imagenes, contadorImagenes);
                            break;
                        case 3:
                            {
                                char nombre[100];
                                printf("Ingrese el nombre para buscar: ");
                                getchar();  // Limpiar el buffer
                                fgets(nombre, 100, stdin);
                                nombre[strcspn(nombre, "\n")] = 0;
                                buscarFicha(imagenes, contadorImagenes, nombre);
                            }
                            break;
                        case 4:
                            printf("Volviendo al menú principal...\n");
                            break;
                        default:
                            printf("Opción no válida.\n");
                    }
                } while (opcionSubmenu != 4);
                break;

            case 3:
                printf("Ingrese el nombre: ");
                getchar();  // Limpiar el buffer
                fgets(persona.nombre, 50, stdin);
                persona.nombre[strcspn(persona.nombre, "\n")] = 0;

                printf("Ingrese el apellido: ");
                fgets(persona.apellido, 50, stdin);
                persona.apellido[strcspn(persona.apellido, "\n")] = 0;

                printf("Ingrese la edad: ");
                scanf("%d", &persona.edad);

                mostrarPersona(persona);
                break;

            case 4:
                for (int i = 0; i < 8; i++) {
                    printf("Ingrese el nombre de la persona %d: ", i + 1);
                    getchar();  // Limpiar el buffer
                    fgets(personas[i].nombre, 50, stdin);
                    personas[i].nombre[strcspn(personas[i].nombre, "\n")] = 0;

                    printf("Ingrese el día de nacimiento: ");
                    scanf("%d", &personas[i].nacimiento.dia);

                    printf("Ingrese el mes de nacimiento: ");
                    scanf("%d", &personas[i].nacimiento.mes);

                    printf("Ingrese el año de nacimiento: ");
                    scanf("%d", &personas[i].nacimiento.ano);
                }

                do {
                    printf("Ingrese un número de mes para buscar (0 para salir): ");
                    scanf("%d", &mesBusqueda);

                    if (mesBusqueda == 0) {
                        printf("Saliendo...\n");
                        break;
                    }

                    printf("Personas que cumplen años en el mes %d:\n", mesBusqueda);
                    for (int i = 0; i < 8; i++) {
                        if (personas[i].nacimiento.mes == mesBusqueda) {
                            imprimirPersonaConFecha(personas[i]);
                        }
                    }

                } while (mesBusqueda != 0);
                break;

            case 5:
                printf("Saliendo del programa...\n");
                break;

            default:
                printf("Opción no válida.\n");
        }
    } while (opcionPrincipal != 5);

    return 0;
}
