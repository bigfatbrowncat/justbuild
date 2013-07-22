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
#include "dep_tree.h"

int main(int argc, char** argv)
{
	// Running tests
	test_listFilesByExt();
	test_listFilesByExtRecursively();

	printf("Current directory: %s\n", realPath(".").c_str());

	// Building C++ files
	string sourcePath = "src";

	DependencyTree depTree;

	list<string> sourceFiles = convertToRealPaths(listFilesByExtRecursively(sourcePath, "cpp", ldOnlyFiles));

	for (list<string>::iterator iter = sourceFiles.begin(); iter != sourceFiles.end(); iter++)
	{
		DependencyTreeItem* item = depTree.addItem(*iter);

		// Adding dependencies
		list<string> dependencyFiles = listDependencies(*iter);

		for (list<string>::iterator iter = dependencyFiles.begin(); iter != dependencyFiles.end(); iter++)
		{
			item->addDependency(*iter);
		}
	}

	depTree.printTree();

	return 0;
}
