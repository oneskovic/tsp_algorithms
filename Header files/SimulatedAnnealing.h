#pragma once
#include <algorithm>
#include <vector>
#include <numeric>
#include <math.h>
#include "Solver.h"
class SimulatedAnnealing : public Solver
{
public:
	SimulatedAnnealing(std::vector<double> parameters);
	SimulatedAnnealing() {};
	std::pair<std::vector<int>, double> solve(std::vector<std::vector<double>> graph);
	std::vector<double> parameters = { initial_temperature, temp_reduction_constant, probability_constant, opt2_rounds, number_of_neighbors };
private:
	double initial_temperature = 30; // 0, 1000
	double temp_reduction_constant = 0.5; // 0, 1
	double probability_constant = 1; // 1, 1
	double opt2_rounds = 30; // 0, 100
	double number_of_neighbors = 30; // 1, 100

	// {0, 1000}, {0, 1}, {1, 1}, {0, 100}, {1, 100}
};

