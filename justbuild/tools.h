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

bool isDir(const string& path);
bool directoryExists( const char* absolutePath );

time_t fileModifiedTime(const string& path);
MakeDirectoryResult makeDirectory(const char* name);
list<string> listFilesByExt(const string& path, const string& extension, ListDirectories listDirs = ldOnlyFiles);


#endif /* TOOLS_H_ */
