#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 600
#define HEIGHT 480
#define GRAVITY 9.8


enum ParticleType {
    PARTICLE_EMPTY,
    PARTICLE_SAND,
    PARTICLE_WATER
};

typedef struct {
    enum ParticleType type;
    Color color;
    int lifeTime;
    Vector2 velocity;
    int hasBeenUpdated;
} Particle;

void UpdateParticle(Particle** particles, const Vector2* locationsToValidate, const int particleIndex, const int size) {
    Particle* particle = particles[particleIndex];
    particle->velocity.y = Clamp(particle->velocity.y + (GRAVITY * GetFrameTime()), -10.f, 10.f);

    for (size_t i = 0; i < size; i++) {
        int index = locationsToValidate[i].y * WIDTH + locationsToValidate[i].x;
        if (index < 0 || index >= WIDTH * HEIGHT) continue;

        Particle* target = particles[index];
        if (target->type == PARTICLE_EMPTY) {
            particles[index] = particles[particleIndex];
            particles[particleIndex] = target;
            break;
        }
    }
}

void UpdateSand(Particle** particles, int x, int y) {
    Vector2 locationsToValidate[] = {
        {x, y + 1},
        {x - 1, y + 1},
        {x + 1, y + 1}
    };
    int size = sizeof(locationsToValidate) / sizeof(Vector2);

    UpdateParticle(particles, locationsToValidate, y * WIDTH + x, size);
}

void UpdateWater(Particle** particles, int x, int y) {
    Vector2 locationsToValidate[] = {
        {x, y + 1},
        {x - 1, y + 1},
        {x + 1, y + 1},
        {x, y - 1},
        {x, y + 1}
    };
    int size = sizeof(locationsToValidate) / sizeof(Vector2);

    UpdateParticle(particles, locationsToValidate, y * WIDTH + x, size);
}

void GenerateParticle(Particle** particles, enum ParticleType type, Color color) {
    Vector2 mousePosition = GetMousePosition();
    int x = (int)mousePosition.x;
    int y = (int)mousePosition.y;
    int radius = 50;

    for (int i = -radius; i < radius; i += 3) {
        for (int j = -radius; j < radius; j += 3) {
            if (i * i + j * j > radius * radius) continue;
            int particleIndex = (y + i) * WIDTH + x + j;
            Particle* particle = particles[particleIndex];
            particle->type = type;
            particle->color = color;
        }
    }
}

void ShowFPS() {
    char currentFPS[50];
    snprintf(currentFPS, 50, "FPS: %d", GetFPS());
    DrawText(currentFPS, 10, 10, 20, GREEN);
}

int main(void) {
    InitWindow(WIDTH, HEIGHT, "Particle simulatino");
    SetTargetFPS(144);

    Particle **particles = (Particle **)calloc(sizeof(Particle*), WIDTH * HEIGHT);
    RenderTexture2D particleTexture = LoadRenderTexture(WIDTH, HEIGHT);

    for (size_t i = 0; i < WIDTH * HEIGHT; i++) {
        particles[i] = (Particle *)calloc(sizeof(Particle), 1);
        particles[i]->type = PARTICLE_EMPTY;
        particles[i]->color = DARKBLUE;
    }


    while (!WindowShouldClose()) {

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            GenerateParticle(particles, PARTICLE_SAND, ORANGE);
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            GenerateParticle(particles, PARTICLE_WATER, SKYBLUE);
        }
        

        for (int i = HEIGHT - 2; i >= 0; i--) {
            for (int j = 0; j < WIDTH; j++) {
                Particle* particle = particles[i * WIDTH + j];

                switch (particle->type)
                {
                case PARTICLE_SAND:
                    UpdateSand(particles, j, i);
                    break;
                
                case PARTICLE_WATER:
                    UpdateWater(particles, j, i);
                    break;
                
                default:
                    break;
                }
            }
        }
        

        BeginTextureMode(particleTexture);
        for (size_t i = 0; i < HEIGHT; i++) {
            for (size_t j = 0; j < WIDTH; j++) {
                DrawPixel(j, HEIGHT - 1 -  i, particles[i * WIDTH + j]->color);
            }
        }
        EndTextureMode();
        

        BeginDrawing();

            DrawTexture(particleTexture.texture, 0, 0, WHITE);
            ShowFPS();

        EndDrawing();
    }

        UnloadRenderTexture(particleTexture);
    CloseWindow();

    for (size_t i = 0; i < WIDTH * HEIGHT; i++) {
        free(particles[i]);
    }
    free(particles);

    return 0;
}
