#include "..\Header files\AntColonySimulation.h"

AntColonySimulation::AntColonySimulation(double alpha, double beta, double q, double evaporation_rate, int number_of_iterations, int swarm_size, double initial_pheromone_strength)
{
	this->alpha = alpha;
	this->beta = beta;
	this->q = q;
	this->evaporation_rate = evaporation_rate;
	this->number_of_iterations = number_of_iterations;
	this->swarm_size = swarm_size;
	this->initial_pheromone_strength = initial_pheromone_strength;
}

/// <param name="alpha">constant - pheromone importance</param>
/// <param name="beta">constant - heuristic information importance</param>
/// <param name="q"></param>
/// <param name="evaporation_rate">constant between 0 and 1, larger values mean more evaporation</param>
/// <param name="number_of_iterations"></param>
/// <param name="swarm_size">Number of ants in one iteration</param>
/// <param name="initial_pheromone_strength"></param>
std::pair<std::vector<int>, double> AntColonySimulation::solve(std::vector<std::vector<double>> graph)
{
	this->graph = graph;
	auto start_time = std::chrono::steady_clock::now();
	int number_of_nodes = graph.size();
	/* Create pheromone graph with every edge equal to the initial pheromone strenght*/
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
			int current_node_index = Globals::random_generator.get_int(0, number_of_nodes - 1);
			current_permutation.push_back(current_node_index);
			visited_nodes[current_node_index] = true;
			while (current_permutation.size() < number_of_nodes)
			{
				// Generate random number for every neighboring node and 
				// multiply by the neighboring edge pheromone strength
				// and find the maximum such product
				bool changed_node = false;
				double sum_all_valid_combinations = total_pheromone_distance_product(&pheromone_graph, current_node_index, &visited_nodes, alpha, beta);
				while (!changed_node)
				{
					double rand_double = Globals::random_generator.get_double(0, 1);
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
		update_pheromones(&pheromone_graph, &edge_pheromone_map, evaporation_rate);
		auto current_time = std::chrono::steady_clock::now();
		log_solution(solution, best_path_length, current_time - start_time);
		start_time = std::chrono::steady_clock::now();
	}
	return std::pair<std::vector<int>, double>(solution, best_path_length);
}

double AntColonySimulation::total_pheromone_distance_product(std::vector<std::vector<double>>* pheromone_graph, int current_node, std::vector<bool>* visited_nodes, double alpha, double beta)
{
	double total_sum = 0;
	int no_nodes = pheromone_graph->size();
	for (int neighbour_index = 0; neighbour_index < no_nodes; neighbour_index++)
	{
		if (!(*visited_nodes)[neighbour_index] && neighbour_index != current_node)
		{
			double pheromones_strength = (*pheromone_graph)[current_node][neighbour_index];
			double heuristic_value = 1 / graph[current_node][neighbour_index];
			total_sum += std::pow(pheromones_strength, alpha) * std::pow(heuristic_value, beta);
		}
	}
	return total_sum;
}

void AntColonySimulation::update_pheromones(std::vector<std::vector<double>>* pheromones_graph, std::map<std::pair<int, int>, double>* edge_pheromone_map, double rho)
{
	int no_nodes = graph.size();
	for (int i = 0; i < no_nodes; i++)
	{
		for (int j = 0; j < no_nodes; j++)
		{
			if (i != j)
			{
				(*pheromones_graph)[i][j] *= (1 - rho);
				auto edge = std::pair<int, int>(std::min(i, j), std::max(i, j));
				(*pheromones_graph)[i][j] += (*edge_pheromone_map)[edge];
			}
		}
	}
}
