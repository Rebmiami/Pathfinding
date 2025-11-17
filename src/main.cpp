#include <exception>
#include <iostream>
#include <string>
#include <fstream>
#include "libs/nlohmann/json.hpp"
using json = nlohmann::json;
#include "Puzzle.h"
#include <emscripten.h>
#include <SDL.h>
#include <SDL_ttf.h>

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

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

    emscripten_set_main_loop(mainloop, 0, false);

    printf("Please input a filename to load a demo from: ");

    std::string filename;
    std::cin >> filename;

    TTF_Font* font = TTF_OpenFont("./assets/MozillaText-Regular.ttf", 14);

    std::ifstream jsonStream(filename);
    json demo = json::parse(jsonStream);

    Puzzle bruteForce = Puzzle(demo);
    double bruteForceCost = bruteForce.CheapestPathBruteForce();
    if (bruteForceCost != -1)
    	printf("Found shortest path of cost %.2f in %d array accesses using brute force approach.\n", bruteForceCost, bruteForce.arrayAccesses);
    else
    	printf("Could not find a valid path after %d array accesses using brute force approach.\n", bruteForce.arrayAccesses);

    Puzzle greedy = Puzzle(demo);
    double greedyCost = greedy.CheapestPathGreedy();
    if (greedyCost != -1)
    	printf("Found shortest path of cost %.2f in %d array accesses using greedy approach.\n", greedyCost, greedy.arrayAccesses);
    else
    	printf("Could not find a valid path after %d array accesses using greedy approach.\n", greedy.arrayAccesses);
    greedy.PrintDistances();

    return 0;
}

static void mainloop() {
	SDL_SetRenderDrawColor(renderer, 16, 16, 16, 255);
    SDL_RenderClear(renderer);

	SDL_RenderPresent(renderer);
}
