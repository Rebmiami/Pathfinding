#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

void DrawText(SDL_Renderer* renderer, TTF_Font* font, std::string text, int x, int y);
