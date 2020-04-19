#pragma once
#include <algorithm>
#include <vector>
#include <map>
#include <math.h>
#include "Solver.h"
class AntColonySimulation : public Solver
{
public:
	AntColonySimulation(std::vector<double> parameters);
	AntColonySimulation() {};
	std::pair<std::vector<int>, double> solve(std::vector<std::vector<double>> graph);
	std::vector<double> parameters = { alpha, beta, q, evaporation_rate, number_of_iterations, swarm_size, initial_pheromone_strength };
private:
	double total_pheromone_distance_product(std::vector<std::vector<double>>* pheromone_graph, int current_node, std::vector<bool>* visited_nodes, double alpha, double beta);
	void update_pheromones(std::vector<std::vector<double>>* pheromones_graph, std::map<std::pair<int, int>, double>* edge_pheromone_map, double rho);
	double alpha = 1.5;
	double beta = 2;
	double q = 1; 
	double evaporation_rate = 0.5;
	double number_of_iterations = 50;
	double swarm_size = 100;
	double initial_pheromone_strength = 1;

	
};

