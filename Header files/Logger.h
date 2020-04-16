#pragma once
#include "Graphics.h"
#include <iostream>
#include <fstream>
#include <chrono>
class Logger
{
public:
	Logger();
	void add_graphics(Graphics* graphics);
	void add_file(std::string file_path);
	void disable_graphics();
	void enable_graphics();
	void log_to_file(std::string text_to_log);
	void log_to_graphics(std::vector<int>* permutation, std::vector<int>* best_permutation);
	void log_to_console(std::string text_to_log);
private:
	bool graphics_enabled, file_enabled;
	std::string file_path;
	Graphics* graphics = nullptr;
};

