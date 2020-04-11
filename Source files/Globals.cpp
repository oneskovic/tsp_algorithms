#include "../Header files/Globals.h"
sf::Color Globals::window_background = sf::Color(2, 128, 144);
sf::Color Globals::edge_color = sf::Color(255, 0, 0);
sf::Color Globals::node_color = sf::Color(255, 255, 255);
sf::Color Globals::optimal_edge_color = sf::Color(0, 255, 0);
sf::Color Globals::optimal_node_color = sf::Color(255, 255, 255);
int Globals::window_width = 1000;
int Globals::window_height = 562;
int Globals::draw_delay = 0;
double Globals::edge_thickness = 2;
double Globals::node_radius = 5.5;
std::mt19937 Globals::random_generator;