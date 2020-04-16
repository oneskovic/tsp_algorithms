#include "..\Header files\SimulatedAnnealing.h"

SimulatedAnnealing::SimulatedAnnealing(double initial_temperature, double temp_reduction_constant, double probability_constant, int opt2_rounds, int number_of_neighbors)
{
	this->initial_temperature = initial_temperature;
	this->temp_reduction_constant = temp_reduction_constant;
	this->probability_constant = probability_constant;
	this->opt2_rounds = opt2_rounds;
	this->number_of_neighbors = number_of_neighbors;
}

std::pair<std::vector<int>, double> SimulatedAnnealing::solve(std::vector<std::vector<double>> graph)
{
	this->graph = graph;
	auto start_time = std::chrono::steady_clock::now();
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
		changes_accepted = 0;
		for (int i = 0; i < number_of_neighbors; i++)
		{
			// Choose one neighboring state at random
			int rand_index1 = Globals::random_generator.get_int(0, permutation_length - 1);
			int rand_index2 = Globals::random_generator.get_int(0, permutation_length - 1);

			//Reject the neighboring state if it is the same as the current state
			if (rand_index1 != rand_index2)
			{
				//Swap two adjacent elements, if rand_index is last element, swap first and last
				std::swap(order_of_visiting[rand_index1], order_of_visiting[(rand_index2)]);
				auto neighboring_state = order_of_visiting;
				std::swap(order_of_visiting[rand_index1], order_of_visiting[(rand_index2)]);

				//Perform 2-opt improvment on the neighbor
				neighboring_state = optimize_genome_locally(neighboring_state, 2 - opt2_rounds);

				// Choose random number between 0 and 1
				double rand_double = Globals::random_generator.get_double(0, 1);
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
		}
		auto current_time = std::chrono::steady_clock::now();
		log_solution(order_of_visiting, length_current, current_time - start_time);
		start_time = std::chrono::steady_clock::now();
		// Reduce temperature
		temperature *= temp_reduction_constant;
	} while (changes_accepted > 0);

	auto order_and_distance = std::pair<std::vector<int>, int>(solution, minimal_total_weight);
	return order_and_distance;
}
