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

bool isDir(const string& path);
list<string> listFilesByExt(const string& path, const string& extension, ListDirectories listDirs = ldOnlyFiles);


#endif /* TOOLS_H_ */
