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

#include "tools_test.h"

int main(int argc, char** argv)
{
	// Running tests
	test_listFilesByExt();

	// Building Java files
	FILE* gcc_stream = popen("gcc", "r");
	char s[65536];
	fgets(s, 65535, gcc_stream);
	printf("[%s]\n", s);
	pclose(gcc_stream);

	MakeDirectoryResult res = makeDirectory("../testproj/target/");
	if (res == mdrSuccess) printf("success\n");
	if (res == mdrError) printf("error\n");
	if (res == mdrAlreadyExists) printf("exists\n");

	return 0;
}
