#pragma once
#include <algorithm>
#include <vector>
#include <numeric>
#include <math.h>
#include "Solver.h"
class SimulatedAnnealing : public Solver
{
public:
	SimulatedAnnealing(double initial_temperature,double temp_reduction_constant, double probability_constant, int opt2_rounds, int number_of_neighbors);
	SimulatedAnnealing() {};
	std::pair<std::vector<int>, double> solve(std::vector<std::vector<double>> graph);
private:
	double initial_temperature = 30;
	double temp_reduction_constant = 0.5;
	double probability_constant = 1;
	int opt2_rounds = 30;
	int number_of_neighbors = 30;
};

