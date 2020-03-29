#pragma once
#include <SFML/Graphics.hpp>
#include "Graphics.h"
#include <numeric>
#include <thread>
#include <iostream>
#include <map>
class Solver 
{
public:
	Solver(Graphics* graphics, std::vector<sf::Vector2f> points);
	Solver(Graphics* graphics, std::vector<std::vector<double>> graph);
	Solver(Graphics* primary_graphics, Graphics* secondary_graphics, std::vector<sf::Vector2f> points);
	std::pair<std::vector<int>, double> solve_bruteforce();
	std::pair<std::vector<int>, double> solve_simulated_annealing(double initial_temperature = 100,
	double temp_reduction_constant = 0.9,double probability_constant = 1);
	std::pair<std::vector<int>, double> solve_ant_colony_simulation(double alpha = 1, double beta = 3, double q = 1, double evaporation_rate = 0.3, int number_of_iterations = 500, int swarm_size = 1000, double initial_pheromone_strength = 1);
private:
	std::vector<std::vector<double>> graph;
	double length_of_path(std::vector<int> order);
	double total_pheromone_distance_product(std::vector<std::vector<double>>* pheromone_graph, int current_node, std::vector<bool>* visited_nodes, double alpha, double beta);
	void update_pheromones(std::vector<std::vector<double>>* pheromones_graph, std::map<std::pair<int, int>, double>* edge_pheromone_map, double rho);
	Graphics* graphics, *secondary_graphics;
};