#include "Graphics.h"

void Graphics::update_graph(std::vector<sf::Vector2f> points, std::vector<int> points_order)
{
	this->points = points; this->points_order = points_order;
}

void Graphics::draw_graph()
{
	for (size_t i = 0; i < points.size(); i++)
	{
		if (i > 0)
			draw_line(points[i - 1], points[i], edge_thickness);	
		sf::CircleShape node = sf::CircleShape(node_radius);
		node.move(points[i].x - node_radius, points[i].y - node_radius);
		(*render_window).draw(node);
	}
	draw_line(points[points.size() - 1], points[0], edge_thickness);
}

sf::Vector2f Graphics::normalize_vector(sf::Vector2f vector)
{
	double length = std::sqrt(vector.x * vector.x + vector.y * vector.y);
	return sf::Vector2f(vector.x / length, vector.y / length);
}
sf::Vector2f Graphics::mul_vector_scalar(sf::Vector2f vector, double scalar)
{
	return sf::Vector2f(vector.x * scalar, vector.y * scalar);
}

void Graphics::draw_line(sf::Vector2f point1, sf::Vector2f point2, double thickness)
{
	sf::Vector2f line = point2 - point1;
	sf::Vector2f normal = normalize_vector(sf::Vector2f(-line.y, line.x));
	sf::Vector2f half_side = mul_vector_scalar(normal, thickness);

	auto line_rectangle = sf::ConvexShape(4);
	line_rectangle.setPoint(0,point1 - half_side);
	line_rectangle.setPoint(1, point1 + half_side);
	line_rectangle.setPoint(2, point2 + half_side);
	line_rectangle.setPoint(3, point2 - half_side);
	(*render_window).draw(line_rectangle);
}

Graphics::Graphics(sf::RenderWindow* window)
{
	render_window = window;
	edge_thickness = 2;
	node_radius = 5;
}
