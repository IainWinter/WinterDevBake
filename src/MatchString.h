#pragma once

#include <string>
#include <vector>
#include <unordered_map>

//	Returns the longest string in options that matches the substring of text starting at currentIndex
//	If no options are met, the empty string is returned
//
std::string MatchString(const std::vector<std::string>& options, const std::string& text, size_t currentIndex);

//	Take a map and check just the first letter of all keys, only copy those which have matching first characters
//	into a vector for MatchString(vector, string, int) to process
//
template<typename _t>
std::string MatchString(const std::unordered_map<std::string, _t>& options, const std::string& text, size_t currentIndex)
{
	std::vector<std::string> avalible;

	for (const auto& [opt, _] : options)
		if (opt.at(0) == text.at(currentIndex))
			avalible.push_back(opt);

	return MatchString(avalible, text, currentIndex);
}