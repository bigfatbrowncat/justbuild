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

	// Building Java files
/*	FILE* gcc_stream = popen("gcc", "r");
	char s[65536];
	fgets(s, 65535, gcc_stream);
	printf("[%s]\n", s);
	pclose(gcc_stream);*/

	list<string> ld = listDependencies("src/main.cpp");
	for (list<string>::iterator iter = ld.begin(); iter != ld.end(); iter++)
	{
		printf("%s\n", (*iter).c_str());
	}

//	makeDirectoryRecursively("../testproj/target/a/b/c");

/*	list<string> fl = listFilesByExtRecursively("../..", "exe", ldEverything);
	for (list<string>::iterator iter = fl.begin(); iter != fl.end(); iter++)
	{
		printf("%s\n", (*iter).c_str());
	}
*/
	return 0;
}
