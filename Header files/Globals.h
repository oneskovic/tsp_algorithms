#pragma once
#include <SFML/Graphics.hpp>
class Globals
{
public:
	static sf::Color window_background;
	static sf::Color edge_color, optimal_edge_color;
	static sf::Color node_color, optimal_node_color;
	static int window_width;
	static int window_height;
	static int draw_delay; //Drawing delay in ms
	static double node_radius;
	static double edge_thickness;
};

