#pragma once

#include <cassert>
#include <vector>

#include <stack>
#include <string>
#include <unordered_map>

#include <chrono>

class ProfileData {
public:
	char* name;

	int depth;

	std::chrono::high_resolution_clock::time_point startTime;
	std::chrono::high_resolution_clock::time_point endTime;

	double GetDuration() {
		assert(endTime >= startTime);
		// milliseconds
		return std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count() / 1000.0;
	}
};

class Profiler {

	std::stack<std::string> scopeStack;
	std::unordered_map<std::string, ProfileData> profileData;

	std::string currentFilter;
	bool isPaused;

	float timeThreshold;
	bool useTimeThreshold;

	std::chrono::high_resolution_clock::time_point beginFrameTime;
	std::chrono::high_resolution_clock::time_point endFrameTime;

	static Profiler* instance;

public:
	static Profiler* GetInstance() {
		if (instance == nullptr) {
			instance = new Profiler();
		}
		return instance;
	}

	void BeginFrame();

	static void Push(const char* name);
	static void Pop(const char* name);

	void EndFrame();

	const std::unordered_map<std::string, ProfileData>& GetProfileData() const { return profileData; }
};