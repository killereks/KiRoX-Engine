#pragma once

#include <Tools/Profiling/Profiler.h>

class ProfileScope {

	const char* name;

public:
	ProfileScope(const char* name) {
		this->name = name;
		Profiler::Push(name);
	}

	~ProfileScope() {
		Profiler::Pop(name);
	}
};