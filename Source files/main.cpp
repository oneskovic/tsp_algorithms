#include <SFML/Graphics.hpp>
#include "Header files/Graphics.h"
#include "Header files/Globals.h"
#include "Header files/Solver.h"
#include <chrono>
#include <iostream>
#include <random>


sf::Vector2f get_rand_point()
{
	return sf::Vector2f(rand() % Globals::window_width/2, rand() % Globals::window_height);
}
void seed_random(size_t seed = std::random_device()())
{
    Globals::random_generator = std::mt19937(seed);
    srand(seed);
}

int main()
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(Globals::window_width, Globals::window_height), "KURAC MOLIM TE <3", sf::Style::Default, settings);
    //Point generation random seeding
    srand(342);

    int n = 80;
    auto points = std::vector<sf::Vector2f>(n);
    std::vector<int> order = std::vector<int>(n);
    for (size_t i = 0; i < n; i++)
    {
      // Generating random points
      points[i] = get_rand_point();
      // Point input via console
      /*std::cin >> nodes[i].x;
      std::cin >> nodes[i].y;*/
      order[i] = i;
    }
    order.push_back(0);

    Graphics g = Graphics(&window,points);
    //Solver random seeding
    seed_random();
    Solver primary_solver = Solver(&g, points);

    std::pair<std::vector<int>, double> order_and_distance;
    /*//Running ant colony
    order_and_distance = primary_solver.solve_ant_colony_simulation();
    auto ant_colony_solution = order_and_distance.first;
    std::cout << "Best distance ant colony simulation found: " << order_and_distance.second << "\n";
    std::this_thread::sleep_for(std::chrono::seconds(5));*/

    //Running simmulated annealing
    order_and_distance = primary_solver.solve_simulated_annealing();
    auto simulated_annealing_solution = order_and_distance.first;
    std::cout << "Best distance simulated annealing found: " << order_and_distance.second << "\n";
    std::this_thread::sleep_for(std::chrono::seconds(5));

    //Running genetic algorithm
    order_and_distance = primary_solver.solve_genetic_algorithm();
    auto genetic_solution = order_and_distance.first;
    std::cout << "Best distance found by genetic algorithm: " << order_and_distance.second << "\n";
    std::this_thread::sleep_for(std::chrono::seconds(5));

    g.update_graph(simulated_annealing_solution, genetic_solution, true);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
    }
    return 0;
}