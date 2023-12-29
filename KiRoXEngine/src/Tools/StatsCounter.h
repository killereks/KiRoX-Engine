#pragma once

#include <vector>
#include <tuple>

class StatsCounter
{
	std::vector<std::tuple<const char*, int>> counterVector;

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

	void SetCounter(const char* label, int value);

	int GetCounter(const char* label);
};

