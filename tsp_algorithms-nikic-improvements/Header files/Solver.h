#pragma once
#include <SFML/Graphics.hpp>
#include "Graphics.h"
#include <numeric>
#include <thread>
#include <iostream>
#include <unordered_set>
#include <algorithm>
#include <map>
class Solver 
{
private:
	std::vector<std::vector<double>> graph;
public:
	Solver(Graphics* graphics, std::vector<sf::Vector2f> points);
	Solver(Graphics* graphics, std::vector<std::vector<double>> graph);
	Solver(Graphics* primary_graphics, Graphics* secondary_graphics, std::vector<sf::Vector2f> points);
	std::pair<std::vector<int>, double> solve_bruteforce();
	std::pair<std::vector<int>, double> solve_simulated_annealing(double initial_temperature = 100,
	double temp_reduction_constant = 0.9,double probability_constant = 1);
	std::pair<std::vector<int>, double> solve_ant_colony_simulation(double alpha = 1, double beta = 3, double q = 1, double evaporation_rate = 0.3, int number_of_iterations = 500, int swarm_size = 1000, double initial_pheromone_strength = 1);
	std::pair<std::vector<int>, double> solve_genetic_algorithm(int population_size = 30, int generations = 3000, double mutation_rate = 0.05, double min_length_percentage = 0.3, double max_length_percentage = 0.5, int opt2_rounds = 100);
private:
	double length_of_path(std::vector<int> order);
	//Ant colony simulation
	double total_pheromone_distance_product(std::vector<std::vector<double>>* pheromone_graph, int current_node, std::vector<bool>* visited_nodes, double alpha, double beta);
	void update_pheromones(std::vector<std::vector<double>>* pheromones_graph, std::map<std::pair<int, int>, double>* edge_pheromone_map, double rho);
	//Genetic algorithm
	std::vector<std::vector<int>> generate_starting_population(int population_size);
	std::vector<std::pair<int, int>> choose_mating_pairs(std::vector<std::vector<int>>* population, int number_of_pairs);
	void mutate_genome(std::vector<int>* permutation);
	std::vector<int> optimize_genome_locally(std::vector<int> permutation, int rounds);

	Graphics* graphics, *secondary_graphics;
};