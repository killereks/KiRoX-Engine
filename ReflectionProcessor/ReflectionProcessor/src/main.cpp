#include <cstdlib>
#include <iomanip>
#include <iostream>

#include <chrono>

#include "Reflection.h"

int main(int argc, char* argv[])
{
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	std::string path = std::string(argv[0]);

	// remove executable name from path
	#if defined(_WIN32) || defined(_WIN64)
	path = path.substr(0, path.find_last_of("\\/"));
	#else
	path = path.substr(0, path.find_last_of("/"));
	#endif

	std::cout << "Begin Reflecting: " << path << std::endl;

	Reflection refl(path.c_str());
	refl.ReflectRecursive(path.c_str());

	std::cout << "End Reflecting: " << path << std::endl;

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	std::cout << "== ReflectionProcessor finished ==\n";
	std::cout << "Total time taken... " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms\n";

	return 0;
}