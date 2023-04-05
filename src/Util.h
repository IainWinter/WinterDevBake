#pragma once

#include <string>
#include <vector>
#include <tuple>

template<typename _t>
void pop_erase(std::vector<_t>& vec, size_t* index)
{
	vec.at(*index) = std::move(vec.back());
	vec.pop_back();
	*index -= 1;
}

std::vector<std::string> split_string(const std::string& str, char delim);
std::tuple<std::string, std::string> split_string2(const std::string& str, char delim);

std::string trim(const std::string& str, const std::string& whitespace = " \t");

std::string load_file(const std::string& filepath);

enum PANIC
{
	FILE_NOT_FOUND = 1,
	FUNCTION_NOT_FOUND,
	INVALID_TEMPLATE,
	INVALID_ARGS,
	DATABASE_ERROR
};

int panic(PANIC p, const char* arg);