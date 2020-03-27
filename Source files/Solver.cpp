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

	while (temperature > 0.0001)
	{
		// Update best solution so far
		if (length_current < minimal_total_weight)
		{
			minimal_total_weight = length_current;
			solution = order_of_visiting;
		}

		// Choose one neighboring state at random
		int rand_index = rand() % permutation_length;

		//Swap two adjacent elements, if rand_index is last element, swap first and last
		std::swap(order_of_visiting[rand_index], order_of_visiting[(rand_index + 1) % permutation_length]);
		auto neighboring_state = order_of_visiting;
		std::swap(order_of_visiting[rand_index], order_of_visiting[(rand_index + 1) % permutation_length]);
		
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

		// Draw current order
		graphics->update_graph(order_of_visiting, solution);
		std::this_thread::sleep_for(std::chrono::milliseconds(Globals::draw_delay));

		// Change state to neighboring state if probability is greater than the chosen random number
		if (probability_of_acceptance > rand_double)
		{
			order_of_visiting = neighboring_state;
			length_current = length_of_path(order_of_visiting);
		}

		// Reduce temperature
		temperature *= temp_reduction_constant;
	}

	auto order_and_distance = std::pair<std::vector<int>, int>(solution, minimal_total_weight);
	return order_and_distance;
}