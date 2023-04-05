#include "CodeStyleGenerator.h"
#include "MatchString.h"
#include <unordered_map>
#include <sstream>

static std::unordered_map<std::string, std::string> gCodeStyles = 
{
	{ "b", "#569cd6" },   // blue
	{ "w", "#dadada" },   // white
	{ "n", "#b4b4b4" },   // normal / grey
	{ "d", "#9a9a9a" },   // dark
	{ "g", "#4ec9b0" },   // green
	{ "y", "#dcdcaa" },   // yellow
	{ "c", "#57a64a" },   // comment
	{ "p", "#d8a0df" },   // purple
};

static std::unordered_map<std::string, std::string> gCodeAutoStyles =
{
	{"(", "n"}, {")", "n"},
	{"{", "n"}, {"}", "n"},
	{"{", "n"}, {"}", "n"},
	{"<", "n"}, {">", "n"},
	{"+", "n"}, {"-", "n"}, {"*", "n"}, {"/", "n"}, {"^", "n"}, {"!", "n"}, {"=", "n"},
	{"&", "n"}, {"*", "n"}, {"&", "n"}, {"|", "n"}, //{"&&", "n"}, {"||", "n"}, // need to fix issue in matchString to enable these
	{",", "n"}, {".", "n"}, {":", "n"}, {";", "n"},

	{"int ", "b"},
	{"void ", "b"},

	{"m_", "w"},
	{"return ", "p"},
	{"std", "w"},
	{"delete{}", "b"}
};

static const char gCodeStyleDelim = '`';

std::string InsertAutoStyleTags(const std::string codeTemplate)
{
	std::stringstream out;
	bool inComment = false;

	for (size_t i = 0; i < codeTemplate.length(); i++)
	{
		char c = codeTemplate.at(i);

		// check for special comment case

		if (inComment) // if in comment, apply to styling and exit state on newline
		{
			if (c == '\n')
				inComment = false;
		}

		else if (i + 1 < codeTemplate.length()) // if not in a comment, check if it should enter state
		{
			char c1 = codeTemplate.at(i + 1);

			if (c == '/' && c1 == '/')
			{
				inComment = true;
				out << "`c`";
			}
		}

		if (inComment) // don't apply style if in comment
			continue;

		std::string match = MatchString(gCodeAutoStyles, codeTemplate, i);

		if (match.length() > 0)
		{
			out << "`" << gCodeAutoStyles.at(match) << "`" << match;
			i += match.length() - 1;
		}

		else
		{
			out << c;
		}
	}

	return out.str();
}

void GenerateCodeStyle(std::ostream& out, const std::string& codeTemplateIn)
{
	// apply auto styling

	std::string codeTemplate = InsertAutoStyleTags(codeTemplateIn);

	// apply styles to template

	std::stringstream acc;
	bool accHasContent = false;
	
	for (size_t i = 0; i < codeTemplate.length(); i++)
	{
		char c = codeTemplate.at(i);

		if (c != gCodeStyleDelim) // if the char is not a delimiter, accumulate
		{
			acc << c;
			accHasContent = true;

			continue;
		}

		// char is delimiter

		if (accHasContent)      // if we have content, save
		{
			out << acc.rdbuf() << "</span>";
			acc = {};
			accHasContent = false;
		}

		i += 1; // advance 1 to skip delimiter  -----> ^^^^

		std::string match = MatchString(gCodeStyles, codeTemplate, i);

		if (match.length() > 0)
		{
			std::string style = gCodeStyles.at(match);
			acc << "<span style=\"color: " << style << "\">";

			// skip match and its ending delimiter
			i += match.length();
		}
	}

	// write final output

	if (accHasContent)
		out << acc.rdbuf() << "</span>";
}
