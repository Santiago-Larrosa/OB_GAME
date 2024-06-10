#include <SDL2/SDL.h>
#include <stdbool.h>

// Definición de constantes
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int PADDLE_WIDTH = 20;
const int PADDLE_HEIGHT = 100;
const int BALL_SIZE = 20;
const int PADDLE_SPEED = 10;
const int BALL_SPEED = 5;

// Función principal
int main(int argc, char *argv[]) {
    // Inicialización de SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    // Creación de la ventana
    SDL_Window *window = SDL_CreateWindow("Pong",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SCREEN_WIDTH,
                                          SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Creación del renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "Could not create renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Definición de las posiciones de las paletas y la pelota
    SDL_Rect paddle1 = { 50, SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2, PADDLE_WIDTH, PADDLE_HEIGHT };
    SDL_Rect paddle2 = { SCREEN_WIDTH - 70, SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2, PADDLE_WIDTH, PADDLE_HEIGHT };
    SDL_Rect ball = { SCREEN_WIDTH / 2 - BALL_SIZE / 2, SCREEN_HEIGHT / 2 - BALL_SIZE / 2, BALL_SIZE, BALL_SIZE };

    int ballVelX = BALL_SPEED;
    int ballVelY = BALL_SPEED;

    bool running = true;
    SDL_Event event;

    // Bucle principal del juego
    while (running) {
        // Manejo de eventos
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        const Uint8 *state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_W]) {
            paddle1.y -= PADDLE_SPEED;
        }
        if (state[SDL_SCANCODE_S]) {
            paddle1.y += PADDLE_SPEED;
        }
        if (state[SDL_SCANCODE_UP]) {
            paddle2.y -= PADDLE_SPEED;
        }
        if (state[SDL_SCANCODE_DOWN]) {
            paddle2.y += PADDLE_SPEED;
        }

        // Movimiento de la pelota
        ball.x += ballVelX;
        ball.y += ballVelY;

        // Colisión con los bordes superior e inferior
        if (ball.y <= 0 || ball.y >= SCREEN_HEIGHT - BALL_SIZE) {
            ballVelY = -ballVelY;
        }

        // Colisión con las paletas
        if (SDL_HasIntersection(&ball, &paddle1) || SDL_HasIntersection(&ball, &paddle2)) {
            ballVelX = -ballVelX;
        }

        // Salida de la pelota por los lados izquierdo y derecho
        if (ball.x <= 0 || ball.x >= SCREEN_WIDTH - BALL_SIZE) {
            ball.x = SCREEN_WIDTH / 2 - BALL_SIZE / 2;
            ball.y = SCREEN_HEIGHT / 2 - BALL_SIZE / 2;
            ballVelX = BALL_SPEED;
            ballVelY = BALL_SPEED;
        }

        // Limpieza de la pantalla
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Renderizado de las paletas y la pelota
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &paddle1);
        SDL_RenderFillRect(renderer, &paddle2);
        SDL_RenderFillRect(renderer, &ball);

        // Actualización de la pantalla
        SDL_RenderPresent(renderer);

        // Control de la velocidad del juego
        SDL_Delay(16);
    }

    // Limpieza de SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
