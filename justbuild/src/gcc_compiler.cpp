/*
 * gcc_compiler.cpp
 *
 *  Created on: Aug 1, 2013
 *      Author: imizus
 */

#include "gcc_compiler.h"

int launchGCC(string arguments, bool showCommand)
{
	string command = (string)"g++ " + arguments;
	if (showCommand) printf("# %s\n", command.c_str());
	FILE* gcc_stream = popen(command.c_str(), "r");
	char s[128];
	string gccm = "";
	while (!feof(gcc_stream))
	{
		if (fgets(s, 128, gcc_stream) != NULL)
		{
			gccm += s;
		}
	}
	int result = pclose(gcc_stream);

	if (gccm.length() > 0) printf("%s\n", gccm.c_str());
	fflush(stdout);
	fflush(stderr);

	return result;
}

int compile(const string& sourceName, const list<string>& includes, const string& targetObjectName, bool showCommand)
{
	string includesLine = "";
	for (list<string>::const_iterator iter = includes.begin(); iter != includes.end(); iter++)
	{
		includesLine += "-I" + (*iter) + " ";
	}

	string arguments = (string)"-c " + sourceName + " " + includesLine + " -o " + targetObjectName;

	return launchGCC(arguments, showCommand);
}

int link(const list<string>& objects, const string& targetName, list<string> options, bool showCommand)
{
	string objectsLine = "";
	for (list<string>::const_iterator iter = objects.begin(); iter != objects.end(); iter++)
	{
		objectsLine += (*iter) + " ";
	}

	string optionsLine = "";
	for (list<string>::const_iterator iter = options.begin(); iter != options.end(); iter++)
	{
		optionsLine += (*iter) + " ";
	}

	string arguments = objectsLine + optionsLine + "-o " + targetName;

	return launchGCC(arguments, showCommand);
}
