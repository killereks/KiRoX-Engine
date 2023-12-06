#include "StatsCounter.h"

#include <iostream>

StatsCounter* StatsCounter::instance = nullptr;

void StatsCounter::DeleteAll()
{
	counter.clear();
}

void StatsCounter::Reset()
{
	for (auto element : counter)
	{
		counter[element.first] = 0;
	}
}

void StatsCounter::IncreaseCounter(const char* label)
{
	IncreaseCounter(label, 1);
}

void StatsCounter::IncreaseCounter(const char* label, int amount)
{
	if (counter.find(label) == counter.end())
	{
		counter[label] = amount;
		return;
	}
	counter[label] += amount;
}

int StatsCounter::GetCounter(const char* label)
{
	if (counter.find(label) == counter.end())
	{
		return -1;
	}

	return counter[label];
}
