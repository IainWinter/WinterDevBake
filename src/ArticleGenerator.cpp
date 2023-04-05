#include "ArticleGenerator.h"
#include "MatchString.h"
#include "Util.h"
#include "CodeStyleGenerator.h"

#include <vector>
#include <unordered_map>

void GenerateParagraph(std::ostream& out, const std::string& text);
void GenerateTitle(std::ostream& out, const std::string& text);
void GenerateSubtitle(std::ostream& out, const std::string& text);
void GenerateCodeblock(std::ostream& out, const std::string& text);
void GenerateVideoFrame(std::ostream& out, const std::string& text);

// store a map between tokens and functions to run with their contents

static std::unordered_map<std::string, void(*)(std::ostream&, const std::string&)> gArticleBlocks = 
{
	{ "p",     GenerateParagraph },
	{ "title", GenerateTitle },
	{ "sub",   GenerateSubtitle },
	{ "code",  GenerateCodeblock} ,
	{ "video", GenerateVideoFrame },
};

static const char gArticleDelimBegin = '`';
static const char* gArticleDelimEnd = "``";

int GenerateArticle(std::ostream& out, const std::string& articleTemplate)
{
	if (   articleTemplate.find(gArticleDelimBegin) != -1   // has open 
		&& articleTemplate.find(gArticleDelimEnd)   == -1)  // but no close
	{
		return panic(INVALID_TEMPLATE, "Template has invalid tag. Open with ` and close with ``");
	}

	for (size_t i = 0; i < articleTemplate.length(); i++)
	{
		char c = articleTemplate.at(i);

		if (c != gArticleDelimBegin) // optimization to skip matching non tag characters
			continue;
	
		i += 1; // advance 1 to skip delimiter  -----> ^^^^

		std::string match = MatchString(gArticleBlocks, articleTemplate, i);

		if (match.length() > 0)
		{
			size_t beginIndex = i + match.length() + 1;

			size_t testForInvalidCloseIndex = articleTemplate.find(gArticleDelimBegin, beginIndex);
			size_t endIndex = articleTemplate.find(gArticleDelimEnd, beginIndex);

			if (testForInvalidCloseIndex != endIndex)
				return panic(INVALID_TEMPLATE, "Tag was opened but not closed. Close with ``");

			std::string args = articleTemplate.substr(beginIndex, endIndex - beginIndex);

			auto func = gArticleBlocks.find(match);
			if (func != gArticleBlocks.end())
			{
				func->second(out, args);
				out << std::endl << std::endl;
			}

			i = endIndex + 2;
		}
	}

	return 0;
}

void GenerateParagraph(std::ostream& out, const std::string& text)
{
	out << "<p>" << text << "</p>";
}

void GenerateTitle(std::ostream& out, const std::string& text)
{
	out << "<h2 class=\"article-title\">" << text << "</h2>";
}

void GenerateSubtitle(std::ostream& out, const std::string& text)
{
	out << "<h3 class=\"article-subtitle mark-section\">" << text << "</h3>";
}

void GenerateCodeblock(std::ostream& out, const std::string& text)
{
	auto [title, pathToCode] = split_string2(text, ',');

	std::string codeText = load_file(pathToCode);

	out << "<div class=\"article-code draw-left-line\">"
		<<		"<p class=\"article-code-file mark-section\">" << title << "</p>"
		<<		"<pre class=\"article-code-text\">";
					GenerateCodeStyle(out, codeText);
	out <<		"</pre>"
		<<	"</div>";
}

void GenerateVideoFrame(std::ostream& out, const std::string& text)
{
	auto [title, src] = split_string2(text, ',');

	out << "<div class=\"article-code draw-left-line\">"
		<<		"<p class=\"article-code-file mark-section\">" << title << "</p>"
		<<		"<iframe class=\"article-video\" src=\"" << src << "\" allowfullscreen></iframe>"
		<< "</div>";
}