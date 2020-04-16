#include "..//Header files/Random.h"

Random::Random(size_t seed)
{
	mersenne_twister = std::mt19937(seed);
}

/// <summary>
/// Produces a uniform random double in range [lower_bound,upper_bound]
/// </summary>
double Random::get_double(double lower_bound, double upper_bound)
{
	std::pair<double, double> interval = std::pair<double, double>(lower_bound, upper_bound);
	if (real_distributions.find(interval) == real_distributions.end()) //distribution not found
	{
		//Create new distribution
		real_distributions[interval] = std::uniform_real_distribution<double>(lower_bound, upper_bound);
	}
	return real_distributions[interval](mersenne_twister);
}

/// <summary>
/// Produces a uniform random integer in range [lower_bound,upper_bound]
/// </summary>
int Random::get_int(int lower_bound, int upper_bound)
{
	std::pair<int, int> interval = std::pair<int, int>(lower_bound, upper_bound);
	if (integer_distributions.find(interval) == integer_distributions.end()) //distribution not found
	{
		//Create new distribution
		integer_distributions[interval] = std::uniform_int_distribution<int>(lower_bound, upper_bound);
	}
	return integer_distributions[interval](mersenne_twister);
}

sf::Vector2f Random::get_point(double min_x, double max_x, double min_y, double max_y)
{
	double x = get_double(min_x, max_x);
	double y = get_double(min_y, max_y);
	return sf::Vector2f(x,y);
}
