#pragma once
#include <SFML/Graphics.hpp>
#include "Graphics.h"
#include "Logger.h"
#include <numeric>
#include <thread>
#include <iostream>
#include <unordered_set>
#include <algorithm>
#include <map>
class Solver 
{
public:
	void add_logger(Logger* logger);
	virtual std::pair<std::vector<int>, double> solve(std::vector<std::vector<double>> graph) = 0;
	std::vector<double> parameters;
	/*Solver(Graphics* graphics, std::vector<sf::Vector2f> points);
	Solver(Graphics* graphics, std::vector<std::vector<double>> input_graph);
	Solver(std::vector<std::vector<double>> input_graph);
	Solver(std::vector<sf::Vector2f> points);*/
	//std::pair<std::vector<int>, double> solve_bruteforce();
protected:
	double length_of_path(std::vector<int> order);
	std::vector<int> optimize_genome_locally(std::vector<int> permutation, int rounds);
	std::vector<int> opt2_swap(std::vector<int>* permutation, int i, int k);
	void log_solution(std::vector<int> new_permutation, double new_result, std::chrono::nanoseconds solving_time);
	Logger* logger;
	std::vector<std::vector<double>> graph;
	double best_result = INFINITY;
	std::vector<int> best_permutation = std::vector<int>();
};