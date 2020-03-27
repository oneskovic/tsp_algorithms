#include <SFML/Graphics.hpp>
#include "Header files/Graphics.h"
#include "Header files/Globals.h"
#include "Header files/Solver.h"
#include <chrono>
#include <iostream>


sf::Vector2f get_rand_point()
{
	return sf::Vector2f(rand() % Globals::window_width/2, rand() % Globals::window_height);
}

int main()
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(Globals::window_width, Globals::window_height), "KURAC MOLIM TE <3", sf::Style::Default, settings);
    srand(time(0));
    int n = 30;
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
    Solver primary_solver = Solver(&g, points);
    auto order_and_distance = primary_solver.solve_simulated_annealing(1e3, 1 - 1.0 / (n * n * n));
    std::cout << "Best distance found: " << order_and_distance.second;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
    }
    /*while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.clear(Globals::window_background);
        g.draw_graph();
        window.display();
    }*/

    return 0;
}