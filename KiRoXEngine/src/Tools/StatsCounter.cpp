#include "StatsCounter.h"

#include <iostream>

StatsCounter* StatsCounter::instance = nullptr;

void StatsCounter::DeleteAll()
{
	counterVector.clear();
}

void StatsCounter::Reset()
{
	for (std::tuple<const char*, int>& element : counterVector)
	{
		element = std::make_tuple(std::get<0>(element), 0);
	}
}

void StatsCounter::IncreaseCounter(const char* label)
{
	IncreaseCounter(label, 1);
}

void StatsCounter::IncreaseCounter(const char* label, int amount)
{
	for (std::tuple<const char*, int>& element : counterVector)
	{
		if (strcmp(std::get<0>(element), label) == 0)
		{
			std::get<1>(element) += amount;
			return;
		}
	}

	counterVector.push_back(std::make_tuple(label, amount));
}

void StatsCounter::SetCounter(const char* label, int value)
{
	for (std::tuple<const char*, int>& element : counterVector)
	{
		if (strcmp(std::get<0>(element), label) == 0)
		{
			std::get<1>(element) = value;
			return;
		}
	}

	counterVector.push_back(std::make_tuple(label, value));
}

int StatsCounter::GetCounter(const char* label)
{
	for (std::tuple<const char*, int>& element : counterVector){
		if (strcmp(std::get<0>(element), label) == 0)
		{
			return std::get<1>(element);
		}
	}

	return -1;
}
