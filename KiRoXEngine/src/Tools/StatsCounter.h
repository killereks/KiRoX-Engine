#pragma once

#include <unordered_map>

class StatsCounter
{
	std::unordered_map<const char*, int> counter;

	static StatsCounter* instance;

public:
	static StatsCounter* GetInstance()
	{
		if (instance == nullptr)
		{
			instance = new StatsCounter();
		}
		return instance;
	}

	void DeleteAll();
	void Reset();
	void IncreaseCounter(const char* label);
	void IncreaseCounter(const char* label, int amount);

	int GetCounter(const char* label);
};

