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