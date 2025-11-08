#include <exception>
#include <iostream>
#include <string>
#include <fstream>
#include "libs/nlohmann/json.hpp"
using json = nlohmann::json;
#include "Puzzle.h"

int main() {
    printf("Please input a filename to load a demo from: ");

    std::string filename;
    std::cin >> filename;

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
