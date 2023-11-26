#pragma once

#include <chrono>

class Stopwatch
{
	std::chrono::steady_clock::time_point begin;
	
	std::string name;

public:
	Stopwatch(std::string name);
	~Stopwatch();
};

