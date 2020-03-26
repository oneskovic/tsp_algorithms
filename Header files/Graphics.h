#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
class Graphics
{
public:
	void update_graph(std::vector<sf::Vector2f> points, std::vector<int> points_order);
	void draw_graph();
	void draw_line(sf::Vector2f point1, sf::Vector2f point2, double thickness);
	Graphics(sf::RenderWindow* window);
private:
	sf::RenderWindow* render_window;
	std::vector<sf::Vector2f> points; std::vector<int> points_order;
	double node_radius, edge_thickness;
	sf::Vector2f normalize_vector(sf::Vector2f vector);
	sf::Vector2f mul_vector_scalar(sf::Vector2f vector, double scalar);
};

