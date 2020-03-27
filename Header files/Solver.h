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
	std::pair<std::vector<int>, double> solve_simulated_annealing(double initial_temperature = 1000,
	double temp_reduction_constant = 0.999999,double probability_constant = 1);
private:
	std::vector<std::vector<double>> graph;
	double length_of_path(std::vector<int> order);
	Graphics* graphics;
};