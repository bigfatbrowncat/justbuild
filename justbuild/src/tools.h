/*
 * tools.h
 *
 *  Created on: Jul 15, 2013
 *      Author: imizus
 */

#ifndef TOOLS_H_
#define TOOLS_H_

#include <string>
#include <list>

using namespace std;

class IncorrectPathException : exception
{
};

enum ListDirectories
{
	ldOnlyDirs,
	ldOnlyFiles,
	ldEverything
};

enum MakeDirectoryResult
{
	mdrSuccess,
	mdrAlreadyExists,
	mdrError
};

bool isDirectory(const string& path);
bool directoryExists(const string& name);

time_t fileModifiedTime(const string& name);

MakeDirectoryResult makeDirectory(const string& name);
bool makeDirectoryRecursively(const string& name);

list<string> listFilesByExt(const string& path, const string& extension, ListDirectories listDirs = ldOnlyFiles);
list<string> listFilesByExtRecursively(const string& path, const string& extension, ListDirectories listDirs);


#endif /* TOOLS_H_ */
