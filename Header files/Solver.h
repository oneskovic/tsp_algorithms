#pragma once
#include <SFML/Graphics.hpp>
#include "Graphics.h"
#include <numeric>
#include <thread>
class Solver 
{
public:
	Solver(Graphics* graphics, std::vector<sf::Vector2f> points);
	Solver(Graphics* graphics, std::vector<std::vector<double>> graph);
	std::pair<std::vector<int>, double> solve_bruteforce();
private:
	std::vector<std::vector<double>> graph;
	Graphics* graphics;
};