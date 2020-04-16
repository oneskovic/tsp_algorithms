#include <SFML/Graphics.hpp>
#include "Header files/SimulatedAnnealing.h"
#include "Header files/GeneticAlgorithm.h"
#include "Header files/AntColonySimulation.h"
#include <chrono>
#include <iostream>
#include <random>
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
int main()
{

    Globals::random_generator = Random();
    int n = 100;
    auto points = std::vector<sf::Vector2f>(n);
    for (size_t i = 0; i < n; i++)
      points[i] = Globals::random_generator.get_point(0, Globals::window_width/2, 0, Globals::window_height);
    
    Logger logger = Logger();
    auto algorithm = AntColonySimulation();
    algorithm.add_logger(&logger);
    algorithm.solve(convert_points_to_graph(points));
    return 0;
}