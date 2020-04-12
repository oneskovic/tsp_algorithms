#include <SFML/Graphics.hpp>
#include "Header files/Graphics.h"
#include "Header files/Globals.h"
#include "Header files/Solver.h"
#include "Header files/Random.h"
#include <chrono>
#include <iostream>
#include <random>


sf::Vector2f get_rand_point()
{
	return sf::Vector2f(rand() % Globals::window_width/2, rand() % Globals::window_height);
}

int main()
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(Globals::window_width, Globals::window_height), "KURAC MOLIM TE <3", sf::Style::Default, settings);
    //Point generation random seeding
    Globals::random_generator = Random();
    srand(time(0));
    int n = 200;
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
    Solver primary_solver = Solver(&g, points);

    std::pair<std::vector<int>, double> order_and_distance;
    //Running ant colony
    order_and_distance = primary_solver.solve_ant_colony_simulation(1,3,1,0.5,90,400,1);
    auto ant_colony_solution = order_and_distance.first;
    std::cout << "Best distance ant colony simulation found: " << order_and_distance.second << "\n";
    std::this_thread::sleep_for(std::chrono::seconds(5));

    //Running simmulated annealing
    //n<=70 30,0.5,1,30,30
    //n<=100 40, 0.6, 1, 30, 30
    order_and_distance = primary_solver.solve_simulated_annealing(50, 0.6, 1, 30, 40);
    auto simulated_annealing_solution = order_and_distance.first;
    std::cout << "Best distance simulated annealing found: " << order_and_distance.second << "\n";
    std::this_thread::sleep_for(std::chrono::seconds(5));

    //Running genetic algorithm
    order_and_distance = primary_solver.solve_genetic_algorithm(30,30,0.1,0.3,0.7,100);
    auto genetic_solution = order_and_distance.first;
    std::cout << "Best distance found by genetic algorithm: " << order_and_distance.second << "\n";
    std::this_thread::sleep_for(std::chrono::seconds(5));

    //g.update_graph(simulated_annealing_solution, genetic_solution, true);

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