#include "../Header files/Graphics.h"

void Graphics::update_graph(std::vector<int> points_order, std::vector<int> optimal_order)
{
	render_window->clear(Globals::window_background);
	this->points_order = std::vector<int>(points_order.begin(),points_order.end());
	auto points_left = std::vector<sf::Vector2f>(points.begin(), points.end());
	edge_color = Globals::edge_color;
	node_color = Globals::node_color;
	draw_graph();
	for (size_t i = 0; i < points.size(); i++)
		points[i].x += Globals::window_width / 2;
	this->points_order = std::vector<int>(optimal_order.begin(), optimal_order.end());
	edge_color = Globals::optimal_edge_color;
	node_color = Globals::optimal_node_color;
	draw_graph();
	points = std::vector<sf::Vector2f>(points_left.begin(), points_left.end());
	render_window->display();
}

void Graphics::draw_graph()
{
	//Draw all edges
	for (size_t i = 1; i < points.size(); i++)
	{
		int point1_index = points_order[i - 1];
		int point2_index = points_order[i];
		draw_line(points[point1_index], points[point2_index], edge_thickness);
	}
	int first_point_index = points_order[0];
	int last_point_index = points_order[points_order.size() - 1];
	draw_line(points[last_point_index], points[first_point_index], edge_thickness);
	//Draw all nodes
	for (size_t i = 0; i < points.size(); i++)
	{
		sf::CircleShape node = sf::CircleShape(node_radius);
		node.move(points[i].x - node_radius, points[i].y - node_radius);
		node.setFillColor(node_color);
		(*render_window).draw(node);
	}
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
	line_rectangle.setFillColor(edge_color);
	(*render_window).draw(line_rectangle);
}

Graphics::Graphics(sf::RenderWindow* window, std::vector<sf::Vector2f> points)
{
	this->points = points;
	render_window = window;
	edge_thickness = Globals::edge_thickness;
	node_radius = Globals::node_radius;
	node_color = Globals::node_color;
	edge_color = Globals::edge_color;
}
