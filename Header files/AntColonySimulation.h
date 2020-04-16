#pragma once
#include <algorithm>
#include <vector>
#include <map>
#include <math.h>
#include "Solver.h"
class AntColonySimulation : public Solver
{
public:
	AntColonySimulation(double alpha, double beta, double q, double evaporation_rate, int number_of_iterations, int swarm_size, double initial_pheromone_strength);
	AntColonySimulation() {};
	std::pair<std::vector<int>, double> solve(std::vector<std::vector<double>> graph);
private:
	double total_pheromone_distance_product(std::vector<std::vector<double>>* pheromone_graph, int current_node, std::vector<bool>* visited_nodes, double alpha, double beta);
	void update_pheromones(std::vector<std::vector<double>>* pheromones_graph, std::map<std::pair<int, int>, double>* edge_pheromone_map, double rho);
	double alpha = 1.5;
	double beta = 2;
	double q = 1; 
	double evaporation_rate = 0.5;
	int number_of_iterations = 50;
	int swarm_size = 100;
	double initial_pheromone_strength = 1;
};

