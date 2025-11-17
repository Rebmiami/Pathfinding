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

enum ChoiceState {
	ChooseDemo,
	ChooseAlgorithm,
	Done
};

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
Puzzle* puzzle;
TTF_Font* font;
pthread_t puzzleThread;
std::map<std::string, ColorMap> colorMaps;
ChoiceState state;

constexpr int WIDTH = 640;
constexpr int HEIGHT = 480;

static void mainloop();
static void DrawPuzzlePath(int originX, int originY, int tileSize);

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

	emscripten_set_main_loop(mainloop, 0, false);

	return 0;
}

static void mainloop() {
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 16, 16, 16, 255);
	SDL_RenderClear(renderer);

	// Worst input handling code written Ever because I had to do it very fast
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		// Handle keyboard input for text input
		if (event.type == SDL_KEYDOWN) {
			switch (state) {
				case ChooseDemo: {
					std::string filename;
					if (event.key.keysym.scancode == SDL_SCANCODE_1) {
						filename = "demos/demo1.json";
						state = ChooseAlgorithm;
					}
					else if (event.key.keysym.scancode == SDL_SCANCODE_2) {
						filename = "demos/demo2.json";
						state = ChooseAlgorithm;
					}
					if (state == ChooseAlgorithm) {
						std::ifstream jsonStream(filename);
						json demo = json::parse(jsonStream);

						puzzle = new Puzzle(demo);
					}
					break;
				}
				case ChooseAlgorithm: {
					if (event.key.keysym.scancode == SDL_SCANCODE_1) {
						state = Done;
						pthread_create(&puzzleThread, NULL, Puzzle::CheapestPathGreedy, puzzle);
					}
					else if (event.key.keysym.scancode == SDL_SCANCODE_2) {
						state = Done;
						pthread_create(&puzzleThread, NULL, Puzzle::CheapestPathBruteForce, puzzle);
					}
					else if (event.key.keysym.scancode == SDL_SCANCODE_3) {
						state = Done;
						puzzle->optimizeBruteForce = true;
						pthread_create(&puzzleThread, NULL, Puzzle::CheapestPathBruteForce, puzzle);
					}
					break;
				}
				case Done: {
					if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
						if (puzzle->delayTime < MAX_DELAY_TIME)
							puzzle->delayTime++;
					}
					else if (event.key.keysym.scancode == SDL_SCANCODE_LEFT) {
						if (puzzle->delayTime > 0)
							puzzle->delayTime--;
					}
					break;
				}
			}
		}
	}

	switch (state) {
		case ChooseDemo:
			DrawText(renderer, font, "Press a number to choose a maze:", WIDTH / 2, 20);
			DrawText(renderer, font, "1: Maze 1", WIDTH / 2, 50);
			DrawText(renderer, font, "2: Maze 2", WIDTH / 2, 90);
			break;
		case ChooseAlgorithm:
			DrawText(renderer, font, "Press a number to choose an algorithm:", WIDTH / 2, 20);
			DrawText(renderer, font, "1: Greedy", WIDTH / 2, 50);
			DrawText(renderer, font, "2: Brute Force", WIDTH / 2, 90);
			DrawText(renderer, font, "3: Brute Force (optimized)", WIDTH / 2, 130);
			break;
		case Done: {
			if (puzzle->done) {
				DrawText(renderer, font, "Finished in " + std::to_string(puzzle->arrayAccesses) + " array accesses", WIDTH / 2, 20);
				if (puzzle->finalCost != -1)
					DrawText(renderer, font, "Path cost: " + std::to_string(puzzle->finalCost), WIDTH / 2, 50);
				else
					DrawText(renderer, font, "Did not find a solution", WIDTH / 2, 50);
				DrawText(renderer, font, "Reload page to try again", WIDTH / 2, HEIGHT - 20);
			}
			else {
				DrawText(renderer, font, "Accesses so far: " + std::to_string(puzzle->arrayAccesses), WIDTH / 2, 20);
				DrawText(renderer, font, "Delay between iterations: " + delayNames[puzzle->delayTime], WIDTH / 2, 50);
				if (puzzle->delayTime > 0)
					DrawText(renderer, font, "<", WIDTH / 2 - 200, 50);
				if (puzzle->delayTime < MAX_DELAY_TIME)
					DrawText(renderer, font, ">", WIDTH / 2 + 200, 50);
			}

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
			SDL_Color highlightColor = {255, 0, 255, 255};
			SDL_Color pathColor = {0, 0, 200, 255};

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
						borderColor = highlightColor;
						tileBorderSize = borderSizeFocused;
					}

					SDL_Rect outerRect = {tileX, tileY, tileSize, tileSize};
					SDL_Rect innerRect = {tileX + tileBorderSize, tileY + tileBorderSize, tileSize - tileBorderSize * 2, tileSize - tileBorderSize * 2};

					SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.b, borderColor.g, borderColor.a);
					SDL_RenderFillRect(renderer, &outerRect);

			  		SDL_SetRenderDrawColor(renderer, color.r, color.b, color.g, color.a);
					SDL_RenderFillRect(renderer, &innerRect);

					SDL_SetRenderDrawColor(renderer, pathColor.r, pathColor.b, pathColor.g, pathColor.a);
					for (int i = -1; i < 1; i++)
						for (int j = -1; j < 1; j++)
							DrawPuzzlePath(originX + i, originY + j, tileSize);

					DrawText(renderer, font, text.str(), tileX + tileSize / 2, tileY + tileSize / 2);
			 	}
			}
			// Should prevent the race condition
			puzzle->updatePathSync = true;
			break;
		}
	}



	SDL_RenderPresent(renderer);
}

static void DrawPuzzlePath(int originX, int originY, int tileSize) {
	SDL_Point buffer[puzzle->width * puzzle->height];
	for (int i = 0; i < puzzle->pathSizeSync; i++) {
		int v = puzzle->pathSync[i];
		int indexX = puzzle->GetIndexX(v);
		int indexY = puzzle->GetIndexY(v);

		int tileX = originX + indexX * tileSize + tileSize / 2;
		int tileY = originY + indexY * tileSize + tileSize / 2;

		buffer[i] = {tileX, tileY};

		SDL_RenderDrawLines(renderer, buffer, i);
	}
}
