#pragma once
#include <algorithm>
#include <vector>
#include "Globals.h"
#include "Solver.h"
class GeneticAlgorithm : public Solver
{
public:
	GeneticAlgorithm(std::vector<double> parameters);
	GeneticAlgorithm() {};
	std::pair<std::vector<int>, double> solve(std::vector<std::vector<double>> graph);
	std::vector<double> parameters = { population_size, generations, opt2_rounds, mutation_rate, min_length_percentage, max_length_percentage };
private:
	std::vector<std::vector<int>> generate_starting_population(int population_size);
	std::vector<std::pair<int, int>> choose_mating_pairs(std::vector<std::vector<int>>* population, int number_of_pairs);
	std::vector<int> crossover_parents(std::vector<int>* parent1, std::vector<int>* parent2, int min_length, int max_length);
	void mutate_genome(std::vector<int>* permutation, double mutation_rate);
	double population_size = 30;
	double generations = 10;
	double opt2_rounds = 30;
	double mutation_rate = 0.1;
	double min_length_percentage = 0.3;
	double max_length_percentage = 0.5;
};

