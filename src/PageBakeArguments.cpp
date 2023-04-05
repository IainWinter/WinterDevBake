#include "PageBakeArguments.h"
#include <filesystem>
#include <iostream>

bool FileExists(const char* path)
{
	if (path && !std::filesystem::exists(path))
	{
		std::cout << "File not found - " << path << std::endl;
		return false;
	}

	return true;
}

bool PageBakeArguments::Valid() const
{
	return pathToTemplate != nullptr
		&& (pathToOutputFile != nullptr || printToConsole)
		&& FileExists(pathToTemplate)
		//&& (!pathToOutputFile || FileExists(pathToOutputFile))
		&& (!pathToArticleTemplate || FileExists(pathToArticleTemplate));
}

void PrintHelp()
{
	std::cout 
		<< "usage: -s <source> [-a <article>] [-c <comment_name>] [-d <destination>] [-t]" << std::endl
		<< std::endl
		<< "Options:" << std::endl
		<<		"\t-s -> source file path of page template" << std::endl
		<<		"\t-a -> source file path of article template" << std::endl
		<<		"\t-c -> name to use for comment lookup" << std::endl
		<<		"\t-d -> destination file path" << std::endl
		<<		"\t-t -> print to console" << std::endl
	;
}