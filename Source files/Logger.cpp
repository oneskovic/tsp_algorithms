#include "..\Header files\Logger.h"


Logger::Logger()
{
	graphics_enabled = false;
}

void Logger::add_graphics(Graphics* graphics)
{
	graphics_enabled = true;
	this->graphics = graphics;
}

void Logger::add_file(std::string file_path)
{
	file_enabled = true;
	this->file_path = file_path;
}


void Logger::disable_graphics()
{
	graphics_enabled = false;
}

void Logger::enable_graphics()
{
	if (graphics != nullptr)
		graphics_enabled = true;
}

void Logger::log_to_file(std::string text_to_log)
{
	if (file_enabled)
	{
		std::ofstream out_stream(file_path, std::ios::app);
		out_stream << text_to_log;
		out_stream.close();
	}
}

void Logger::log_to_graphics(std::vector<int>* permutation, std::vector<int>* best_permutation)
{
	if (graphics_enabled)
	{
		graphics->update_graph(*permutation, *best_permutation);
	}
}

void Logger::log_to_console(std::string text_to_log)
{
	std::cout << text_to_log;
}
