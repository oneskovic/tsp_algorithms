#include "..\Header files\ParameterFinder.h"

ParameterFinder::ParameterFinder(Solver* algorithm)
{
	this->algorithm = algorithm;
}

void ParameterFinder::add_parameter_bounds(std::vector<std::pair<double, double>> parameter_bounds)
{
	this->parameter_bounds = parameter_bounds;
}

void ParameterFinder::find_parameters(int iteration_number, int reps_number, int n, std::string output_path, std::string parameters_path)
{
    auto log_distance_time = Logger::Logger();
    auto log_parameters = Logger::Logger();
    log_distance_time.add_file(output_path);
    log_parameters.add_file(parameters_path);

	for (int iteration = 0; iteration < iteration_number; iteration++)
	{
        // Create random parameters
        std::vector<double> random_parameters;
		for (auto parameter_bound : parameter_bounds)
		{
			double min = parameter_bound.first;
			double max = parameter_bound.second;
			double rand_parameter = Globals::random_generator.get_double(min, max);
            random_parameters.push_back(rand_parameter);
		}

        this->algorithm->parameters = random_parameters;

        double avg_distance = 0;
        double avg_time = 0;
        for (int reps = 0; reps < reps_number; reps++)
        {
            // Generate random graph
            auto points = std::vector<sf::Vector2f>(n);
            for (size_t i = 0; i < n; i++)
                points[i] = Globals::random_generator.get_point(0, Globals::window_width / 2, 0, Globals::window_height);
            auto start_time = std::chrono::steady_clock::now();
            auto order_and_distance = algorithm->solve(convert_points_to_graph1(points));
            auto end_time = std::chrono::steady_clock::now();

            double distance = order_and_distance.second;
            auto time = (end_time - start_time).count() / 1e9;
            avg_distance += distance;
            avg_time += time;
        }
        avg_distance /= reps_number;
        avg_time /= reps_number;

        // Logging
        std::cout << "Iteration " << iteration << ": Average distance: " << avg_distance << " Average time: " << avg_time << "\n";
        log_distance_time.log_to_file(std::to_string(avg_distance) + " " + std::to_string(avg_time) + "\n");
        for (auto rand_parameter : random_parameters)
        {
            log_parameters.log_to_file(std::to_string(rand_parameter) + " ");
        }
        log_parameters.log_to_file("\n");
	}
}

std::vector<std::vector<double>> ParameterFinder::convert_points_to_graph1(std::vector<sf::Vector2f> points)
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