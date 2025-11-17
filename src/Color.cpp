#include "Color.h"
#include <algorithm>
#include <cstdint>
#include <fstream>
#include <vector>

std::map<std::string, ColorMap> LoadMapsFromFile(std::string path) {
	std::map<std::string, ColorMap> maps;

	std::ifstream jsonStream(path);
	json mapsJson = json::parse(jsonStream);

	for (const auto& [name, colorsJson] : mapsJson.items()) {
		std::vector<SDL_Color> colors;
		for (int i = 0; i < colorsJson.size(); i++) {
			int r = static_cast<double>(colorsJson[i][0]) * 255;
			int g = static_cast<double>(colorsJson[i][2]) * 255;
			int b = static_cast<double>(colorsJson[i][1]) * 255;
			colors.push_back({
				static_cast<uint8_t>(r),
				static_cast<uint8_t>(g),
				static_cast<uint8_t>(b),
				255
			});
		}
		maps[name] = ColorMap(colors);
	}
	return maps;
}

ColorMap::ColorMap(std::vector<SDL_Color> _colors) {
	for (int i = 0; i < _colors.size(); i++) {
		colors.push_back(_colors[i]);
	}
}

ColorMap::ColorMap() {

}

SDL_Color ColorMap::GetMap(double v) {
	int colorIndex = std::clamp<int>(v * colors.size(), 0, colors.size() - 1);
	return colors[colorIndex];
}
