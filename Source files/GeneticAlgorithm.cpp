#include "..\Header files\GeneticAlgorithm.h"

GeneticAlgorithm::GeneticAlgorithm(std::vector<double> parameters)
{
	this->population_size = parameters[0];
	this->generations = parameters[1];
	this->opt2_rounds = parameters[2];
	this->mutation_rate = parameters[3];
	this->min_length_percentage = parameters[4];
	this->max_length_percentage = parameters[5];

	this->parameters = parameters;
}

std::vector<std::vector<int>> GeneticAlgorithm::generate_starting_population(int population_size)
{
	int permutation_length = graph.size();
	auto population = std::vector<std::vector<int>>(population_size);
	for (int i = 0; i < population_size; i++)
	{
		auto permutation = std::vector<int>(permutation_length);
		std::iota(permutation.begin(), permutation.end(), 0);
		std::shuffle(permutation.begin(), permutation.end(), Globals::random_generator.mersenne_twister);
		population[i] = permutation;
	}
	return population;
}

std::vector<std::pair<int, int>> GeneticAlgorithm::choose_mating_pairs(std::vector<std::vector<int>>* population, int number_of_pairs)
{
	auto fitnesses = std::vector<double>(population->size());
	double total_fitness = 0;
	for (size_t i = 0; i < population->size(); i++)
	{
		//Compute individual fitness
		double fitness = 1 / length_of_path(population->at(i));
		total_fitness += fitness;
		fitnesses[i] = fitness;
	}

	//Compute the probability distribution - cumulative sums of normalized fitnesses
	std::vector<double> probability_distribution(population->size() + 1);
	double normalized_sum = 0;
	for (size_t i = 0; i < population->size(); i++)
	{
		probability_distribution[i] = normalized_sum;
		normalized_sum += fitnesses[i] / total_fitness;
	}
	probability_distribution[population->size() - 1] = 1.1;

	auto mating_pairs = std::vector<std::pair<int, int>>(number_of_pairs);
	for (size_t i = 0; i < number_of_pairs; i++)
	{
		std::pair<int, int> mating_pair;
		//Choose first parent following the distribution
		double rand_double = Globals::random_generator.get_double(0, 1);
		for (int i = 0; i < probability_distribution.size(); i++)
		{
			if (rand_double >= probability_distribution[i] && rand_double < probability_distribution[i + 1])
			{
				mating_pair.first = i;
				break;
			}
		}
		//Choose second parent
		rand_double = Globals::random_generator.get_double(0, 1);
		for (int i = 0; i < probability_distribution.size(); i++)
		{
			if (rand_double >= probability_distribution[i] && rand_double < probability_distribution[i + 1])
			{
				mating_pair.second = i;
				break;
			}
		}
		mating_pairs[i] = mating_pair;
	}
	return mating_pairs;
}

std::vector<int> GeneticAlgorithm::crossover_parents(std::vector<int>* parent1, std::vector<int>* parent2, int min_length, int max_length)
{
	//Initialize child and uniform distributions
	std::vector<int> child(parent1->size());
	int starting_position = Globals::random_generator.get_int(0, parent1->size() - 1);
	int length = Globals::random_generator.get_int(min_length, max_length - 1);

	//Copy part of parent1 permutation
	std::unordered_set<int> taken_cities;
	int current_position = starting_position;
	for (size_t i = 0; i < length; i++)
	{
		int parent1_city = parent1->at(current_position % parent1->size());
		child[current_position % child.size()] = parent1_city;
		taken_cities.insert(parent1_city);
		current_position++;
	}

	//Copy rest from parent2
	current_position = starting_position + length;
	int child_index = current_position;
	while (taken_cities.size() != child.size())
	{
		int parent2_city = parent2->at(current_position % parent2->size());
		if (taken_cities.find(parent2_city) == taken_cities.end()) //City wasn't taken
		{
			taken_cities.insert(parent2_city);
			child[child_index % child.size()] = parent2_city;
			child_index++;
		}
		current_position++;
	}
	return child;
}

void GeneticAlgorithm::mutate_genome(std::vector<int>* permutation, double mutation_rate)
{
	double rand_double = Globals::random_generator.get_double(0, 1);
	if (rand_double < mutation_rate)
	{
		int index0 = Globals::random_generator.get_int(0, permutation->size() - 1);
		int index1 = Globals::random_generator.get_int(0, permutation->size() - 1);
		std::swap(permutation->at(index0), permutation->at(index1));
	}
}

std::pair<std::vector<int>, double> GeneticAlgorithm::solve(std::vector<std::vector<double>> graph)
{
	this->graph = graph;
	auto start_time = std::chrono::steady_clock::now();
	int max_length = graph.size() * max_length_percentage;
	int min_length = graph.size() * min_length_percentage;
	auto current_population = generate_starting_population(population_size);

	auto best_permutation = current_population[0];
	double best_path_length = INFINITY;
	for (int generation_index = 0; generation_index < generations; generation_index++)
	{
		auto new_population = std::vector<std::vector<int>>(); new_population.reserve(population_size);
		auto mating_pairs = choose_mating_pairs(&current_population, std::ceil(population_size / 2.0));

		auto best_perm_in_generation = std::vector<int>();
		double best_path_len_in_generation = INFINITY;
		for (size_t i = 0; i < mating_pairs.size(); i++)
		{
			auto parent1 = &current_population[mating_pairs[i].first];
			auto parent2 = &current_population[mating_pairs[i].second];
			auto child1 = crossover_parents(parent1, parent2, min_length, max_length);
			auto child2 = crossover_parents(parent2, parent1, min_length, max_length);
			mutate_genome(&child1, mutation_rate);
			mutate_genome(&child2, mutation_rate);

			child1 = optimize_genome_locally(child1, opt2_rounds);
			double child_path = length_of_path(child1);
			if (child_path < best_path_len_in_generation)
			{
				best_path_len_in_generation = child_path;
				best_perm_in_generation = child1;
			}

			child2 = optimize_genome_locally(child2, opt2_rounds);
			child_path = length_of_path(child2);
			if (child_path < best_path_len_in_generation)
			{
				best_path_len_in_generation = child_path;
				best_perm_in_generation = child2;
			}

			new_population.push_back(child1);
			new_population.push_back(child2);
		}

		if (best_path_len_in_generation < best_path_length)
		{
			best_path_length = best_path_len_in_generation;
			best_permutation = best_perm_in_generation;
		}

		current_population = new_population;
	}

	auto current_time = std::chrono::steady_clock::now();
	log_solution(best_permutation, best_path_length, current_time - start_time);
	return std::pair<std::vector<int>, double>(best_permutation, best_path_length);
}
