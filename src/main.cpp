#include <cstddef>
#include <sstream>
#include <string>
#include <fstream>
#include "libs/nlohmann/json.hpp"
using json = nlohmann::json;
#include "Color.h"
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
std::map<std::string, ColorMap> colorMaps;

constexpr int WIDTH = 640;
constexpr int HEIGHT = 480;

static void mainloop();

int main() {
	if (SDL_Init(SDL_INIT_EVERYTHING) > 0) {
		printf("Could not initialize SDL: %s\n", SDL_GetError());
		SDL_Quit();
		return 0;
	}

	window = SDL_CreateWindow("Pathfinding", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
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

	colorMaps = LoadMapsFromFile("assets/colormaps.json");
	std::string filename;
	filename = "demos/demo2.json";
	// std::cin >> filename;

	std::ifstream jsonStream(filename);
	json demo = json::parse(jsonStream);

	puzzle = new Puzzle(demo);

	pthread_create(&puzzleThread, NULL, Puzzle::CheapestPathGreedy, puzzle);

	emscripten_set_main_loop(mainloop, 0, false);

	return 0;
}

static void mainloop() {
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 16, 16, 16, 255);
	SDL_RenderClear(renderer);

	DrawText(renderer, font, "Accesses so far: " + std::to_string(puzzle->arrayAccesses), WIDTH / 2, 20);

	int tileSize = 50;
	int borderSize = 2;
	int borderSizeFocused = 4;

	int centerX = WIDTH / 2;
	int centerY = HEIGHT / 2;

	int originX = centerX - puzzle->width * tileSize / 2;
	int originY = centerY - puzzle->height * tileSize / 2;

	SDL_Color wallColor = {32, 32, 32, 255};
	SDL_Color unexploredColor = {80, 80, 80, 255};
	SDL_Color startColor = {0, 0, 255, 255};
	SDL_Color endColor = {255, 0, 0, 255};
	SDL_Color pathColor = {255, 0, 255, 255};

	for (int x = 0; x < puzzle->width; x++) {
		for (int y = 0; y < puzzle->height; y++) {
			int tileBorderSize = borderSize;
			int tileX = originX + x * tileSize;
			int tileY = originY + y * tileSize;

			SDL_Color color;
	 		SDL_Color borderColor = {16, 16, 16, 255};
			std::stringstream text;
			if (puzzle->walls[puzzle->GetBoundedIndex(x, y)] == -1) {
				color = wallColor;
			}
			else if (puzzle->distances[puzzle->GetBoundedIndex(x, y)] == -1) {
				color = unexploredColor;
				text << "Inf.";
			}
			else {
				double distance = puzzle->distances[puzzle->GetBoundedIndex(x, y)];
				double relativeDistance = distance / puzzle->bestSoFar;
				color = colorMaps["plasma"].GetMap(relativeDistance);
				text << std::setprecision(3) << puzzle->distances[puzzle->GetBoundedIndex(x, y)];
			}

			if (puzzle->highlighted[puzzle->GetBoundedIndex(x, y)]) {
				borderColor = pathColor;
				tileBorderSize = borderSizeFocused;
			}

			SDL_Rect outerRect = {tileX, tileY, tileSize, tileSize};
			SDL_Rect innerRect = {tileX + tileBorderSize, tileY + tileBorderSize, tileSize - tileBorderSize * 2, tileSize - tileBorderSize * 2};

			SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.b, borderColor.g, borderColor.a);
			SDL_RenderFillRect(renderer, &outerRect);

	  		SDL_SetRenderDrawColor(renderer, color.r, color.b, color.g, color.a);
			SDL_RenderFillRect(renderer, &innerRect);

			DrawText(renderer, font, text.str(), tileX + tileSize / 2, tileY + tileSize / 2);
	 	}
	}


	SDL_RenderPresent(renderer);
}
