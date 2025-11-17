#pragma once
#include <array>
#include <chrono>
#include <cmath>
#include <ratio>
#include <stack>
#include <string_view>
#include <vector>
#include "libs/nlohmann/json.hpp"
using json = nlohmann::json;

constexpr int NEIGHBOR_COUNT = 8;
constexpr int NEIGHBOR_OFFSET_X[NEIGHBOR_COUNT] = {0, 1, 1, 1, 0, -1, -1, -1};
constexpr int NEIGHBOR_OFFSET_Y[NEIGHBOR_COUNT] = {1, 1, 0, -1, -1, -1, 0, 1};

constexpr double WEIGHT_DIAGONAL = 1.414213562;
constexpr double NEIGHBOR_COSTS[NEIGHBOR_COUNT] = {1, WEIGHT_DIAGONAL, 1, WEIGHT_DIAGONAL, 1, WEIGHT_DIAGONAL, 1, WEIGHT_DIAGONAL};

constexpr int MAX_DELAY_TIME = 7;

const std::string delayNames[] = {
	"1 second",
	"100 ms",
	"10 ms",
	"1 ms",
	"0.1 ms",
	"0.01 ms",
	"0.001 ms",
	"No delay",
};

constexpr std::chrono::duration<long, std::ratio<1, 1000000>> delayTimes[] = {
	std::chrono::microseconds(1000000),
	std::chrono::microseconds(100000),
	std::chrono::microseconds(10000),
	std::chrono::microseconds(1000),
	std::chrono::microseconds(100),
	std::chrono::microseconds(10),
	std::chrono::microseconds(1),
	std::chrono::microseconds(0),
};

struct Puzzle {
    int height;
    int width;
    int startx;
    int starty;
    int endx;
    int endy;

    bool done = false;
    int* walls;
    double* distances;
    bool* highlighted;
    double bestSoFar = -1;
    int* path;
    int pathSize = 0;
    bool optimizeBruteForce = false;
    int delayTime = 0;

    bool updatePathSync;
    int* pathSync;
    int pathSizeSync = 0;

    int arrayAccesses = 0;
    // Used for animation
    int iterationsPassed = 0;
    int iterationBudget = 0;
    double finalCost = -1;

    Puzzle(json demo);
    int GetBoundedIndex(int x, int y);
    int GetIndexX(int index);
    int GetIndexY(int index);
    bool IsWall(int x, int y);
    // Returns the length of the shortest path, calculated using the brute force approach
    static void* CheapestPathBruteForce(void* arg);
    double CheapestPathBruteForceRecursive(std::vector<bool> explored, int current, double cost);
    // Returns the length of the shortest path, calculated using the greedy approach
    static void* CheapestPathGreedy(void* arg);
    void PrintDistances();
    void ShowShortestPath();
};
