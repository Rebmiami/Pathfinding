#include <cstddef>
#include <string>
#include <fstream>
#include "libs/nlohmann/json.hpp"
using json = nlohmann::json;
#include "Puzzle.h"
#include <emscripten.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <pthread.h>
#include "Text.h"

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
Puzzle* puzzle;
TTF_Font* font;
pthread_t puzzleThread;

constexpr int WIDTH = 640;
constexpr int HEIGHT = 480;

static void mainloop();

int main() {
	if (SDL_Init(SDL_INIT_EVERYTHING) > 0) {
        printf("Could not initialize SDL: %s\n", SDL_GetError());
        SDL_Quit();
        return 0;
    }

    window = SDL_CreateWindow("SDLRPG", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Could not create window: %s\n", SDL_GetError());
        SDL_Quit();
        return 0;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (renderer == nullptr) {
        printf("Could not create renderer: %s\n", SDL_GetError());
        SDL_Quit();
        return 0;
    }

    TTF_Init();
    font = TTF_OpenFont("assets/MozillaText-Regular.ttf", 20);

    std::string filename;
    filename = "demos/demo1.json";
    // std::cin >> filename;

    std::ifstream jsonStream(filename);
    json demo = json::parse(jsonStream);

    puzzle = new Puzzle(demo);

    // pthread_create(&puzzleThread, NULL, Puzzle::CheapestPathBruteForce, puzzle);

    emscripten_set_main_loop(mainloop, 0, false);

    return 0;
}

static void mainloop() {
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 16, 16, 16, 255);
    SDL_RenderClear(renderer);

    DrawText(renderer, font, "Accesses so far: " + std::to_string(puzzle->arrayAccesses), 5, 5);

	SDL_RenderPresent(renderer);
}
