#include <SFML/Graphics.hpp>
#include "Globals.h"
#include "Graphics.h"
#include <chrono>
sf::Vector2f get_rand_point()
{
    return sf::Vector2f(rand() % 900, rand() % 500);
}
int main()
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(1000, 562), "SFML works!", sf::Style::Default, settings);
    
    Graphics g = Graphics(&window);
    srand(time(0));
    int n = 10;
    auto nodes = std::vector<sf::Vector2f>(n);
    std::vector<int> order = std::vector<int>(n);
    for (size_t i = 0; i < n; i++)
    {
        nodes[i] = get_rand_point();
        order[i] = i;
    }
    order.push_back(0);
    g.update_graph(nodes, order);
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.clear(Globals::window_background);
        g.draw_graph();
        //g.draw_line(sf::Vector2f(30, 30), sf::Vector2f(500, 300), 5);
        window.display();
    }

    return 0;
}