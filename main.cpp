#include <SFML/Graphics.hpp>
#include "Header files/Graphics.h"
#include "Header files/Globals.h"
#include "Header files/Solver.h"
#include "Header files/Random.h"
#include <chrono>
#include <iostream>
#include <random>

int main()
{
   /* sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(Globals::window_width, Globals::window_height), "TSP solver", sf::Style::Default, settings);*/
    //Point generation random seeding
    Globals::random_generator = Random(420);
    int n = 50;
    auto points = std::vector<sf::Vector2f>(n);
    for (size_t i = 0; i < n; i++)
      points[i] = Globals::random_generator.get_point(0, Globals::window_width/2, 0, Globals::window_height);
    
    //Graphics g = Graphics(&window,points);
    Solver primary_solver = Solver(points);

    std::pair<std::vector<int>, double> order_and_distance;
    //Running ant colony
    order_and_distance = primary_solver.solve_ant_colony_simulation(1.5, 2, 1, 0.5, 50, 100, 1);
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


    return 0;
}