#include "File.h"

namespace asio
{
	File::File(const char* filename)
	{
		init(filename);
	}

	File::~File()
	{
		if (fp)
		{
			fclose(fp);
			fp = nullptr;
		}
		//if (fp != INVALID_HANDLE_VALUE)
		//{
		//	CloseHandle(fp);
		//}
		//if (overlapped.hEvent != INVALID_HANDLE_VALUE)
		//{
		//	CloseHandle(overlapped.hEvent);
		//}
	}

	bool File::init(const char* filename)
	{
		//fp = fopen(filename, "rb");
		fopen_s(&fp, filename, "rb");

		if (!fp)
		{
			printf("could not open file %s\n", filename);
			return false;
		}

		fseek(fp, SEEK_END, 0);
		file_len = (size_t)ftell(fp);
		fseek(fp, SEEK_SET, 0);

		data.reserve(file_len);

		return true;
	}

	bool File::isValid() const
	{
		return fp;
	}

	bool File::poll()
	{
		if (!isValid()) return true;

		constexpr size_t chunk_size = 16384;
		size_t old_size = data.size();
		data.resize(data.size() + chunk_size);

		uint8_t* beg = &data[old_size];

		size_t read = fread(beg, 1, chunk_size, fp);
		if (read != chunk_size)
		{
			return true;
		}

		return data.size() == file_len;
	}

	std::vector<uint8_t>&& File::getData()
	{
		return std::move(data);
	}
} // namespace asio