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
double temp_reduction_constant,double probability_constant)
{
	int permutation_length = graph.size();
	std::vector<int> order_of_visiting(permutation_length);
	// Fills order_of_visiting with numbers from 0 to n - 1
	std::iota(order_of_visiting.begin(), order_of_visiting.end(), 0);
	double length_current = length_of_path(order_of_visiting);

	double minimal_total_weight = INFINITY;
	double temperature = initial_temperature;
	std::vector<int> solution;

	while (temperature > 0.1)
	{
		int number_of_changes = 100 * permutation_length;
		for (int i = 0; i < number_of_changes; i++)
		{
			// Choose one neighboring state at random
			int rand_index1 = rand() % permutation_length;
			int rand_index2 = rand() % permutation_length;

			//Swap two adjacent elements, if rand_index is last element, swap first and last
			std::swap(order_of_visiting[rand_index1], order_of_visiting[(rand_index2) % permutation_length]);
			auto neighboring_state = order_of_visiting;
			std::swap(order_of_visiting[rand_index1], order_of_visiting[(rand_index2) % permutation_length]);

			// Choose random number between 0 and 1
			double rand_double = rand() / (RAND_MAX * 1.0);
			// Evaluate probability function of acceptance
			double length_new = length_of_path(neighboring_state);
			double length_difference = length_current - length_new;
			double probability_of_acceptance;
			if (length_new > length_current)
				probability_of_acceptance = std::exp((length_difference) / (probability_constant * temperature));
			else
				probability_of_acceptance = 1;

			// Change state to neighboring state if probability is greater than the chosen random number
			if (probability_of_acceptance > rand_double)
			{
				order_of_visiting = neighboring_state;
				length_current = length_of_path(order_of_visiting);
			}

			// Update best solution so far
			if (length_current < minimal_total_weight)
			{
				minimal_total_weight = length_current;
				solution = order_of_visiting;
			}

			// Draw current order
			graphics->update_graph(order_of_visiting, solution);
			std::this_thread::sleep_for(std::chrono::milliseconds(Globals::draw_delay));
		}

		// Reduce temperature
		temperature *= temp_reduction_constant;
	}

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
