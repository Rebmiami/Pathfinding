#pragma once
#include <array>
#include <chrono>
#include <cmath>
#include <stack>
#include <vector>
#include "libs/nlohmann/json.hpp"
using json = nlohmann::json;

constexpr int NEIGHBOR_COUNT = 8;
constexpr int NEIGHBOR_OFFSET_X[NEIGHBOR_COUNT] = {0, 1, 1, 1, 0, -1, -1, -1};
constexpr int NEIGHBOR_OFFSET_Y[NEIGHBOR_COUNT] = {1, 1, 0, -1, -1, -1, 0, 1};

constexpr double WEIGHT_DIAGONAL = 1.414213562;
constexpr double NEIGHBOR_COSTS[NEIGHBOR_COUNT] = {1, WEIGHT_DIAGONAL, 1, WEIGHT_DIAGONAL, 1, WEIGHT_DIAGONAL, 1, WEIGHT_DIAGONAL};

constexpr std::chrono::duration delayTime = std::chrono::milliseconds(1);

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
