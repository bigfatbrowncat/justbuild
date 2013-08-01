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
string realPath(const string& relativePath);
list<string> convertToRealPaths(const list<string>& pathList);

MakeDirectoryResult makeDirectory(const string& name);
bool makeDirectoryTree(const string& name);

list<string> listFilesByExt(const string& path, const string& extension, ListDirectories listDirs = ldOnlyFiles);
list<string> listFilesByExtRecursively(const string& path, const string& extension, ListDirectories listDirs);

bool pathBeginsWith(string path, string beginning);
bool pathEndsWith(string path, string ending);
string replaceRelativePath(string path, string oldBeginning, string newBeginning);
string replaceBeginning(string path, string oldBeginning, string newBeginning);


#endif /* TOOLS_H_ */
