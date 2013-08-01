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

static const int CANT_LIST_DEPENDENCIES_ERROR = 1;
static const int CANT_COMPILE_ERROR = 2;
static const int CANT_LINK_ERROR = 3;

time_t latestModificationTime(const list<string>& files)
{
	time_t latest = 0;
	for (list<string>::const_iterator iter = files.begin(); iter != files.end(); iter++)
	{
		time_t mod = fileModifiedTime(*iter);
		if (mod > latest) latest = mod;
	}
	return latest;
}

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
	string targetBinaryName = "justbuild";

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
			return CANT_LIST_DEPENDENCIES_ERROR;
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

	// Compiling

	list<string> objects;

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
			string objectFile = replaceRelativePath(currentFile, sourcePath, targetObjectPath) + ".o";
			objects.push_back(objectFile);

			if (!fileExists(objectFile) ||
				fileModifiedTime(objectFile) < latestModificationTime((*iter).listDependencyFileNames()) ||
				fileModifiedTime(objectFile) < fileModifiedTime(currentFile))
			{
				printf("Compiling %s...\n", replaceBeginning(currentFile, realPath(localPathAbsolute + "/"), "").c_str());

				int errorCode = compile(currentFile, objectFile);
				if (errorCode != 0)
				{
					printf("Compilation errors. Compiler returned %d\n", errorCode);
					return CANT_COMPILE_ERROR;
				}
			}
			else
			{
				printf("Passing %s through...\n", replaceBeginning(currentFile, realPath(localPathAbsolute + "/"), "").c_str());
			}
		}
	}

	string targetFile = realPath(targetBinaryPath + "/" + targetBinaryName);

#ifdef __MINGW32__
	// For Windows we add .exe to the binary
	targetFile += ".exe";
#endif

	printf("Linking %s...\n", replaceBeginning(targetFile, realPath(localPathAbsolute + "/"), "").c_str());

	int errorCode = link(objects, targetFile, linkerOptions);
	if (errorCode != 0)
	{
		printf("Linking error. Linker returned %d\n", errorCode);
		return CANT_LINK_ERROR;
	}

	return 0;
}
