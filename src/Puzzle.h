#pragma once
#include <array>
#include <cmath>
#include <vector>
#include "libs/nlohmann/json.hpp"
using json = nlohmann::json;

constexpr int NEIGHBOR_COUNT = 8;
constexpr int NEIGHBOR_OFFSET_X[NEIGHBOR_COUNT] = {0, 1, 1, 1, 0, -1, -1, -1};
constexpr int NEIGHBOR_OFFSET_Y[NEIGHBOR_COUNT] = {1, 1, 0, -1, -1, -1, 0, 1};

constexpr double WEIGHT_DIAGONAL = 1.414213562;
constexpr double NEIGHBOR_COSTS[NEIGHBOR_COUNT] = {1, WEIGHT_DIAGONAL, 1, WEIGHT_DIAGONAL, 1, WEIGHT_DIAGONAL, 1, WEIGHT_DIAGONAL};

struct Puzzle {
    int height;
    int width;
    int startx;
    int starty;
    int endx;
    int endy;

    int* walls;
    double* distances;
    double bestSoFar = -1;

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
};
