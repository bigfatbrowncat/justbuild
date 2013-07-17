/*
 * tools.cpp
 *
 *  Created on: Jul 15, 2013
 *      Author: imizus
 */

#include <string>
#include <list>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include "tools.h"

using namespace std;

bool isDir(const string& path)
{
	struct stat st;
	stat(path.c_str(), &st);
	if(S_ISDIR(st.st_mode))
	   return true;					// It's a dir
	else
	   return false;				// It's a file
}

time_t fileModifiedTime(const string& path)
{
	struct stat st;
	stat(path.c_str(), &st);
	return st.st_mtime;
}

list<string> listFilesByExt(const string& path, const string& extension, ListDirectories listDirs)
{
	string ext = extension;
	list<string> res;
	DIR* dir = opendir(path.c_str());

	if (dir != NULL)
	{
		// 1. Listing all files

		dirent* dirEntry;
		while ((dirEntry = readdir(dir)) != NULL)
		{
			if ((string)(dirEntry->d_name) == "." || (string)(dirEntry->d_name) == "..") continue;
			switch (listDirs)
			{
			case ldOnlyDirs:
				if (isDir(path + "/" + dirEntry->d_name)) res.push_back(dirEntry->d_name);
				break;
			case ldOnlyFiles:
				if (!isDir(path + "/" + dirEntry->d_name)) res.push_back(dirEntry->d_name);
				break;
			case ldEverything:
				res.push_back(dirEntry->d_name);
				break;
			}
		}

		closedir(dir);

		// 2. Checking if extension present
		if (extension == "")
		{
			return res;
		}
		else if (extension == ".")
		{
			// Listing files without extension
			list<string> res2;
			for (list<string>::iterator iter = res.begin(); iter != res.end(); iter++)
			{
				size_t len = (*iter).length();
				if (((*iter).find(".") == string::npos) || ((*iter).find(".") == len - 1))
				{
					res2.push_back(*iter);
				}
			}
			return res2;
		}
		else
		{
			if (extension[0] != '.')
			{
				ext = "." + extension;
			}

			// Listing files with correct extensions
			list<string> res2;
			for (list<string>::iterator iter = res.begin(); iter != res.end(); iter++)
			{
				size_t len = (*iter).length();
				if ((len >= ext.length()) && ((*iter).substr(len - ext.length()) == ext))
				{
					res2.push_back(*iter);
				}
			}
			return res2;
		}
	}
	else
	{
		throw IncorrectPathException();
	}

}

class TreeItem
{
	TreeItem* parent;
	string name;
	list<TreeItem> children;
public:
	TreeItem(TreeItem& parent, const string& name) : parent(&parent), name(name) {}
	TreeItem* getParent() const { return parent; }
	const string& getName() const { return name; }
	TreeItem* nextSibling() const
	{
		list<TreeItem>::iterator iter = parent->children.begin();
		while (&(*iter) != this)
		{
			iter++;
		}
		iter++;
		if (iter == parent->children.end())
		{
			return NULL;
		}
		else
		{
			return &(*iter);
		}
	}
	TreeItem* firstChild()
	{
		if (children.size() > 0)
		{
			return &(*(children.begin()));
		}
		else
		{
			return NULL;
		}
	}
	void addChild(const string& name)
	{
		children.push_back(TreeItem(*this, name));
	}
};

list<string> listFilesByExtRecursively(const string& path, const string& extension, ListDirectories listDirs)
{

}
