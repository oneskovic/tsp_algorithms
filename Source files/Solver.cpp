#include "..//Header files/Solver.h"

/*
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
		if (should_draw)
		{
			graphics->update_graph(order_of_visiting, solution);
			std::this_thread::sleep_for(std::chrono::milliseconds(Globals::draw_delay));
		}
	} while (std::next_permutation(order_of_visiting.begin(), order_of_visiting.end()));

	auto order_and_distance = std::pair<std::vector<int>, int>(solution, minimal_total_weight);
	return order_and_distance;
}*/

void Solver::add_logger(Logger* logger)
{
	this->logger = logger;
}

double Solver::length_of_path(std::vector<int> order)
{
	double path_length = 0;
	int no_nodes = order.size();
	for(size_t i = 0; i < no_nodes; i++)
		path_length += graph[order[i]][order[(i + 1) % no_nodes]];
	return path_length;
}

std::vector<int> Solver::opt2_swap(std::vector<int>* permutation, int i, int k)
{
	std::vector<int> new_permutation(permutation->size());

	std::copy(permutation->begin(), permutation->begin() + i, new_permutation.begin());
	
	std::reverse_copy(permutation->begin() + i, permutation->begin() + k+1, new_permutation.begin() + i);
	std::copy(permutation->begin() + k + 1, permutation->end(), new_permutation.begin() + k + 1);
	return new_permutation;
}

void Solver::log_solution(std::vector<int> new_permutation, double new_result, std::chrono::nanoseconds solving_time)
{
	if (new_result < best_result)
	{
		best_permutation = new_permutation;
		best_result = new_result;
	}
	auto ms_count = std::chrono::duration_cast<std::chrono::microseconds>(solving_time).count();
	std::string log_line = "Length: " + std::to_string(new_result) + " Time: " + std::to_string(ms_count/1e6) + "\n";
	logger->log_to_console(log_line);

	logger->log_to_graphics(&new_permutation,&best_permutation);

	logger->log_to_file(log_line);
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
