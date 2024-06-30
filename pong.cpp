#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 600
#define SPRITE_WIDTH 250
#define SPRITE_ATACK_WIDTH 500
#define SPRITE_STAND_WIDTH 284
#define SPRITE_HEIGHT 246
#define SPRITE_ATACK_HEIGHT 252
#define SPRITE_STAND_HEIGHT 252
#define SPRITES_PER_IMAGE 4
#define SPRITES_PER_IMAGE_ATACK 2
#define PLATFORM_HEIGHT 40
#define FRAME_TIME 100
#define GRAVITY 0.01
#define JUMP_FORCE -2
#define PLATFORM_NUM  24
#define TELEPORT_ZONE_X 6024/2
#define TELEPORT_ZONE_Y 420
#define TELEPORT_ZONE_X_2 20800/2
#define TELEPORT_ZONE_Y_2 420
#define TELEPORT_ZONE_WIDTH 50
#define TELEPORT_ZONE_HEIGHT 200
#define TELEPORT_DEST_X 7000
#define TELEPORT_DEST_Y 300
#define TELEPORT_DEST_X_2 12100
#define TELEPORT_ZONE_X_3 30400/2
#define TELEPORT_ZONE_Y_3 200



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
    int isStop;
    int isAtacking;
    int attackFrame;
} Character;

typedef struct {
    int x, y;
} Camera;

typedef struct {
    SDL_Rect position;
    int currentFrame;
} FireEntity;

void savePlayerPosition(Character player);
void loadPlayerPosition(Character *player);

int checkCollision(SDL_Rect rectA, SDL_Rect rectB) {
    return !(rectA.y + rectA.h <= rectB.y ||
             rectA.y >= rectB.y + rectB.h ||
             rectA.x + rectA.w <= rectB.x ||
             rectA.x >= rectB.x + rectB.w);
}
int init(SDL_Renderer **renderer) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

  

    

int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        return 0;
    }

    return 1;
}

int isMouseInsideRect(int x, int y, SDL_Rect rect) {
    return (x > rect.x && x < rect.x + rect.w && y > rect.y && y < rect.y + rect.h);
}
void savePlayerPosition(Character player) {
    FILE *file = fopen("player_position.txt", "w");
    if (file != NULL) {
        fprintf(file, "%d %d\n", player.position.x, player.position.y);
        fclose(file);
    } else {
        printf("No se pudo abrir el archivo o aun no se a creado.\n");
    }
}
void loadPlayerPosition(Character *player) {
    FILE *file = fopen("player_position.txt", "r");
    if (file != NULL) {
        fscanf(file, "%d %d", &player->position.x, &player->position.y);
        fclose(file);
    } else {
        printf("Error al abrir el archivo player_position.txt para carga: ");
        player->position.x = (SCREEN_WIDTH - SPRITE_WIDTH) / 2;
        player->position.y = SCREEN_HEIGHT - SPRITE_HEIGHT - PLATFORM_HEIGHT - 10;
    }
}



int main(int argc, char* args[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Texture* spriteSheet1 = NULL;
    SDL_Texture* spriteSheet2 = NULL;
    SDL_Texture* jumpSpriteSheet = NULL;
    SDL_Texture* fallingSpriteSheet = NULL;
    SDL_Texture* standingSpriteSheet = NULL;
    SDL_Texture* AtackSpriteSheet1 = NULL;
    SDL_Texture* InfoImage = NULL;
    SDL_Texture* AtackSpriteSheet2 = NULL;
    SDL_Texture* AtackSpriteSheet3 = NULL;
    SDL_Texture* platformTexture = NULL;
    SDL_Texture* platformTexture2 = NULL;
    SDL_Texture* BackImage = NULL;
    SDL_Texture *topImage = NULL;
    SDL_Texture *playButton = NULL;
    SDL_Texture *square1 = NULL;
    SDL_Texture *square2 = NULL;
    SDL_Texture *topRightButton = NULL;
    SDL_Texture *teleportZoneTexture = NULL;
    SDL_Texture *mute = NULL;
    SDL_Texture *Tutorial1 = NULL;
    SDL_Texture *Tutorial2 = NULL;
    SDL_Texture *Tutorial3 = NULL;
    SDL_Texture *Tutorial4 = NULL;
    SDL_Texture *win = NULL;
    SDL_Texture *level1 = NULL;
    SDL_Texture *level2 = NULL;
    SDL_Texture *level3 = NULL;
    SDL_Texture* fireTexture = NULL;
    SDL_Rect spriteClips[SPRITES_PER_IMAGE * 4];
    SDL_Rect standingClips[3];
    SDL_Rect atackingClips[1];
    SDL_Rect teleportZone = {0, 430, 200, 150}; 
    SDL_Rect platforms[PLATFORM_NUM];
    SDL_Rect fireClips[3];
    Mix_Music *backgroundMusic = NULL;
    
    

        int showInitialScreen = 1;
        int ShowInfo = 0;
        int PlayMusic = 1;
        int isMute = 0;      
        int isMusicPlaying = 1; 
                
           

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
    platformTexture2 = IMG_LoadTexture(renderer, "platform2.jpg");
    standingSpriteSheet = IMG_LoadTexture(renderer, "standing.bmp");
    AtackSpriteSheet1 = IMG_LoadTexture(renderer, "atack1.bmp");
    AtackSpriteSheet2 = IMG_LoadTexture(renderer, "atack2.bmp");
    AtackSpriteSheet3 = IMG_LoadTexture(renderer, "atack3.bmp");
    fireTexture = IMG_LoadTexture(renderer, "fire.bmp");
    InfoImage = IMG_LoadTexture(renderer, "info_image.png");
    topImage = IMG_LoadTexture(renderer, "top_image.png");
    playButton = IMG_LoadTexture(renderer, "play_button.png");
    square1 = IMG_LoadTexture(renderer, "square1.png");
    square2 = IMG_LoadTexture(renderer, "square2.png");
    topRightButton = IMG_LoadTexture(renderer, "top_right_button.png");
    BackImage = IMG_LoadTexture(renderer, "background_image.jpg");
    teleportZoneTexture = IMG_LoadTexture(renderer, "teleport_image.webp");
    mute = IMG_LoadTexture(renderer, "mute_button.png");
    Tutorial1 = IMG_LoadTexture(renderer,"tutorial1.png");
    Tutorial2 = IMG_LoadTexture(renderer,"tutorial2.png");
    Tutorial3 = IMG_LoadTexture(renderer,"tutorial3.png");
    Tutorial4 = IMG_LoadTexture(renderer,"tutorial4.png");
    win = IMG_LoadTexture(renderer,"win.png");
    level1 = IMG_LoadTexture(renderer,"level1.png");
    level2 = IMG_LoadTexture(renderer,"level2.png");
    level3 = IMG_LoadTexture(renderer,"level3.png");
    Character player;
  

    if (!topImage||!playButton||!square1||!square2||!topRightButton) {
        printf("Una o más texturas no se pudieron cargar.\n");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return -1;
    }
  

    for (int i = 0; i < SPRITES_PER_IMAGE_ATACK; ++i) {
        atackingClips[i].x = i * SPRITE_ATACK_WIDTH;
        atackingClips[i].y = 0;
        atackingClips[i].w = SPRITE_ATACK_WIDTH;
        atackingClips[i].h = SPRITE_ATACK_HEIGHT;
    }
    for (int i = 0; i < SPRITES_PER_IMAGE_ATACK; ++i) {
        atackingClips[i + SPRITES_PER_IMAGE_ATACK].x = i * SPRITE_ATACK_WIDTH;
        atackingClips[i + SPRITES_PER_IMAGE_ATACK].y = 0;
        atackingClips[i + SPRITES_PER_IMAGE_ATACK].w = SPRITE_ATACK_WIDTH;
        atackingClips[i + SPRITES_PER_IMAGE_ATACK].h = SPRITE_ATACK_HEIGHT;
    }
    for (int i = 0; i < SPRITES_PER_IMAGE_ATACK; ++i) {
        atackingClips[i + 2 * SPRITES_PER_IMAGE_ATACK].x = i * SPRITE_ATACK_WIDTH;
        atackingClips[i + 2 * SPRITES_PER_IMAGE_ATACK].y = 0;
        atackingClips[i + 2 * SPRITES_PER_IMAGE_ATACK].w = SPRITE_ATACK_WIDTH;
        atackingClips[i + 2 * SPRITES_PER_IMAGE_ATACK].h = SPRITE_ATACK_HEIGHT;
    }
    

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
    for (int i = 0; i < 3; ++i) {
        standingClips[i + 2].x = i * SPRITE_STAND_WIDTH;
        standingClips[i + 2].y = 0;
        standingClips[i + 2].w = SPRITE_STAND_WIDTH;
        standingClips[i + 2].h = SPRITE_STAND_HEIGHT;
    }

   int fireClipWidth, fireClipHeight;
SDL_QueryTexture(fireTexture, NULL, NULL, &fireClipWidth, &fireClipHeight);

for (int i = 0; i < 3; ++i) {
    fireClips[i].x = i * (289 / 3);
    fireClips[i].y = 0;
    fireClips[i].w = 289 / 3;
    fireClips[i].h = 287;
}

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
    player.isStop = 1;
    player.isAtacking = 0;
    player.attackFrame = 0;
    loadPlayerPosition(&player);

    Camera camera = {0 - (SPRITE_WIDTH / 6), 0};

    FireEntity fireEntity;
    fireEntity.position.x = 500; 
    fireEntity.position.y = 300;
    fireEntity.position.w = 289 / 3; 
    fireEntity.position.h = 287 /3; 
    fireEntity.currentFrame = 0;

    platforms[0] = (SDL_Rect){0, SCREEN_HEIGHT - PLATFORM_HEIGHT+30, SCREEN_WIDTH * 2, PLATFORM_HEIGHT};
    platforms[1] = (SDL_Rect){SCREEN_WIDTH + 20, SCREEN_HEIGHT - 200, 100, PLATFORM_HEIGHT};
    platforms[2] = (SDL_Rect){SCREEN_WIDTH + 320, SCREEN_HEIGHT - 300, 100, PLATFORM_HEIGHT};
    platforms[3] = (SDL_Rect){SCREEN_WIDTH + 620, SCREEN_HEIGHT - 400, 100, PLATFORM_HEIGHT};
    platforms[4] = (SDL_Rect){SCREEN_WIDTH + 1700, SCREEN_HEIGHT - PLATFORM_HEIGHT+30, SCREEN_WIDTH, PLATFORM_HEIGHT};
    platforms[5] = (SDL_Rect){SCREEN_WIDTH + 1300, SCREEN_HEIGHT - 200, 100, PLATFORM_HEIGHT};
    platforms[6] = (SDL_Rect){6800, SCREEN_HEIGHT - PLATFORM_HEIGHT+30, SCREEN_WIDTH, PLATFORM_HEIGHT};
    platforms[7] = (SDL_Rect){8100, SCREEN_HEIGHT- 200, 100, PLATFORM_HEIGHT};
    platforms[8] = (SDL_Rect){8400, SCREEN_HEIGHT-50, 200, PLATFORM_HEIGHT};
    platforms[9] = (SDL_Rect){8800, SCREEN_HEIGHT-250, 100, PLATFORM_HEIGHT};
    platforms[10] = (SDL_Rect){9000, SCREEN_HEIGHT-350, 100, PLATFORM_HEIGHT};
    platforms[11] = (SDL_Rect){9400, SCREEN_HEIGHT-200, 100, PLATFORM_HEIGHT};
    platforms[12] = (SDL_Rect){9700, SCREEN_HEIGHT-350, 100, PLATFORM_HEIGHT};
    platforms[13] = (SDL_Rect){10100, SCREEN_HEIGHT-50, 50, PLATFORM_HEIGHT};
    platforms[14] = (SDL_Rect){10300, SCREEN_HEIGHT - PLATFORM_HEIGHT+30, 500, PLATFORM_HEIGHT};
    platforms[15] = (SDL_Rect){12000, SCREEN_HEIGHT - PLATFORM_HEIGHT+30, 500, PLATFORM_HEIGHT};
    platforms[16] = (SDL_Rect){12600, SCREEN_HEIGHT-300, 100, PLATFORM_HEIGHT};
    platforms[17] = (SDL_Rect){13000, SCREEN_HEIGHT-200, 100, PLATFORM_HEIGHT};
    platforms[18] = (SDL_Rect){13400, SCREEN_HEIGHT-100, 100, PLATFORM_HEIGHT};
    platforms[19] = (SDL_Rect){13800, SCREEN_HEIGHT-350, 50, PLATFORM_HEIGHT};
    platforms[20] = (SDL_Rect){14200, SCREEN_HEIGHT-50, 50, PLATFORM_HEIGHT};
    platforms[21] = (SDL_Rect){14700, SCREEN_HEIGHT-50, 50, PLATFORM_HEIGHT};
    platforms[22] = (SDL_Rect){15200, SCREEN_HEIGHT-50, 500, PLATFORM_HEIGHT};
    platforms[23] = (SDL_Rect){6050, SCREEN_HEIGHT - 700, 300, PLATFORM_HEIGHT + 1000};

    int currentFrame = 0;
    SDL_Event e;
    int quit = 0;
    Uint32 startTime = SDL_GetTicks();
    int isMoving = 0;
    int direction = 0;

SDL_Texture* backgroundTexture = IMG_LoadTexture(renderer, "platform.png");

if (backgroundTexture == NULL) {
    printf("No se pudo cargar la imagen de fondo. SDL_image Error: %s\n", IMG_GetError());
    return -1;
}

    backgroundMusic = Mix_LoadMUS("background_music.mp3");
    if (backgroundMusic == NULL) {
        printf("No se pudo cargar la música de fondo. Mix_Error: %s\n", Mix_GetError());
        return -1;
    }
  if (isMute){

  }
  else if (!isMute){
    if (Mix_PlayMusic(backgroundMusic, -1) == -1) {
        printf("No se pudo reproducir la música de fondo. Mix_Error: %s\n", Mix_GetError());
        return -1;
    }}

    Uint32 animationStartTime = SDL_GetTicks();
    Uint32 animationFrameTime = 200; 
    

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                savePlayerPosition(player);
                quit = 1;
            } else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_LEFT:
                        if (!player.isPreparingToJump && !player.isJumping) {
                            direction = -1;
                            isMoving = 1;
                            player.isAtacking = 0;
                        }
                        break;
                    case SDLK_RIGHT:
                        if (!player.isPreparingToJump && !player.isJumping) {
                            direction = 1;
                            isMoving = 1;
                            player.isAtacking = 0;
                        }
                        break;
                    case SDLK_UP:
                        if (!player.isJumping && !player.isPreparingToJump) {
                            player.isPreparingToJump = 1;
                            player.prepareJumpFrame = 0;
                            player.isAtacking = 0;
                        }
                        break;
                    case SDLK_r:
                        player.position.y = 0;
                        player.position.x = 0;
                        player.isOnGround = 0;
                        player.isJumping = 1;
                        player.velocityY = -1;
                        player.isAtacking = 0;
                        break;
                    case SDLK_x:
                    if (!player.isJumping &&  !player.isPreparingToJump  && direction == 0){
                        player.isAtacking = 1;
                        player.attackFrame = 0;
                    if (player.position.x >= TELEPORT_ZONE_X - 50 && player.position.x <= TELEPORT_ZONE_X + TELEPORT_ZONE_WIDTH &&
    player.position.y >= TELEPORT_ZONE_Y && player.position.y <= TELEPORT_ZONE_Y + TELEPORT_ZONE_HEIGHT) {
    player.position.x = TELEPORT_DEST_X;
    player.position.y = TELEPORT_DEST_Y;
}
else if (player.position.x >= TELEPORT_ZONE_X_2 - 50 && player.position.x <= TELEPORT_ZONE_X_2 + TELEPORT_ZONE_WIDTH &&
    player.position.y >= TELEPORT_ZONE_Y_2 && player.position.y <= TELEPORT_ZONE_Y_2 + TELEPORT_ZONE_HEIGHT) {
    player.position.x = TELEPORT_DEST_X_2;
    player.position.y = TELEPORT_DEST_Y;
}
else if (player.position.x >= TELEPORT_ZONE_X_3 - 50 && player.position.x <= TELEPORT_ZONE_X_3 + 400 &&
    player.position.y >= TELEPORT_ZONE_Y_2 && player.position.y <= TELEPORT_ZONE_Y_2 + 300) {
    player.position.x = 0;
    player.position.y = 0;
}
}
                        break;
                }

            } if (e.type == SDL_QUIT) {
                        quit = 1;
                    } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                        int x, y;
                        SDL_GetMouseState(&x, &y);

                        if (showInitialScreen) {
                            SDL_Rect playButtonRect = { (SCREEN_WIDTH - 400) / 2, 250, 400, 100 };
                            if (isMouseInsideRect(x, y, playButtonRect)) {
                                showInitialScreen = 0;
                            }
                        } else{
                            SDL_Rect topRightButtonRect = { SCREEN_WIDTH - 100, 0, 100, 100 };
                            if (isMouseInsideRect(x, y, topRightButtonRect)) {
                                showInitialScreen = 1;
                                ShowInfo = 0;
                            }
                            
                        }
                        if (!ShowInfo){
                            SDL_Rect playButtonRect = { (SCREEN_WIDTH + 100) / 2, 400, 100, 100  };
                            SDL_Rect tuneRect = { (SCREEN_WIDTH -300) / 2, 400, 100, 100  };
                            if (isMouseInsideRect(x, y, playButtonRect)) {
                            ShowInfo = 1;
                            showInitialScreen = 0;
                            }
                            if (isMouseInsideRect(x, y, tuneRect)){
                            if (isMute){
                            isMute = 0;
                            if (isMusicPlaying == 0) {
                            Mix_ResumeMusic();
                            isMusicPlaying = 1;
                            }
                            }
                        else {
                            isMute = 1;
                        if (isMusicPlaying == 1) {
                            Mix_PauseMusic();
                            isMusicPlaying = 0;
                        }
                        }}
                        }
                    }   else if (e.type == SDL_KEYUP) {
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
        if (player.position.y > 600){
            player.position.x = 0;
            player.position.y = 0;
            player.isJumping = 1;
          
        }
        if (!isMoving && !player.isPreparingToJump && !player.isJumping) {
            direction = 0;
            player.isStop = 0;
        } else {
            player.isStop = 1;
        }

        if (player.isOnGround) {
            if (direction == 0) {
                player.isStop = 0;
            } else {
                player.isStop = 1;
            }
        }

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
        } else if (!player.isOnGround) {
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
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
        SDL_SetRenderDrawColor(renderer, 135, 206, 250, 255);
       
          
           

        SDL_Rect renderPlayerRect = {player.position.x - camera.x, player.position.y, player.position.w, player.position.h};
        SDL_Rect renderEnemyRect = {fireEntity.position.x - camera.x, fireEntity.position.y, fireEntity.position.w, fireEntity.position.h};
        
        for (int i = 0; i < PLATFORM_NUM; ++i) {
            if (i == 0){
                SDL_Rect tutorial1Rect = { 300 -camera.x, 300, 400, TELEPORT_ZONE_HEIGHT };
                SDL_Rect level1Rect = { 200 -camera.x, 0, 600, TELEPORT_ZONE_HEIGHT };
                SDL_Rect level2Rect = { 7100 -camera.x, 0, 600, TELEPORT_ZONE_HEIGHT };
                SDL_Rect level3Rect = { 11900 -camera.x, 0, 600, TELEPORT_ZONE_HEIGHT };
                SDL_Rect tutorial2Rect = { 1025 -camera.x, 200, 400, TELEPORT_ZONE_HEIGHT };
                SDL_Rect tutorial3Rect = { 2200 -camera.x, 200, 300, TELEPORT_ZONE_HEIGHT };
                SDL_Rect tutorial4Rect = { 2850 -camera.x, 200, 300, TELEPORT_ZONE_HEIGHT };
                SDL_Rect teleportZone1 = { TELEPORT_ZONE_X - camera.x, TELEPORT_ZONE_Y, TELEPORT_ZONE_WIDTH, TELEPORT_ZONE_HEIGHT };
                SDL_Rect teleportZone2 = { TELEPORT_ZONE_X_2 - camera.x, TELEPORT_ZONE_Y_2, TELEPORT_ZONE_WIDTH, TELEPORT_ZONE_HEIGHT };
                SDL_Rect teleportZone3 = { TELEPORT_ZONE_X_3 - camera.x, TELEPORT_ZONE_Y_3, 400, 300 };
             
             SDL_RenderCopy(renderer, Tutorial1, NULL, &tutorial1Rect);
             SDL_RenderCopy(renderer, level1, NULL, &level1Rect);
             SDL_RenderCopy(renderer, level2, NULL, &level2Rect);
             SDL_RenderCopy(renderer, level3, NULL, &level3Rect);
              SDL_RenderCopy(renderer, Tutorial2, NULL, &tutorial2Rect);
               SDL_RenderCopy(renderer, Tutorial3, NULL, &tutorial3Rect);
                SDL_RenderCopy(renderer, Tutorial4, NULL, &tutorial4Rect);
           SDL_RenderCopy(renderer, teleportZoneTexture, NULL, &teleportZone1);
SDL_RenderCopy(renderer, teleportZoneTexture, NULL, &teleportZone2);
SDL_RenderCopy(renderer, win, NULL, &teleportZone3);}

            SDL_Rect renderPlatformRect = {platforms[i].x - camera.x, platforms[i].y, platforms[i].w, platforms[i].h};
            SDL_RenderCopy(renderer, platformTexture2, NULL, &renderPlatformRect);
        }
       
        SDL_Texture* currentSpriteSheet;
        SDL_Rect* currentClip;
        Uint32 currentTime = SDL_GetTicks();

        if (player.isAtacking) {
                renderPlayerRect.w = 500 / 3;
                renderPlayerRect.h = 252 / 3;
            currentSpriteSheet = (player.attackFrame < 2) ? AtackSpriteSheet1 : (player.attackFrame < 4) ? AtackSpriteSheet2 : AtackSpriteSheet3;
            currentClip = &atackingClips[player.attackFrame % SPRITES_PER_IMAGE_ATACK];
            if (currentTime - startTime > FRAME_TIME) {
                player.attackFrame++;
                if (player.attackFrame >= 6) {
                    player.attackFrame = 0;
                    player.isAtacking = 0;
                }
                startTime = currentTime;
            }
        } else if (player.isJumping) {
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

            if (currentTime - startTime > FRAME_TIME) {
                player.prepareJumpFrame = (player.prepareJumpFrame + 1) % 2; 
                startTime = currentTime;
            }
            currentClip = &spriteClips[player.prepareJumpFrame];
        } else if (!player.isStop) {
            currentSpriteSheet = standingSpriteSheet;
            renderPlayerRect.w = SPRITE_WIDTH /3;
            renderPlayerRect.h = SPRITE_HEIGHT /3;
            currentClip = &standingClips[2];
        } else if (player.isStop) {
            currentSpriteSheet = (currentFrame < SPRITES_PER_IMAGE) ? spriteSheet1 : spriteSheet2;
            currentClip = &spriteClips[currentFrame % SPRITES_PER_IMAGE];
        }

        SDL_RendererFlip flip = direction == -1 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
        SDL_RenderCopyEx(renderer, currentSpriteSheet, currentClip, &renderPlayerRect, 0, NULL, flip);

       
    Uint32 currentFireTime = SDL_GetTicks();
if (currentFireTime - animationStartTime > animationFrameTime) {
    renderEnemyRect.w = 289 / 3;
    renderEnemyRect.h = 287 / 3;
    fireEntity.currentFrame = (fireEntity.currentFrame + 1) % 3;
    animationStartTime = currentFireTime;
}
 if (showInitialScreen) {
                    SDL_Rect BackRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT }; 
                    SDL_Rect topRect = { (SCREEN_WIDTH - 575) / 2, 0, 600, 300 };
                    SDL_Rect playButtonRect = { (SCREEN_WIDTH - 400) / 2, 200, 400, 200 };
                    SDL_Rect square1Rect = { (SCREEN_WIDTH - 300) / 2, 400, 100, 100 };
                    SDL_Rect tuneButtonRect = { (SCREEN_WIDTH - 300) / 2, 400, 100, 100 };
                    SDL_Rect square2Rect = { (SCREEN_WIDTH + 100) / 2, 400, 100, 100 };
                    
                    SDL_RenderCopy(renderer, BackImage, NULL, &BackRect);
                    SDL_RenderCopy(renderer, topImage, NULL, &topRect);
                    SDL_RenderCopy(renderer, playButton, NULL, &playButtonRect);
                     if (!isMute){
                         SDL_RenderCopy(renderer, square1, NULL, &square1Rect);
                    }
                    else if (isMute) {
                        SDL_RenderCopy(renderer, mute, NULL, &tuneButtonRect);
                    };
                    SDL_RenderCopy(renderer, square2, NULL, &square2Rect);
                } else {
                    SDL_Rect topRightButtonRect = { SCREEN_WIDTH - 100, 0, 100, 100 };

                    SDL_RenderCopy(renderer, topRightButton, NULL, &topRightButtonRect);

               }
 if (ShowInfo) {
     SDL_Rect BackRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT }; 
     SDL_Rect infoRect = { (SCREEN_WIDTH - 800) / 2, 50, 800, 500 };
     SDL_Rect topRightButtonRect = { SCREEN_WIDTH - 100, 0, 100, 100 };
     
     SDL_RenderCopy(renderer, BackImage, NULL, &BackRect);
     SDL_RenderCopy(renderer, InfoImage, NULL, &infoRect);
     SDL_RenderCopy(renderer, topRightButton, NULL, &topRightButtonRect);
    
 }


   
  
    SDL_RenderPresent(renderer);
    }
      savePlayerPosition(player);

    Mix_FreeMusic(backgroundMusic);
    backgroundMusic = NULL;
    SDL_DestroyTexture(Tutorial1);
    SDL_DestroyTexture(Tutorial2);
    SDL_DestroyTexture(Tutorial3);
    SDL_DestroyTexture(Tutorial4);
    SDL_DestroyTexture(spriteSheet1);
    SDL_DestroyTexture(spriteSheet2);           
    SDL_DestroyTexture(jumpSpriteSheet);
    SDL_DestroyTexture(fallingSpriteSheet);
    SDL_DestroyTexture(platformTexture);
    SDL_DestroyTexture(standingSpriteSheet);
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(fireTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(topImage);
    SDL_DestroyTexture(playButton);
    SDL_DestroyTexture(square1);
    SDL_DestroyTexture(square2);
    SDL_DestroyTexture(topRightButton);
    SDL_DestroyTexture(InfoImage);
    SDL_DestroyTexture(BackImage);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();


    return 0;
}
