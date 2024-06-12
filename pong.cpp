#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h> 
#include <stdio.h>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 600
#define SPRITE_WIDTH 250
#define SPRITE_HEIGHT 246
#define SPRITES_PER_IMAGE 4
#define PLATFORM_HEIGHT 20
#define FRAME_TIME 150
#define GRAVITY 0.01
#define JUMP_FORCE -2
#define PLATFORM_NUM  10

typedef struct {
    SDL_Rect position;
    double velocityY;
    double velocityX;
    int isJumping;
    int isPreparingToJump;
    int jumpFrame;
    int prepareJumpFrame;
    int isMoving;
    int isOnGround;
} Character;

typedef struct {
    int x, y;
} Camera;

int checkCollision(SDL_Rect rectA, SDL_Rect rectB) {
    return !(rectA.y + rectA.h <= rectB.y ||
             rectA.y >= rectB.y + rectB.h ||
             rectA.x + rectA.w <= rectB.x ||
             rectA.x >= rectB.x + rectB.w);
}

int main(int argc, char* args[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Texture* spriteSheet1 = NULL;
    SDL_Texture* spriteSheet2 = NULL;
    SDL_Texture* jumpSpriteSheet = NULL;
    SDL_Texture* fallingSpriteSheet = NULL; 
    SDL_Texture* platformTexture = NULL;
    SDL_Rect spriteClips[SPRITES_PER_IMAGE * 4];
    SDL_Rect platforms[PLATFORM_NUM]; 
    Mix_Music *backgroundMusic = NULL; 

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) { 
        printf("No se pudo inicializar SDL. SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("No se pudo inicializar SDL_image. SDL_image Error: %s\n", IMG_GetError());
        return -1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) { 
        printf("No se pudo inicializar SDL_mixer. Mix_Error: %s\n", Mix_GetError());
        return -1;
    }

    window = SDL_CreateWindow("Furnace KNIGH",
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              SCREEN_WIDTH, SCREEN_HEIGHT,
                              SDL_WINDOW_SHOWN);
    

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
   

    spriteSheet1 = IMG_LoadTexture(renderer, "spritesheet1.bmp");
   

    spriteSheet2 = IMG_LoadTexture(renderer, "spritesheet2.bmp");
   

    jumpSpriteSheet = IMG_LoadTexture(renderer, "jump_spritesheet.bmp");
   

    fallingSpriteSheet = IMG_LoadTexture(renderer, "falling.bmp"); 
  

    platformTexture = IMG_LoadTexture(renderer, "platform.png");
    

    for (int i = 0; i < SPRITES_PER_IMAGE; ++i) {
        spriteClips[i].x = i * SPRITE_WIDTH;
        spriteClips[i].y = 0;
        spriteClips[i].w = SPRITE_WIDTH;
        spriteClips[i].h = SPRITE_HEIGHT;
    }

    for (int i = 0; i < SPRITES_PER_IMAGE; ++i) {
        spriteClips[i + SPRITES_PER_IMAGE].x = i * SPRITE_WIDTH;
        spriteClips[i + SPRITES_PER_IMAGE].y = 0;
        spriteClips[i + SPRITES_PER_IMAGE].w = SPRITE_WIDTH;
        spriteClips[i + SPRITES_PER_IMAGE].h = SPRITE_HEIGHT;
    }

    for (int i = 0; i < SPRITES_PER_IMAGE; ++i) {
        spriteClips[i + 2 * SPRITES_PER_IMAGE].x = i * SPRITE_WIDTH;
        spriteClips[i + 2 * SPRITES_PER_IMAGE].y = 0;
        spriteClips[i + 2 * SPRITES_PER_IMAGE].w = SPRITE_WIDTH;
        spriteClips[i + 2 * SPRITES_PER_IMAGE].h = SPRITE_HEIGHT;
    }

    for (int i = 0; i < SPRITES_PER_IMAGE; ++i) {
        spriteClips[i + 3 * SPRITES_PER_IMAGE].x = i * SPRITE_WIDTH;
        spriteClips[i + 3 * SPRITES_PER_IMAGE].y = 0;
        spriteClips[i + 3 * SPRITES_PER_IMAGE].w = SPRITE_WIDTH;
        spriteClips[i + 3 * SPRITES_PER_IMAGE].h = SPRITE_HEIGHT;
    }

    Character player;
    player.position.w = SPRITE_WIDTH / 3;
    player.position.h = SPRITE_HEIGHT / 3;
    player.position.x = (SCREEN_WIDTH - SPRITE_WIDTH) / 2;
    player.position.y = SCREEN_HEIGHT - SPRITE_HEIGHT - PLATFORM_HEIGHT - 10;
    player.velocityY = 0;
    player.velocityX = 1;
    player.isJumping = 0;
    player.isPreparingToJump = 0;
    player.jumpFrame = 0;
    player.prepareJumpFrame = 0;
    player.isMoving = 0;
    player.isOnGround = 1; 

    Camera camera = {0-(SPRITE_WIDTH/6), 0};

    
    platforms[0] = (SDL_Rect){0, SCREEN_HEIGHT - PLATFORM_HEIGHT, SCREEN_WIDTH*10, PLATFORM_HEIGHT};
    platforms[1] = (SDL_Rect){200, SCREEN_HEIGHT - 200, 300, PLATFORM_HEIGHT};
    platforms[2] = (SDL_Rect){500, SCREEN_HEIGHT - 300, 300, PLATFORM_HEIGHT};
    platforms[3] = (SDL_Rect){800, SCREEN_HEIGHT - 400, 300, PLATFORM_HEIGHT};
    platforms[4] = (SDL_Rect){1100, SCREEN_HEIGHT - 200, 300, PLATFORM_HEIGHT};
    platforms[5] = (SDL_Rect){1400, SCREEN_HEIGHT - 300, 300, PLATFORM_HEIGHT};
    platforms[6] = (SDL_Rect){1700, SCREEN_HEIGHT - 100, 250, PLATFORM_HEIGHT};
    platforms[7] = (SDL_Rect){1900, SCREEN_HEIGHT - 300, 50, PLATFORM_HEIGHT};
    platforms[8] = (SDL_Rect){2300, SCREEN_HEIGHT - 400, 200, PLATFORM_HEIGHT};
    platforms[9] = (SDL_Rect){2500, SCREEN_HEIGHT - 100, 300, PLATFORM_HEIGHT};

    int currentFrame = 0;
    SDL_Event e;
    int quit = 0;
    Uint32 startTime = SDL_GetTicks();
    int isMoving = 0;
    int direction = 0;

   
    backgroundMusic = Mix_LoadMUS("background_music.mp3");
    if (backgroundMusic == NULL) {
        printf("No se pudo cargar la música de fondo. Mix_Error: %s\n", Mix_GetError());
        return -1;
    }

    // Reproducir la música de fondo en bucle
    if (Mix_PlayMusic(backgroundMusic, -1) == -1) {
        printf("No se pudo reproducir la música de fondo. Mix_Error: %s\n", Mix_GetError());
        return -1;
    }

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            } else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_LEFT:
                        if (!player.isPreparingToJump && !player.isJumping) {
                            direction = -1;
                            isMoving = 1;
                        }
                        break;
                    case SDLK_RIGHT:
                        if (!player.isPreparingToJump && !player.isJumping) {
                            direction = 1;
                            isMoving = 1;
                        }
                        break;
                    case SDLK_UP:
                        if (!player.isJumping && !player.isPreparingToJump) {
                            player.isPreparingToJump = 1;
                            player.prepareJumpFrame = 0;
                        }
                        break;
                    case SDLK_r:
                       player.position.y = 0;
                       player.position.x = 0;
                       player.isOnGround = 0;
                       player.isJumping = 0;
                       player.velocityY = -1;
                        break;
                }
                
            } else if (e.type == SDL_KEYUP) {
                switch (e.key.keysym.sym) {
                    case SDLK_LEFT:
                    case SDLK_RIGHT:
                        direction = 0;
                        isMoving = 0;
                        break;
                    case SDLK_UP:
                        if (player.isPreparingToJump) {
                            player.isPreparingToJump = 0;
                            player.isJumping = 1;
                            player.jumpFrame = 0;
                            player.velocityY = JUMP_FORCE;
                        }
                        break;
                }
            }
        }

        int wasFalling = player.velocityY > 0;
      
        if (player.isJumping) {
            player.velocityY += GRAVITY;
            player.position.y += player.velocityY;

            Uint32 currentTime = SDL_GetTicks();
            if (currentTime - startTime > FRAME_TIME) {
                player.jumpFrame = (player.jumpFrame + 1) % SPRITES_PER_IMAGE;
                startTime = currentTime;
            }

            for (int i = 0; i < PLATFORM_NUM; ++i) {
                if (checkCollision(player.position, platforms[i])) {
                    player.velocityY = 0;
                    player.position.y = platforms[i].y - player.position.h;
                    player.isJumping = 0;
                    player.isOnGround = 1;
                    break;
                }
            }
        } 
        else if (!player.isOnGround) {
            player.velocityY += GRAVITY;
            player.position.y += player.velocityY;

            for (int i = 0; i < PLATFORM_NUM; ++i) {
                if (checkCollision(player.position, platforms[i])) {
                    player.velocityY = 0;
                    player.position.y = platforms[i].y - player.position.h;
                    player.isOnGround = 1;
                    break;
                }
            }
        }

        if (isMoving && !player.isPreparingToJump) {
            player.position.x += direction * player.velocityX;
            camera.x += direction * player.velocityX;

            Uint32 currentTime = SDL_GetTicks();
            if (currentTime - startTime > FRAME_TIME) {
                currentFrame = (currentFrame + 1) % (SPRITES_PER_IMAGE * 2);
                startTime = currentTime;
            }
        } else {
            currentFrame = 0;
        }

        // Verificación del borde de la plataforma
        if (player.isOnGround) {
            int onPlatform = 0;
            for (int i = 0; i < PLATFORM_NUM; ++i) {
                if (checkCollision(player.position, platforms[i])) {
                    onPlatform = 1;
                    break;
                }
            }
            if (!onPlatform) {
                player.isOnGround = 0;
                
                player.velocityY = GRAVITY;
            }
        }

        if (player.position.x < SCREEN_WIDTH / 2) {
            camera.x = 0;
        } else {
            camera.x = player.position.x - SCREEN_WIDTH / 2;
        }

        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 135, 206, 250, 255);
        SDL_RenderClear(renderer);

        SDL_Rect renderPlayerRect = {player.position.x - camera.x, player.position.y, player.position.w, player.position.h};

        for (int i = 0; i < PLATFORM_NUM; ++i) {
            SDL_Rect renderPlatformRect = {platforms[i].x - camera.x, platforms[i].y, platforms[i].w, platforms[i].h};
            SDL_RenderCopy(renderer, platformTexture, NULL, &renderPlatformRect);
        }

        SDL_Texture* currentSpriteSheet;
        SDL_Rect* currentClip;

        if (player.isJumping) {
            if (player.velocityY > 0) { 
                currentSpriteSheet = fallingSpriteSheet;
                renderPlayerRect.w = 504 / 6; 
                renderPlayerRect.h = 495 / 6; 
                currentClip = NULL; 
            } else {
                currentSpriteSheet = jumpSpriteSheet;
                renderPlayerRect.w = SPRITE_WIDTH / 3; 
                renderPlayerRect.h = SPRITE_HEIGHT / 3; 
                currentClip = &spriteClips[player.jumpFrame + 2 * SPRITES_PER_IMAGE];
            }
        } else if (player.isPreparingToJump) {
            currentSpriteSheet = jumpSpriteSheet;
            renderPlayerRect.w = SPRITE_WIDTH / 3; 
            renderPlayerRect.h = SPRITE_HEIGHT / 3; 
            Uint32 currentTime = SDL_GetTicks();
            if (currentTime - startTime > FRAME_TIME) {
                player.prepareJumpFrame = (player.prepareJumpFrame + 1) % 2;  // Solo usar los primeros dos frames
                startTime = currentTime;
            }
            currentClip = &spriteClips[player.prepareJumpFrame];
        } else {
            currentSpriteSheet = (currentFrame < SPRITES_PER_IMAGE) ? spriteSheet1 : spriteSheet2;
            currentClip = &spriteClips[currentFrame % SPRITES_PER_IMAGE];
        }

        SDL_RendererFlip flip = direction == -1 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
        SDL_RenderCopyEx(renderer, currentSpriteSheet, currentClip, &renderPlayerRect, 0, NULL, flip);

        SDL_RenderPresent(renderer);
    }

    Mix_FreeMusic(backgroundMusic);
    backgroundMusic = NULL;

    SDL_DestroyTexture(spriteSheet1);
    SDL_DestroyTexture(spriteSheet2);
    SDL_DestroyTexture(jumpSpriteSheet);
    SDL_DestroyTexture(fallingSpriteSheet); 
    SDL_DestroyTexture(platformTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}
//gcc -o game game.cpp -lSDL2 -lSDL2_image -lSDL2_mixer
