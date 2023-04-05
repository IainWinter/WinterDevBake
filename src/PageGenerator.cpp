#include "PageGenerator.h"
#include "MatchString.h"
#include "ArticleGenerator.h"
#include "CommentGeneration.h"
#include "Util.h"
#include <filesystem>

int BakeArticle(std::ostream& out, const PageBakeArguments& args)
{
	if (!args.pathToArticleTemplate) // block no file
		return panic(INVALID_ARGS, "No article file provided for wbake");

	std::string articleTemplate = load_file(args.pathToArticleTemplate);
	return GenerateArticle(out, articleTemplate);
}

int BakeComments(std::ostream& out, const PageBakeArguments& args)
{
	if (!args.nameOfComments) // block no file
		return panic(INVALID_ARGS, "No comment name provided for wbake");

	return GenerateComments(out, args.nameOfComments);
}

static std::unordered_map<std::string, int(*)(std::ostream&, const PageBakeArguments&)> gPageBlocks =
{
	{ "article", BakeArticle },
	{ "comments", BakeComments }
};

int GeneratePage(std::ostream& out, const std::string& text, const PageBakeArguments& args)
{
	for (size_t i = 0; i < text.length(); i++)
	{
		std::string match = MatchString({ "<wbake " }, text, i);

		if (match.length() == 0)
		{
			out << text[i];
			continue;
		}

		i += match.length();

		// look ahead for /> to make sure this is a valid tag

		size_t endOfTag = i + 1;
		for (; endOfTag < text.length(); endOfTag++)
		{
			if (text.at(endOfTag - 1) == '<') // found another open, exit with error
				return panic(INVALID_TEMPLATE, "wbake tag was not closed");

			if (   text.at(endOfTag - 1) == '/' 
				&& text.at(endOfTag)     == '>') // found close
				break;
		}

		// find source

		size_t sourceIndex = text.find("source", i);
		size_t sourceArgIndexBegin = text.find("\"", sourceIndex) + 1;
		size_t sourceArgIndexEnd = text.find("\"", sourceArgIndexBegin);

		std::string source = text.substr(sourceArgIndexBegin, sourceArgIndexEnd - sourceArgIndexBegin);

		auto itr = gPageBlocks.find(source);
		if (itr == gPageBlocks.end())
			return panic(FUNCTION_NOT_FOUND, source.c_str());

		int err = itr->second(out, args);

		if (err)
			return err;

		i = endOfTag;
	}

	return 0;
}