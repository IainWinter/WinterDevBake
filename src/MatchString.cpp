#include "MatchString.h"
#include "Util.h"

std::string MatchString(const std::vector<std::string>& options, const std::string& text, size_t currentIndex)
{
	if (options.size() == 0)
		return "";

	// make a copy to remove options, not the best but works easy
	std::vector<std::string> avalible = options;

	// offset of the lookahead
	int window = 0;

	while (avalible.size() > 0 
		&& avalible.at(0).length() > window) // loop until all options are exhausted 
	{
		for (size_t avalibleStyleIndex = 0; avalibleStyleIndex < avalible.size(); avalibleStyleIndex++)
		{
			char currentChar = text.at(currentIndex + window);
			char mustMatch = avalible[avalibleStyleIndex][window];

			bool match = currentChar == mustMatch;

			if (!match && mustMatch == ' ')    // if this must match whitespace, allow matches 
				match = !isalpha(currentChar); // for everything that isn't a letter

			if (!match)
				pop_erase(avalible, &avalibleStyleIndex);
		}

		window += 1;

		if (currentIndex + window >= text.length()) // if at end of text, stop matching
		{
			break;
		}
	}

	if (avalible.size() == 0) // exit if no options
		return "";

	if (avalible.at(0).length() != window) // exit if not fully matching
		return "";

	return avalible.at(0);
}