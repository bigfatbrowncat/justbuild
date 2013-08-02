/*
 * gcc_compiler.h
 *
 *  Created on: Aug 1, 2013
 *      Author: imizus
 */

#ifndef GCC_COMPILER_H_
#define GCC_COMPILER_H_

#include <string>
#include <list>

using namespace std;

int compile(const string& sourceName, const list<string>& includes, const string& targetObjectName, bool showCommand);
int link(const list<string>& objects, const string& targetName, list<string> options, bool showCommand = false);

#endif /* GCC_COMPILER_H_ */
