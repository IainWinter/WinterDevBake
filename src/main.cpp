#include "PageGenerator.h"
#include "ArticleGenerator.h"
#include "Util.h"

#include <iostream>
#include <fstream>
#include <filesystem>

int main(int argc, const char** args)
{
	PageBakeArguments bake = {};

	for (int i = 1; i < argc; i++)
	{
		if (args[i][0] != '-' || strlen(args[i]) < 2)
			continue;

		switch (args[i][1])
		{
			case 't':
				bake.printToConsole = true;
				break;
			case 's':
				bake.pathToTemplate = args[i + 1];
				break;
			case 'a':
				bake.pathToArticleTemplate = args[i + 1];
				break;
			case 'c':
				bake.nameOfComments= args[i + 1];
				break;
			case 'd':
				bake.pathToOutputFile = args[i + 1];
				break;
		}
	}

	if ( !bake.Valid() )
	{
		PrintHelp();
		return 1;
	}
	
	std::string file = load_file(bake.pathToTemplate);
	
	int error = 0;

	if (bake.printToConsole)
	{
		error = GeneratePage(std::cout, file, bake);
	}

	else
	{
		std::ofstream out = std::ofstream(bake.pathToOutputFile);
		error = GeneratePage(out, file, bake);
	}

	printf("\n\n%s\n", error == 0 ? "Done" : "Failed");

	return error;
}