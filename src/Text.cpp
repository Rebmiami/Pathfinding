#include "Text.h"

SDL_Surface* textSurface;
SDL_Texture* textTexture;
SDL_Rect textRect;

void DrawText(SDL_Renderer* renderer, TTF_Font* font, std::string text, int x, int y) {
	if (text.length() == 0)
		return;

	SDL_Color fg = {255, 255, 255, 255};
	SDL_Color bg = {0, 0, 0, 0};

	textSurface = TTF_RenderText_Blended(font, text.c_str(), fg);
	textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

	textRect.x = x - textSurface->w / 2;
	textRect.y = y - textSurface->h / 2;
	textRect.w = textSurface->w;
	textRect.h = textSurface->h;

	SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

	SDL_DestroyTexture(textTexture);
	SDL_FreeSurface(textSurface);
}
