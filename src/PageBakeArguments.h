#pragma once

struct PageBakeArguments
{
	bool printToConsole;
	const char* pathToTemplate;
	const char* pathToArticleTemplate;
	const char* nameOfComments;
	const char* pathToOutputFile;

	bool Valid() const;
};

void PrintHelp();