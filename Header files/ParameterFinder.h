#pragma once
#include "SimulatedAnnealing.h"
#include "AntColonySimulation.h"
#include "GeneticAlgorithm.h"
#include <fstream>

class ParameterFinder
{
public:
	ParameterFinder(Solver* algorithm);

	void add_parameter_bounds(std::vector<std::pair<double, double>> parameter_bounds);
	void find_parameters(int iteration_number, int reps_number, int n, std::string output_path, std::string parameters_path);


private:
	std::vector<std::pair<double, double>> parameter_bounds;
	Solver* algorithm;
	std::vector<std::vector<double>> convert_points_to_graph1(std::vector<sf::Vector2f> points);
};

