#include "..//Header files/Solver.h"

std::vector<std::vector<double>> convert_points_to_graph(std::vector<sf::Vector2f> points)
{
	int n = points.size();
	std::vector<std::vector<double>> graph(n);

	// For each point calculate distance between every other point
	for (int i = 0; i < n; i++)
	{
		auto current_point = points[i];
		graph[i] = std::vector<double>(n);
		for (int j = 0; j < n; j++)
		{
			if (j != i)
			{
				auto next_point = points[j];
				// Calculates distance between two chosen points
				double dx = current_point.x - next_point.x;
				double dy = current_point.y - next_point.y;
				double distance = sqrt(dx * dx + dy * dy);

				graph[i][j] = distance;
			}
			// If new chosen point is the same, weight in a graph is -1
			else
				graph[i][j] = -1;
		}
	}
	return graph;
}

Solver::Solver(Graphics* graphics, std::vector<sf::Vector2f> points)
{
	graph = convert_points_to_graph(points);
	this->graphics = graphics;
}

Solver::Solver(Graphics* graphics, std::vector<std::vector<double>> graph)
{
	this->graph = graph;
	this->graphics = graphics;
}

Solver::Solver(Graphics* primary_graphics, Graphics* secondary_graphics, std::vector<sf::Vector2f> points)
{
	graph = convert_points_to_graph(points);
	this->graphics = graphics;
	this->secondary_graphics = secondary_graphics;
}

std::pair<std::vector<int>, double> Solver::solve_bruteforce()
{
	int number_of_nodes = graph.size();
	std::vector<int> order_of_visiting(number_of_nodes);
	// Fills order_of_visiting with numbers from 0 to n - 1
	std::iota(order_of_visiting.begin(), order_of_visiting.end(), 0);

	double minimal_total_weight = INFINITY;
	std::vector<int> solution;
	// Loop through each possible permutation of orders of visiting
	do
	{
		double current_total_weight = 0;
		// Visit nodes in order of the current permutation
		for (int i = 0; i < number_of_nodes; i++)
		{
			int current_node_index = order_of_visiting[i];
			int next_node_index;
			// If next node is out of range, return to the starting node
			if (i + 1 >= number_of_nodes)
				next_node_index = order_of_visiting[0];
			else
				next_node_index = order_of_visiting[i + 1];

			// Find weight between current and next node and add it to the running total
			double current_weight = graph[current_node_index][next_node_index];
			current_total_weight += current_weight;
		}

		// Update minimal total weight and save the solution
		if (current_total_weight < minimal_total_weight)
		{
			minimal_total_weight = current_total_weight;
			solution = std::vector<int>(order_of_visiting);
		}
		graphics->update_graph(order_of_visiting, solution);
		std::this_thread::sleep_for(std::chrono::milliseconds(Globals::draw_delay));
	} while (std::next_permutation(order_of_visiting.begin(), order_of_visiting.end()));

	auto order_and_distance = std::pair<std::vector<int>, int>(solution, minimal_total_weight);
	return order_and_distance;
}

double Solver::length_of_path(std::vector<int> order)
{
	double path_length = 0;
	int no_nodes = order.size();
	for(size_t i = 0; i < no_nodes; i++)
		path_length += graph[order[i]][order[(i + 1) % no_nodes]];
	return path_length;
}

std::pair<std::vector<int>, double> Solver::solve_simulated_annealing(double initial_temperature,
double temp_reduction_constant,double probability_constant, int opt2_rounds)
{
	int permutation_length = graph.size();
	std::vector<int> order_of_visiting(permutation_length);
	// Fills order_of_visiting with numbers from 0 to n - 1
	std::iota(order_of_visiting.begin(), order_of_visiting.end(), 0);
	double length_current = length_of_path(order_of_visiting);

	double minimal_total_weight = INFINITY;
	double temperature = initial_temperature;
	std::vector<int> solution;

	int changes_accepted;
	do
	{
		int number_of_changes = 10 * permutation_length;
		changes_accepted = 0;
		for (int i = 0; i < number_of_changes; i++)
		{
			// Choose one neighboring state at random
			int rand_index1 = rand() % permutation_length;
			int rand_index2 = rand() % permutation_length;

			//Reject the neighboring state if it is the same as the current state
			if (rand_index1 != rand_index2)
			{
				//Swap two adjacent elements, if rand_index is last element, swap first and last
				std::swap(order_of_visiting[rand_index1], order_of_visiting[(rand_index2)]);
				auto neighboring_state = order_of_visiting;
				std::swap(order_of_visiting[rand_index1], order_of_visiting[(rand_index2)]);

				//Perform 2-opt improvment on the neighbor
				neighboring_state = optimize_genome_locally(neighboring_state, 2-opt2_rounds);

				
				// Choose random number between 0 and 1
				double rand_double = rand() / (RAND_MAX * 1.0);
				// Evaluate probability function of acceptance
				double length_new = length_of_path(neighboring_state);
				double length_difference = length_current - length_new;
				if (length_difference != 0)
				{
					double probability_of_acceptance;
					if (length_new > length_current)
						probability_of_acceptance = std::exp((length_difference) / (probability_constant * temperature));
					else
						probability_of_acceptance = 1;

					// Change state to neighboring state if probability is greater than the chosen random number
					if (probability_of_acceptance > rand_double)
					{
						changes_accepted++;
						order_of_visiting = neighboring_state;
						length_current = length_of_path(order_of_visiting);
					}

					// Update best solution so far
					if (length_current < minimal_total_weight)
					{
						minimal_total_weight = length_current;
						solution = order_of_visiting;
					}
				}
			}

			// Draw current order
			graphics->update_graph(order_of_visiting, solution);
			std::this_thread::sleep_for(std::chrono::milliseconds(Globals::draw_delay));
		}

		std::cout << "Tmperature: " << temperature << " Accepted changees: " << changes_accepted << " Lenght: " << minimal_total_weight << "\n";

		// Reduce temperature
		temperature *= temp_reduction_constant;
	} while (changes_accepted > 0);

	auto order_and_distance = std::pair<std::vector<int>, int>(solution, minimal_total_weight);
	return order_and_distance;
} 

void Solver::update_pheromones(std::vector<std::vector<double>>* pheromones_graph, std::map<std::pair<int, int>, double>* edge_pheromone_map, double rho)
{
	int no_nodes = graph.size();
	for (int i = 0; i < no_nodes; i++)
	{
		for (int j = 0; j < no_nodes; j++)
		{
			if (i != j)
			{
				(*pheromones_graph)[i][j] *= (1-rho);
				auto edge = std::pair<int, int>(std::min(i, j), std::max(i, j));
				(*pheromones_graph)[i][j] += (*edge_pheromone_map)[edge];
			}
		}
	}
}

double Solver::total_pheromone_distance_product(std::vector<std::vector<double>>* pheromone_graph, int current_node, std::vector<bool>* visited_nodes, double alpha, double beta)
{
	double total_sum = 0;
	int no_nodes = pheromone_graph->size();
	for (int neighbour_index = 0; neighbour_index < no_nodes; neighbour_index++)
	{
		if (!(*visited_nodes)[neighbour_index] && neighbour_index != current_node)
		{
			double pheromones_strength = (*pheromone_graph)[current_node][neighbour_index];
			double heuristic_value = 1 / graph[current_node][neighbour_index];
			total_sum += std::pow(pheromones_strength, alpha) * std::pow(heuristic_value,beta);
		}
	}
	/*for (size_t i = 0; i < no_nodes; i++)
	{
		for (size_t j = 0; j < no_nodes; j++)
		{
			if (i != j)
				total_sum += std::pow((*pheromone_graph)[i][j], alpha) * std::pow(1/graph[i][j],beta);
		}
	}*/
	return total_sum;
}

/// <summary>
/// 
/// </summary>
/// <param name="alpha">constant - pheromone importance</param>
/// <param name="beta">constant - heuristic information importance</param>
/// <param name="q"></param>
/// <param name="evaporation_rate">constant between 0 and 1, larger values mean more evaporation</param>
/// <param name="number_of_iterations"></param>
/// <param name="swarm_size">Number of ants in one iteration</param>
/// <param name="initial_pheromone_strength"></param>
/// <returns></returns>
std::pair<std::vector<int>, double> Solver::solve_ant_colony_simulation(double alpha, double beta, double q, double evaporation_rate, int number_of_iterations, int swarm_size, double initial_pheromone_strength)
{
	int number_of_nodes = graph.size();
	// Create pheromone graph with every edge equal to the initial pheromone strenght
	auto pheromone_graph = std::vector<std::vector<double>>(number_of_nodes);
	for (size_t i = 0; i < number_of_nodes; i++)
		pheromone_graph[i] = std::vector<double>(number_of_nodes, initial_pheromone_strength);
	
	std::vector<int> solution;
	std::vector<int> current_permutation;
	double best_path_length = INFINITY;
	for (int iteration = 0; iteration < number_of_iterations; iteration++)
	{
		std::map<std::pair<int, int>, double> edge_pheromone_map;
		for (int ant_index = 0; ant_index < swarm_size; ant_index++)
		{
			// Choose random starting node for an ant and add it to current permutation
			current_permutation = std::vector<int>(); current_permutation.reserve(number_of_nodes);
			auto visited_nodes = std::vector<bool>(number_of_nodes);
			int current_node_index = rand() % number_of_nodes;
			current_permutation.push_back(current_node_index);
			visited_nodes[current_node_index] = true;
			while (current_permutation.size() < number_of_nodes)
			{
				// Generate random number for every neighboring node and 
				// multiply by the neighboring edge pheromone strength
				// and find the maximum such product
				bool changed_node = false;
				double sum_all_valid_combinations = total_pheromone_distance_product(&pheromone_graph,current_node_index, &visited_nodes, alpha, beta);
				while (!changed_node)
				{
					double rand_double = rand() / (RAND_MAX * 1.0);
					for (int neighbour_index = 0; neighbour_index < number_of_nodes; neighbour_index++)
					{
						double probability;
						if (visited_nodes[neighbour_index])
						{
							probability = 0;
						}
						else
						{
							probability = std::pow(pheromone_graph[current_node_index][neighbour_index], alpha) *
								std::pow(1 / graph[current_node_index][neighbour_index], beta) /
								sum_all_valid_combinations;
						}

						if (probability > rand_double)
						{
							//Ant goes over edge current_node_index <--> neighbour_index
							current_permutation.push_back(neighbour_index);
							visited_nodes[neighbour_index] = true;
							current_node_index = neighbour_index;
							changed_node = true;
							break;
						}
					}
				}
			}
			double current_path_length = length_of_path(current_permutation);
			if (current_path_length < best_path_length)
			{
				solution = current_permutation;
				best_path_length = current_path_length;
			}

			for (size_t i = 0; i < number_of_nodes; i++)
			{
				int index1 = current_permutation[i];
				int index2 = current_permutation[(i + 1) % number_of_nodes];
				auto edge = std::pair<int, int>(std::min(index1, index2), std::max(index1, index2));
				edge_pheromone_map[edge] += q / current_path_length;
			}
		}
		update_pheromones(&pheromone_graph,&edge_pheromone_map, evaporation_rate);
		// Draw current order
		graphics->update_graph(current_permutation, solution);
		std::this_thread::sleep_for(std::chrono::milliseconds(Globals::draw_delay));
		std::cout << "Iteration: " << iteration << " Length: " << best_path_length << "\n";
	}
	return std::pair<std::vector<int>, double>(solution, best_path_length);
}

std::vector<std::vector<int>> Solver::generate_starting_population(int population_size)
{
	int permutation_length = graph.size();
	auto population = std::vector<std::vector<int>>(population_size);
	for (int i = 0; i < population_size; i++)
	{
		auto permutation = std::vector<int>(permutation_length);
		std::iota(permutation.begin(), permutation.end(), 0);
		/*for (int j = 0; j <= permutation_length - 2; j++)
		{
			int swap_position = j + rand() % (permutation_length - j);
			std::swap(permutation[j], permutation[swap_position]);
		}*/
		std::shuffle(permutation.begin(),permutation.end(),Globals::random_generator);
		population[i] = permutation;
	}
	return population;
}

std::vector<std::pair<int, int>> Solver::choose_mating_pairs(std::vector<std::vector<int>>* population, int number_of_pairs)
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
	std::vector<double> probability_distribution(population->size()+1);
	double normalized_sum = 0;
	for (size_t i = 0; i < population->size(); i++)
	{
		probability_distribution[i] = normalized_sum;
		normalized_sum += fitnesses[i] / total_fitness;
	}
	probability_distribution[population->size()-1] = 1.1;

	auto mating_pairs = std::vector<std::pair<int, int>>(number_of_pairs);
	for (size_t i = 0; i < number_of_pairs; i++)
	{
		std::pair<int, int> mating_pair;
		//Choose first parent following the distribution
		double rand_double = rand() / (RAND_MAX * 1.0);
		for (int i = 0; i < probability_distribution.size(); i++)
		{
			if (rand_double >= probability_distribution[i] && rand_double < probability_distribution[i + 1])
			{
				mating_pair.first = i;
				break;
			}
		}
		//Choose second parent
		rand_double = rand() / (RAND_MAX * 1.0);
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

std::vector<int> crossover_parents(std::vector<int>* parent1, std::vector<int>* parent2, int min_length, int max_length)
{
	//Initialize child and uniform distributions
	std::vector<int> child(parent1->size());
	std::uniform_int_distribution<int> uniform_distribution(0, parent1->size());
	int starting_position = uniform_distribution(Globals::random_generator);
	uniform_distribution = std::uniform_int_distribution<int>(min_length, max_length);
	int length = uniform_distribution(Globals::random_generator);

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
	current_position = starting_position+length;
	int child_index = current_position;
	while (taken_cities.size() != child.size())
	{
		int parent2_city = parent2->at(current_position % parent2->size());
		if (taken_cities.find(parent2_city) == taken_cities.end()) //City wasn't taken
		{
			taken_cities.insert(parent2_city);
			child[child_index%child.size()] = parent2_city;
			child_index++;
		}
		current_position++;
	}
	return child;
}

std::vector<int> opt2_swap(std::vector<int>* permutation, int i, int k)
{
	std::vector<int> new_permutation(permutation->size());

	std::copy(permutation->begin(), permutation->begin() + i, new_permutation.begin());
	
	std::reverse_copy(permutation->begin() + i, permutation->begin() + k+1, new_permutation.begin() + i);
	std::copy(permutation->begin() + k + 1, permutation->end(), new_permutation.begin() + k + 1);
	return new_permutation;
}

std::vector<int> Solver::optimize_genome_locally(std::vector<int> permutation, int rounds)
{
	for (size_t round = 0; round < rounds; round++)
	{
		double best_distance = length_of_path(permutation);
		bool better_path_found = false;
		for (int i = 0; !better_path_found && i < permutation.size()-1; i++)
		{
			for (int k = i+1; !better_path_found && k < permutation.size(); k++)
			{
				auto new_permutation = opt2_swap(&permutation, i, k);
				double new_distance = length_of_path(new_permutation);
				if (new_distance < best_distance)
				{
					permutation = new_permutation;
					best_distance = new_distance;
					better_path_found = true;
				}
			}
		}
		if (!better_path_found)
		{
			return permutation;
		}
	}
	return permutation;
}

std::pair<std::vector<int>, double> Solver::solve_genetic_algorithm(int population_size, int generations, double mutation_rate, double min_length_percentage, double max_length_percentage, int opt2_rounds)
{
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
		graphics->update_graph(best_perm_in_generation, best_permutation);
		std::this_thread::sleep_for(std::chrono::milliseconds(Globals::draw_delay));
		std::cout << "Iteration: " << generation_index << " Length: " << best_path_length << "\n";
	}
	return std::pair<std::vector<int>, double>(best_permutation,best_path_length);
}
