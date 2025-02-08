#include "renderer.h"
#include "globals.h"
#include <SDL2/SDL.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_INCLUDE_STANDARD_FUNCTIONS
#define NK_INCLUDE_COMMAND_USERDATA
#define NK_INCLUDE_VERTEX_LAYOUT

#include "nuklear.h"
#include "nuklear_sdl_renderer.h"

#define DEFAULT_PRADIUS 15

int NP = 10;
float gravity = 9.8f;
int pradius = DEFAULT_PRADIUS;
int rectWidth = 600, rectHeight = 600;
int isSimulationRunning = 0;
int prev_pradius = DEFAULT_PRADIUS;
int prev_rectWidth = 600;
int prev_rectHeight = 600;
float spacing = 5;

struct color {
    int r;
    int g;
    int b;
    int a;
};

struct particle {
    int x;
    int y;
    int radius;
    float vy;
    float vx;
    struct color col;
};

struct particle *particles = NULL;

void DrawCircle(SDL_Renderer *renderer, int32_t centreX, int32_t centreY, int32_t radius) {
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;
            int dy = radius - h;
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, centreX + dx, centreY + dy);
            }
        }
    }
}

void initParticles() {
    if (particles) {
        free(particles);
    }
    particles = malloc(NP * sizeof(struct particle));
    if (!particles) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    int particlesPerRow = (int)sqrt(NP);
    int particlesPerCol = (NP + particlesPerRow - 1) / particlesPerRow; 

    float space = pradius * 2 + spacing; 
    int xStart = 320;
    int yStart = 320;

    for (int i = 0; i < NP; i++) {
        int row = i / particlesPerRow;
        int col = i % particlesPerRow;

        particles[i].x = xStart + col * space;
        particles[i].y = yStart + row * space;
        particles[i].radius = pradius;
        particles[i].vx = 0;
        particles[i].vy = 0;
        particles[i].col = (struct color){ rand() % 256, rand() % 256, rand() % 256, 255 };
    }
}

void resolveCollisions(struct particle *p) {
    int leftBound = 300 + p->radius;
    int rightBound = 300 + rectWidth - p->radius;
    int topBound = 300 + p->radius;
    int bottomBound = 300 + rectHeight - p->radius;

    if (p->x > rightBound) { p->x = rightBound; p->vx *= -0.8f; }
    if (p->x < leftBound) { p->x = leftBound; p->vx *= -0.8f; }
    if (p->y > bottomBound) { p->y = bottomBound; p->vy *= -0.8f; }
    if (p->y < topBound) { p->y = topBound; p->vy *= -0.8f; }
}

void updateParticleSettings() {
    if (pradius != prev_pradius || rectWidth != prev_rectWidth || rectHeight != prev_rectHeight) {
        for (int i = 0; i < NP; i++) {
            particles[i].radius = pradius;
            particles[i].x = fmin(fmax(particles[i].x, 300 + pradius), 300 + rectWidth - pradius);
            particles[i].y = fmin(fmax(particles[i].y, 300 + pradius), 300 + rectHeight - pradius);
        }
        prev_pradius = pradius;
        prev_rectWidth = rectWidth;
        prev_rectHeight = rectHeight;
    }
}

void render(SDL_Renderer* renderer, struct nk_context *ctx) {
    updateParticleSettings();
    
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect rect = { 300, 300, rectWidth, rectHeight };
    SDL_RenderDrawRect(renderer, &rect);

    for (int i = 0; i < NP; i++) {
        SDL_SetRenderDrawColor(renderer, particles[i].col.r, particles[i].col.g, particles[i].col.b, particles[i].col.a);
        if (isSimulationRunning) {
            particles[i].vy += gravity * deltaTime;
            particles[i].y += particles[i].vy;
            particles[i].x += particles[i].vx;
            resolveCollisions(&particles[i]);
        }
        DrawCircle(renderer, particles[i].x, particles[i].y, particles[i].radius);
    }

    if (nk_begin(ctx, "Simulation Settings", nk_rect(10, 10, 250, 600),
        NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MOVABLE)) {
        
        nk_layout_row_dynamic(ctx, 30, 1);
        if (nk_button_label(ctx, isSimulationRunning ? "Stop Simulation" : "Start Simulation")) {
            isSimulationRunning = !isSimulationRunning;
        }
        
        nk_layout_row_dynamic(ctx, 25, 1);
        nk_label(ctx, "Gravity:", NK_TEXT_LEFT);
        nk_slider_float(ctx, 0.0f, &gravity, 20.0f, 0.1f);

        int prev_NP = NP;
        nk_label(ctx, "Balls Num:", NK_TEXT_LEFT);
        nk_slider_int(ctx, 1, &NP, 100, 1);
        if (NP != prev_NP) {
            initParticles();
        }

        int prev_spacing = spacing;
        nk_label(ctx, "Ball Spacing:", NK_TEXT_LEFT);
        nk_slider_float(ctx, 0.0f, &spacing, 20.0f, 0.1f);
        if (spacing != prev_spacing) {
            initParticles();
        }
        
        nk_label(ctx, "Ball Radius:", NK_TEXT_LEFT);
        nk_slider_int(ctx, 5, &pradius, 30, 1);
        
        nk_label(ctx, "Rectangle Width:", NK_TEXT_LEFT);
        nk_slider_int(ctx, 200, &rectWidth, 700, 10);
        
        nk_label(ctx, "Rectangle Height:", NK_TEXT_LEFT);
        nk_slider_int(ctx, 200, &rectHeight, 700, 10);
        
        if (nk_button_label(ctx, "Reset Particles")) {
            initParticles();
            isSimulationRunning = 0;
        }
    }
    nk_end(ctx);
    nk_sdl_render(NK_ANTI_ALIASING_ON);
    SDL_RenderPresent(renderer);
}

void cleanup() {
    if (particles) {
        free(particles);
        particles = NULL;
    }
}