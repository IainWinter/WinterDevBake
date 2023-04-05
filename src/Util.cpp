#include "Util.h"
#include <sstream>
#include <fstream>

std::vector<std::string> split_string(const std::string& str, char delim)
{
	std::vector<std::string> strings;
	std::stringstream cur;

	for (int i = 0; i < str.length(); i++)
	{
		char inspect = str.at(i);

		if (inspect == delim)
		{
			strings.emplace_back(trim(cur.str()));
			cur = {};
		}

		else // don't write to string if the char is the delimiter
		{
			cur << inspect;
		}
	}

	strings.emplace_back(trim(cur.str()));

	return strings;
}

std::tuple<std::string, std::string> split_string2(const std::string& str, char delim)
{
	std::vector<std::string> v = split_string(str, delim);

	if (v.size() != 2) // should log error
		return {};

	return { v.at(0), v.at(1) };
}

std::string trim(const std::string& str, const std::string& whitespace)
{
	const auto strBegin = str.find_first_not_of(whitespace);
	if (strBegin == std::string::npos)
		return ""; // no content

	const auto strEnd = str.find_last_not_of(whitespace);
	const auto strRange = strEnd - strBegin + 1;

	return str.substr(strBegin, strRange);
}

std::string load_file(const std::string& filepath)
{
	std::ifstream t(filepath);
	std::stringstream buffer;
	buffer << t.rdbuf();

	return buffer.str();
}

int panic(PANIC p, const char* arg)
{
	switch (p)
	{
		case FILE_NOT_FOUND:
			printf("File not found. %s\n", arg);
			break;
		case PANIC::FUNCTION_NOT_FOUND:
			printf("Function not found. %s\n", arg);
			break;
		case PANIC::INVALID_TEMPLATE:
			printf("Invalid template. %s\n", arg);
			break;
		case PANIC::INVALID_ARGS:
			printf("Invalid arguments. %s\n", arg);
			break;
		case PANIC::DATABASE_ERROR:
			printf("Database error. %s\n", arg);
			break;
		default:
			printf("unknown error. %s\n", arg);
			break;
	}

	return (int)p;
}
