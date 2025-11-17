#pragma once
#include <vector>
#include <SDL.h>
#include <string>
#include <map>
#include "libs/nlohmann/json.hpp"
using json = nlohmann::json;

struct ColorMap {
	ColorMap();
	ColorMap(std::vector<SDL_Color> _colors);
	std::vector<SDL_Color> colors;
	SDL_Color GetMap(double v);
};

std::map<std::string, ColorMap> LoadMapsFromFile(std::string path);
