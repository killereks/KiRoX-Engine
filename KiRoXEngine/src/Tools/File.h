#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <vector>

//#define WIN32_LEAN_AND_MEAN
//#include <windows.h>

namespace asio
{
	struct File
	{
		File() = default;
		File(const char* filename);
		~File();

		bool init(const char* filename);
		bool isValid() const;
		// returns true when finished
		bool poll();
		std::vector<uint8_t>&& getData();

	private:
		FILE* fp = nullptr;
		//HANDLE fp = INVALID_HANDLE_VALUE;
		//OVERLAPPED overlapped = {};
		std::vector<uint8_t> data;
		size_t file_len = 0;
	};
} // namespace asio