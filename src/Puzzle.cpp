#include "Puzzle.h"
#include <iomanip>
#include <iostream>
#include <list>
#include <stack>
#include <strings.h>
#include <vector>
#include <thread>

Puzzle::Puzzle(json demo) {
	width = demo["width"];
    height = demo["height"];

    startx = demo["startx"];
    starty = demo["starty"];

    endx = demo["endx"];
    endy = demo["endy"];

    walls = new int[width * height]();
    distances = new double[width * height]();
    highlighted = new bool[width * height]();
    path = new std::stack<int>();

    std::vector<int> wallVector = demo["walls"];

    for (int i = 0; i < wallVector.size(); i++) {
       	walls[i] = wallVector[i];
		// -1 is used here in place of infinity
        distances[i] = -1;
        highlighted[i] = false;
    }
    // Set the distance to the starting position to 0
	distances[GetBoundedIndex(startx, starty)] = 0;
}

int Puzzle::GetBoundedIndex(int x, int y) {
	if (x >= width || x < 0 || y >= height || y < 0)
		return -1;
	return x + y * width;
}

bool Puzzle::IsWall(int x, int y) {
	arrayAccesses++;
	int index = GetBoundedIndex(x, y);
	if (index == -1)
		return true;
	return walls[index] == -1;
}

int Puzzle::GetIndexX(int index) {
	return index % width;
}

int Puzzle::GetIndexY(int index) {
	return index / width;
}

void* Puzzle::CheapestPathBruteForce(void* args) {
	Puzzle* puzzle = (Puzzle*)args;
	std::vector<bool> explored;
	for (int i = 0; i < puzzle->width * puzzle->height; i++) {
		explored.push_back(false);
	}

	puzzle->finalCost = puzzle->CheapestPathBruteForceRecursive(explored, puzzle->GetBoundedIndex(puzzle->startx, puzzle->starty), 0);
	return 0;
}

double Puzzle::CheapestPathBruteForceRecursive(std::vector<bool> explored, int current, double cost) {
	std::this_thread::sleep_for(delayTime);
	if (distances[current] == -1 || cost < distances[current])
		distances[current] = cost;
	// Optimization: Give up on paths that are more expensive than the current best path
	// if (bestSoFar != -1 && cost > bestSoFar)
	// 	return -1;
	if (current == GetBoundedIndex(endx, endy)) {
		if (bestSoFar == -1 || cost < bestSoFar)
			bestSoFar = cost;
		return cost;
	}
	double bestSolution = -1;
	explored[current] = true;

	for (int i = 0; i < NEIGHBOR_COUNT; i++) {
		int x = GetIndexX(current);
		int y = GetIndexY(current);
		int nx = x + NEIGHBOR_OFFSET_X[i];
		int ny = y + NEIGHBOR_OFFSET_Y[i];
		int n = GetBoundedIndex(nx, ny);
		if (!IsWall(nx, ny)) {
			if (!explored[n]) {
				highlighted[current] = true;
				double newSolution = CheapestPathBruteForceRecursive(explored, n, cost + NEIGHBOR_COSTS[i]);
				highlighted[current] = false;
				if (newSolution != -1 && newSolution < bestSolution || bestSolution == -1)
					bestSolution = newSolution;
			}
		}
	}
	return bestSolution;
}

void* Puzzle::CheapestPathGreedy(void* args) {
	Puzzle* puzzle = (Puzzle*)args;
	std::list<int> unvisited;

	for (int i = 0; i < puzzle->width * puzzle->height; i++) {
		unvisited.push_back(i);
	}

	while (!unvisited.empty()) {
		int next = -1;
		double nextDist = -1;
		for (int i : unvisited) {
			if (puzzle->distances[i] != -1 && (puzzle->distances[i] < nextDist || nextDist == -1)) {
				next = i;
				nextDist = puzzle->distances[i];
			}
		}

		if (next == -1 || next == puzzle->GetBoundedIndex(puzzle->endx, puzzle->endy))
			break;

		unvisited.remove(next);

		for (int i = 0; i < NEIGHBOR_COUNT; i++) {
			int x = puzzle->GetIndexX(next);
			int y = puzzle->GetIndexY(next);
			int nx = x + NEIGHBOR_OFFSET_X[i];
			int ny = y + NEIGHBOR_OFFSET_Y[i];
			int n = puzzle->GetBoundedIndex(nx, ny);

			if (!puzzle->IsWall(nx, ny)) {
				puzzle->highlighted[n] = true;
				std::this_thread::sleep_for(delayTime);
				puzzle->highlighted[n] = false;
				double newDistance = nextDist + NEIGHBOR_COSTS[i];
				if (newDistance > puzzle->bestSoFar) {
					puzzle->bestSoFar = newDistance;
				}
				if (puzzle->distances[n] == -1 || newDistance < puzzle->distances[n])
					puzzle->distances[n] = newDistance;
			}
		}
	}

	puzzle->finalCost = puzzle->distances[puzzle->GetBoundedIndex(puzzle->endx, puzzle->endy)];
	return 0;
}

void Puzzle::PrintDistances() {
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			std::cout << std::setprecision(2) << distances[GetBoundedIndex(x, y)] << "\t";
		}
		std::cout << "\n";
	}
}
