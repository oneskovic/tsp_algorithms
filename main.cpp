#include <SFML/Graphics.hpp>
#include "Header files/Graphics.h"
#include "Header files/Globals.h"
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
    Graphics g = Graphics(&window,nodes);
    g.update_graph(order);
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
        window.display();
    }

    return 0;
}