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
#include "gcc_compiler.h"
#include "tools_test.h"
#include "dep_tree.h"

static const int ERROR = 1;

int main(int argc, char** argv)
{
	// Running tests
	test_listFilesByExt();
	test_listFilesByExtRecursively();

	string localPathAbsolute = realPath("./");
	printf("Current directory: %s\n", localPathAbsolute.c_str());

	// ** Building C++ files **

	string sourcePath = "src";
	string targetObjectPath = "target/obj";
	string targetBinaryPath = "target/bin";
	string targetBinaryName = "justbuild.exe";

	list<string> linkerOptions;
	linkerOptions.push_back("-static-libgcc");
	linkerOptions.push_back("-static-libstdc++");
	linkerOptions.push_back("-static");

	// Building the dependency tree

	DependencyTree depTree;
	list<string> sourceFiles = convertToRealPaths(listFilesByExtRecursively(sourcePath, "cpp", ldOnlyFiles));
	for (list<string>::iterator iter = sourceFiles.begin(); iter != sourceFiles.end(); iter++)
	{
		DependencyTreeItem* item = depTree.addItem(*iter);
		// Adding dependencies
		list<string> dependencyFiles = listDependencies(*iter);
		if (dependencyFiles.size() == 0)
		{
			// Just exit
			return ERROR;
		}

		for (list<string>::iterator iter = dependencyFiles.begin(); iter != dependencyFiles.end(); iter++)
		{
			// Checking if the dependency is local
			if (pathBeginsWith(*iter, localPathAbsolute))
			{
				item->addDependency(*iter);
			}
		}
	}
	//depTree.printTree();

	// Searching for the most independent source
	list<string> compiledObjects;

	if (!makeDirectoryTree(targetObjectPath))
	{
		fprintf(stderr, "Can't make the directory %s\n", targetObjectPath.c_str());
	}
	if (!makeDirectoryTree(targetBinaryPath))
	{
		fprintf(stderr, "Can't make the directory %s\n", targetBinaryPath.c_str());
	}

	for (list<DependencyTreeItem>::const_iterator iter = depTree.listFiles().begin(); iter != depTree.listFiles().end(); iter++)
	{
		string currentFile = (*iter).getFileName();
		// For each source file we analyze its dependencies
		if (pathEndsWith(currentFile, ".cpp"))
		{
			printf("Compiling %s...\n", replaceBeginning(currentFile, realPath(localPathAbsolute + "/"), "").c_str());
			string objectFile = replaceRelativePath(currentFile, sourcePath, targetObjectPath) + ".o";
			compiledObjects.push_back(objectFile);

			int errorCode = compile(currentFile, objectFile);
			if (errorCode != 0)
			{
				printf("Compilation errors. Compiler returned %d\n", errorCode);
			}
		}
	}

	string targetFile = realPath(targetBinaryPath + "/" + targetBinaryName);
	printf("Linking %s...\n", replaceBeginning(targetFile, realPath(localPathAbsolute + "/"), "").c_str());
	link(compiledObjects, targetFile, linkerOptions);

	return 0;
}
