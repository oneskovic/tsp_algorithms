#pragma once
#include <algorithm>
#include <vector>
#include "Globals.h"
#include "Solver.h"
class GeneticAlgorithm : public Solver
{
public:
	GeneticAlgorithm(int population_size, int generations, double mutation_rate, double min_length_percentage, double max_length_percentage, int opt2_rounds);
	GeneticAlgorithm() {};
	std::pair<std::vector<int>, double> solve(std::vector<std::vector<double>> graph);
private:
	std::vector<std::vector<int>> generate_starting_population(int population_size);
	std::vector<std::pair<int, int>> choose_mating_pairs(std::vector<std::vector<int>>* population, int number_of_pairs);
	std::vector<int> crossover_parents(std::vector<int>* parent1, std::vector<int>* parent2, int min_length, int max_length);
	void mutate_genome(std::vector<int>* permutation, double mutation_rate);
	int population_size = 30;
	int generations = 10;
	int opt2_rounds = 30;
	double mutation_rate = 0.1;
	double min_length_percentage = 0.3;
	double max_length_percentage = 0.5;
};

