#include "Stopwatch.h"

#include <iostream>

Stopwatch::Stopwatch(std::string name)
{
	begin = std::chrono::steady_clock::now();

	this->name = name;
}

Stopwatch::~Stopwatch()
{
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();

	std::cout << "[" << name << "] - " << duration * 0.001f << " milliseconds" << std::endl;
}
