#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include "globals.h"
#include "renderer.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_SDL_RENDERER_IMPLEMENTATION
#include "nuklear.h"
#include "nuklear_sdl_renderer.h"

int main(int argc, char *args[])
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    float gravity = 1.0f;

    struct nk_font_atlas *atlas;
    struct nk_font_config config = nk_font_config(0);
    struct nk_font *font;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Create window
    window = SDL_CreateWindow("Shitty Liquid Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
    {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Init nukler
    struct nk_context *ctx = nk_sdl_init(window, renderer);
    if (!ctx)
    {
        printf("Nuklear initialization failed!\n");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    float font_scale = 1;

    nk_sdl_font_stash_begin(&atlas);
    font = nk_font_atlas_add_default(atlas, 13 * font_scale, &config);
    nk_sdl_font_stash_end();
    font->handle.height /= font_scale;
    nk_style_set_font(ctx, &font->handle);

    initParticles();

    // Main loop
    Uint64 LAST = SDL_GetPerformanceCounter();
    SDL_Event e;
    int quit = 0;

    while (!quit)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                quit = 1;
            nk_sdl_handle_event(&e); 
        }

        Uint64 NOW = SDL_GetPerformanceCounter();
        deltaTime = (double)(NOW - LAST) / SDL_GetPerformanceFrequency();
        LAST = NOW;

        render(renderer, ctx);
        nk_sdl_render(NK_ANTI_ALIASING_ON);

        SDL_Delay(16); //60 fps
    }

    // Cleanup
    nk_sdl_shutdown();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}