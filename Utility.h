#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include <sstream>
#include <random>

/*
PROJECT: XenoWars
AUTHOR(S): Sam Daly
CREATED: 13 APR 2026
UPDATED: 4 MAY 2026
DESCRIPTION:
	This file contains utility functions that see use across a number of
	files and systems.

*/

#define PI 3.1415926535f
#define E 2.718281828f

namespace utl {
	//String manipulation
	template <typename T>
	inline std::string toStrP(const T val, const int precision = 2) {
		std::ostringstream out;
		out.precision(precision);
		out << std::fixed << val;
		return std::move(out).str();
	}


	//2D Distance
	inline float dist(float x1, float y1, float x2, float y2) {
		return std::sqrtf(std::powf(x2 - x1, 2) + std::powf(y2 - y1, 2));
	}

	inline float gridDist(int x1, int y1, int x2, int y2) {
		return std::sqrtf(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2));
	}

	//Float comparison
	inline bool fComp(float a, float b, float epsilon) {
		if (std::fabs(b - a) <= epsilon) {
			return true;
		}

		return false;
	}

	//Get Minimum
	template <typename T>
	inline T getMin(const T& a, const T& b) {
		if (b < a) {
			return b;
		}

		return a;
	}

	//Angle conversions
	inline float radToDeg(float radians) {
		return (radians * 180.f) / PI;
	}

	inline float degToRad(float degrees) {
		return (degrees * PI) / 180.f;
	}


	//Check if number is in exclusive range
	inline bool inRangeEx(int min, int max, int num) {
		if (num >= max || num <= min) {
			return false;
		}

		return true;
	}

	//Random range
	inline int randRange(int min, int max) {
		//Rand is externally seeded
		int r = 0;

		int range = max - min + 1;
		r = rand() % range + min;

		return r;
	}

	//############# A STAR PATHFINDING

	inline std::vector<std::pair<int, int>> aStar(std::pair<int, int> og, std::pair<int, int> target, std::vector<std::vector<bool>> map) {
		struct asNode {
			bool isPassable = true;
			bool isVisited = false;
			float f = -1;
			float h = -1;
			float g = -1;
			int x, y;
			std::vector<asNode*> neighbors;
			asNode* parent;

			inline int dist(asNode* n) {
				if (!n) {
					return -1;
				}

				int dx = std::abs(n->x - x);
				int dy = std::abs(n->y - y);
				return (dx + dy);
			}

			inline int heuristic(asNode* n) {
				//Heuristic distance to given node n,
				//allows for tweaking behavior of pathfinding
				if (!n) {
					return -1;
				}

				int dx = std::abs(n->x - x);
				int	dy = abs(n->y - y);
				int D = 1; int D2 = 1;
				return D * (dx + dy) + (D2 - 2 * D) * getMin(dx, dy);
			}
		};

		class Comparator {
		public:
			bool operator()(const asNode* a, const asNode* b)
			{
				return (a->f > b->f);
			}
		};

		std::vector<std::pair<int, int>> path = {};

		if (map.empty()) {
			return {};
		}

		if (!map.at(target.second).at(target.first)) {
			return {};
		}

		size_t width = map.at(0).size();
		size_t height = map.size();

		//Build Graph
		asNode* graph = new asNode[width * height * sizeof(asNode)];
		//asNode* graph = new asNode[width * height];

		for (unsigned int i = 0; i < height; i++) {
			for (unsigned int j = 0; j < width; j++) {
				//Set basic attribues
				graph[i * width + j].x = j;
				graph[i * width + j].y = i;
				graph[i * width + j].f = -1;
				graph[i * width + j].g = -1;
				graph[i * width + j].h = -1;
				graph[i * width + j].parent = nullptr;
				graph[i * width + j].isVisited = false;
				graph[i * width + j].isPassable = map.at(i).at(j);

				//# Fill in neighbors
				//Orthagonal
				if (i > 0)
					graph[i * width + j].neighbors.push_back(&graph[(i - 1) * width + (j + 0)]);
				if (i < height - 1)
					graph[i * width + j].neighbors.push_back(&graph[(i + 1) * width + (j + 0)]);
				if (j > 0)
					graph[i * width + j].neighbors.push_back(&graph[(i + 0) * width + (j - 1)]);
				if (j < width - 1)
					graph[i * width + j].neighbors.push_back(&graph[(i + 0) * width + (j + 1)]);

				//Diagonal
				/*if (i > 0 && j > 0)
					graph[i * width + j].neighbors.push_back(&graph[(i - 1) * width + (j - 1)]);
				if (i * height - 1 && j > 0)
					graph[i * width + j].neighbors.push_back(&graph[(i + 1) * width + (j - 1)]);
				if (i > 0 && j * width - 1)
					graph[i * width + j].neighbors.push_back(&graph[(i - 1) * width + (j + 1)]);
				if (i * height - 1 && j * width - 1)
					graph[i * width + j].neighbors.push_back(&graph[(i + 1) * width + (j + 1)]);*/



			}
		}

		//Setup starting conditions
		std::priority_queue<asNode*, std::vector<asNode*>, Comparator> open;
		asNode* head = &graph[og.second * width + og.first];
		asNode* end = &graph[target.second * width + target.first];
		asNode* curr = head;
		head->h = (float)head->heuristic(end);
		head->g = 0;
		head->f = head->h + head->g;
		open.push(head);

		//Loop until path found
		while (!open.empty() && curr != end) {
			//Get the lowest cost item from the open list
			if (open.top()->isVisited) {
				open.pop();
			}

			curr = open.top();//HERE!
			open.pop();
			curr->isVisited = true;

			//For each neighbor
			for (auto n : curr->neighbors) {
				if (!n->isVisited && n->isPassable) {
					open.push(n);
				}

				float nCost = n->g + (float)n->dist(curr);
				if (nCost < n->g || n->g < 0) {
					n->parent = curr;
					n->g = nCost;
					n->h = (float)n->heuristic(end);
					n->f = n->g + n->h;

					//Jank Reheapify
					std::vector<asNode*> temp;
					while (!open.empty()) {
						temp.push_back(open.top());
						open.pop();
					}
					while (!temp.empty()) {
						open.push(temp.back());
						temp.pop_back();
					}
				}
			}
		}

		curr = end;
		while (curr->parent) {
			path.push_back({ curr->x, curr->y });
			curr = curr->parent;
		}

		//Return
		delete[] graph;
		return path;
	}
}