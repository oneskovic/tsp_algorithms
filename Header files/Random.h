#pragma once
#include <map>
#include <random>
#include <SFML\System\Vector2.hpp>
class Random
{
public:
	Random(size_t seed = std::random_device()());
	double get_double(double lower_bound, double upper_bound);
	int get_int(int lower_bound, int upper_bound);
	sf::Vector2f get_point(double min_x, double max_x, double min_y, double max_y);
	std::mt19937 mersenne_twister;
private:
	std::map<std::pair<int, int>, std::uniform_int_distribution<int>> integer_distributions;
	std::map<std::pair<double, double>, std::uniform_real_distribution<double>> real_distributions;
};

