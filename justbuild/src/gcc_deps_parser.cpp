/*
 * c_deps_parser.cpp
 *
 *  Created on: Jul 17, 2013
 *      Author: imizus
 */

#include <list>
#include <vector>
#include <algorithm>

#include "gcc_deps_parser.h"

string replaceString(string subject, const string& search, const string& replace)
{
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != string::npos)
    {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
    }
    return subject;
}

list<string> split(const string& s, const string& delim, const bool keep_empty = true) {
	list<string> result;
    if (delim.empty()) {
        result.push_back(s);
        return result;
    }
    string::const_iterator substart = s.begin(), subend;
    while (true) {
        subend = search(substart, s.end(), delim.begin(), delim.end());
        string temp(substart, subend);
        if (keep_empty || !temp.empty()) {
            result.push_back(temp);
        }
        if (subend == s.end()) {
            break;
        }
        substart = subend + delim.size();
    }
    return result;
}

list<string> listDependencies(const string& sourceName)
{
	FILE* gcc_stream = popen(((string)"gcc -M " + sourceName).c_str(), "r");
	char s[128];
	string gccm = "";
	while (!feof(gcc_stream))
	{
		if (fgets(s, 128, gcc_stream) != NULL)
		{
			gccm += s;
		}
	}

	list<string> files;

	if (pclose(gcc_stream) == 0)
	{
		gccm = replaceString(gccm, (string)"\\\n", (string)"");
		gccm = replaceString(gccm, (string)"\n", (string)"");
		files = split(gccm, " ", false);

		files.erase(files.begin());		// Removing o:
		files.erase(files.begin());		// Removing the file itself
	}

	return files;
}
