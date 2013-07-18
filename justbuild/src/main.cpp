/**
 * JustBuild is a simple build tool for projects made with Java, C and C++
 * It is intended to be simple, stupid, doesn't need any hardcore configuration.
 *
 * It Just Builds.
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

#include "tools.h"
#include "gcc_deps_parser.h"
#include "tools_test.h"

int main(int argc, char** argv)
{
	// Running tests
	test_listFilesByExt();
	test_listFilesByExtRecursively();

	printf("Current directory: %s\n", realPath(".").c_str());

	// Building C++ files
	string sourcePath = "src";
	list<string> sourceFiles = convertToRealPaths(listFilesByExtRecursively(sourcePath, "cpp", ldOnlyFiles));


	for (list<string>::iterator iter = sourceFiles.begin(); iter != sourceFiles.end(); iter++)
	{
		printf("%s\n", (*iter).c_str());
	}

	return 0;
}
