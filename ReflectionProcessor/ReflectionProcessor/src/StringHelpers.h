#pragma once
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

namespace StringHelpers {

    const char* ws = " \t\n\r\f\v";

    // trim from end of string (right)
    inline std::string& rtrim(std::string& s, const char* t = ws)
    {
        s.erase(s.find_last_not_of(t) + 1);
        return s;
    }

    // trim from beginning of string (left)
    inline std::string& ltrim(std::string& s, const char* t = ws)
    {
        s.erase(0, s.find_first_not_of(t));
        return s;
    }

    // trim from both ends of string (right then left)
    inline std::string& trim(std::string& s, const char* t = ws)
    {
        return ltrim(rtrim(s, t), t);
    }

    inline std::vector<std::string> split(const std::string& s, char delimiter)
	{
		std::vector<std::string> tokens;
		std::string token;
		std::istringstream tokenStream(s);
		while (std::getline(tokenStream, token, delimiter))
		{
			tokens.push_back(token);
		}
		return tokens;
	}

    inline std::vector<std::string> split(std::string& s, std::string delimiter)
	{
		std::vector<std::string> tokens;
		size_t pos = 0;
		std::string token;
		while ((pos = s.find(delimiter)) != std::string::npos)
		{
			token = s.substr(0, pos);
			tokens.push_back(token);
			s.erase(0, pos + delimiter.length());
		}
		tokens.push_back(s);
		return tokens;
	}

    inline std::string GetBetween(std::string line, std::string left, std::string right) {
        std::size_t start = line.find(left);

        if (start == std::string::npos) {
            return "";
        }

        start += left.length();

        std::size_t end = line.find(right, start);
        if (end == std::string::npos) {
			return "";
		}

        return line.substr(start, end - start);
    }

    inline std::string GetAfter(std::string line, std::string left) {
		std::size_t start = line.find(left);

        if (start == std::string::npos) {
			return "";
		}

		start += left.length();

		return line.substr(start);
	}

    inline std::string Replace(std::string line, std::string from, std::string to) {
        size_t start_pos = 0;
        while ((start_pos = line.find(from, start_pos)) != std::string::npos) {
			line.replace(start_pos, from.length(), to);
			start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
		}

		return line;
    }

    // for example if our path is MyDocuments/Project
    // and then our file is MyDocuments/Project/MyClass.h
    // this should return MyClass.h
    // if our file is MyDocuments/Project/MyFolder/MyClass.h
    // this should return MyFolder/MyClass.h
    inline std::string GetRelativePath(std::string basePath, std::string fullPath) {
        std::string relativePath = fullPath;
        relativePath = Replace(relativePath, basePath, "");
        relativePath = Replace(relativePath, "\\", "/");
        relativePath = trim(relativePath, "/");
        return relativePath;
	}

};