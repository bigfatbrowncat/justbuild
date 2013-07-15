/*
 * tools_test.cpp
 *
 *  Created on: Jul 15, 2013
 *      Author: imizus
 */

#include <stdio.h>

#include "tools_test.h"

#include "tools.h"

bool test_listFilesByExt_1()
{
	list<string> res = listFilesByExt("tests/listFilesByExt", ".");
	if (res.size() != 2) return false;
	for (list<string>::iterator iter = res.begin(); iter != res.end(); iter++)
	{
		if ((*iter) != "file" && (*iter) != "ext") return false;
	}
	return true;
}

bool test_listFilesByExt_2a()
{
	list<string> res = listFilesByExt("tests/listFilesByExt", "ext", ldEverything);
	if (res.size() != 2) return false;
	for (list<string>::iterator iter = res.begin(); iter != res.end(); iter++)
	{
		if ((*iter) != "dir.ext" && (*iter) != "file1.ext") return false;
	}
	return true;
}

bool test_listFilesByExt_2b()
{
	list<string> res = listFilesByExt("tests/listFilesByExt", ".ext", ldEverything);
	if (res.size() != 2) return false;
	for (list<string>::iterator iter = res.begin(); iter != res.end(); iter++)
	{
		if ((*iter) != "dir.ext" && (*iter) != "file1.ext") return false;
	}
	return true;
}

bool test_listFilesByExt_3()
{
	list<string> res = listFilesByExt("tests/listFilesByExt", "", ldOnlyDirs);
	if (res.size() != 1) return false;
	for (list<string>::iterator iter = res.begin(); iter != res.end(); iter++)
	{
		if ((*iter) != "dir.ext") return false;
	}
	return true;
}

bool test_listFilesByExt()
{
	if (!test_listFilesByExt_1()) { printf("test_listFilesByExt_1 failed\n"); return false; }
	if (!test_listFilesByExt_2a()) { printf("test_listFilesByExt_2a failed\n"); return false; }
	if (!test_listFilesByExt_2b()) { printf("test_listFilesByExt_2b failed\n"); return false; }
	if (!test_listFilesByExt_3()) { printf("test_listFilesByExt_3 failed\n"); return false; }
	return true;
}
