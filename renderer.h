#ifndef RENDERER_H
#define RENDERER_H

struct SDL_Renderer;  
struct nk_context;   

void initParticles(void);
void render(struct SDL_Renderer* renderer, struct nk_context *ctx);

#endif