/*
 * tools.cpp
 *
 *  Created on: Jul 15, 2013
 *      Author: imizus
 */

#include <string>
#include <list>
#include <vector>
#include <algorithm>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <limits.h>

#include "tools.h"

#ifdef __MINGW32__
	#define REALPATH(N,R)		_fullpath((R),(N),_MAX_PATH)
	#define PATH_LENGTH			_MAX_PATH
#else
	#define REALPATH(N,R)		realpath((N), (R))
	#define PATH_LENGTH			PATH_MAX
#endif

using namespace std;

bool isDirectory(const string& path)
{
	struct stat st;
	stat(path.c_str(), &st);
	if(S_ISDIR(st.st_mode))
	   return true;					// It's a dir
	else
	   return false;				// It's a file
}

bool directoryExists(const string& name)
{
    if (access(name.c_str(), 0) == 0)
    {
        return isDirectory(name);
    }
    else
    {
    	return false;
    }
}

time_t fileModifiedTime(const string& name)
{
	struct stat st;
	stat(name.c_str(), &st);
	return st.st_mtime;
}

string realPath(const string& relativePath)
{
	char path[PATH_LENGTH];
	char* res = REALPATH(relativePath.c_str(), path);
	if (res == NULL)
	{
		throw IncorrectPathException();
	}
	else
	{
		return string(res);
	}
}

list<string> convertToRealPaths(const list<string>& pathList)
{
	list<string> res;
	for (list<string>::const_iterator iter = pathList.begin(); iter != pathList.end(); iter++)
	{
		res.push_back(realPath(*iter));
	}
	return res;
}

MakeDirectoryResult makeDirectory(const string& name)
{
	if (directoryExists(name.c_str()))
	{
		return mdrAlreadyExists;
	}
	else
	{
		int res;
#ifdef __MINGW32__
		res = mkdir(name.c_str());
#else
		res = mkdir(name.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
#endif
		if (res == 0)
			return mdrSuccess;
		else
			return mdrError;
	}
}

bool makeDirectoryTree(const string& name)
{
	// Making list of directory names
	vector<string> dirs;
	string curDir = "";
	for (size_t i = 0; i < name.size(); i++)
	{
		if (name[i] == '/')
		{
			dirs.push_back(curDir);
			curDir = "";
		}
		else
		{
			curDir += name[i];
		}
	}
	dirs.push_back(curDir);

	// Creating directories
	size_t k = 0;
	string tmpPath = "";
	while (k < dirs.size())
	{
		tmpPath += ((tmpPath != "") ? "/" : "") + dirs[k];
		MakeDirectoryResult mdr = makeDirectory(tmpPath);
		if (mdr == mdrError) return false;
		k++;
	}
	return true;
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
				if (isDirectory(path + "/" + dirEntry->d_name)) res.push_back(dirEntry->d_name);
				break;
			case ldOnlyFiles:
				if (!isDirectory(path + "/" + dirEntry->d_name)) res.push_back(dirEntry->d_name);
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
	class name_is
	{
		string name;
	public:
		name_is(const string& name) : name(name) {}
		bool operator() (const TreeItem& item) { return item.name == name; }
	};


	TreeItem* parent;
	string name;
	list<TreeItem> children;
public:
	TreeItem(TreeItem* parent, const string& name) : parent(parent), name(name) {}
	TreeItem* getParent() const { return parent; }
	const string& getName() const { return name; }
	TreeItem* nextSibling() const
	{
		if (parent == NULL) return NULL;

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

	TreeItem* addChild(const string& name)
	{
		children.push_back(TreeItem(this, name));
		return &(children.back());
	}

	void removeChild(const string& name)
	{
		children.remove_if(name_is(name));
	}

	string path()
	{
		TreeItem* cur = this;
		string path = this->name;
		while (cur != NULL)
		{
			cur = cur->getParent();
			if (cur != NULL && cur->name != "")
			{
				path = cur->getName() + "/" + path;
			}
		}
		return path;
	}
	TreeItem* findChildByName(const string& name)
	{
		for (list<TreeItem>::iterator iter = children.begin(); iter != children.end(); iter++)
		{
			if ((*iter).name == name) return &(*iter);
		}
		return NULL;
	}
};

void listFilesToTreeItem(const string& path, TreeItem& treeItem, const string& extension, ListDirectories listDirs)
{
//	printf("entering listFilesToTreeItem with path %s and treeitem %s\n", path.c_str(), treeItem.getName().c_str()); fflush(stdout);
	string curPath = path + "/" + treeItem.path();
	list<string> directories = listFilesByExt(curPath, "", ldOnlyDirs);
	for (list<string>::iterator iter = directories.begin(); iter != directories.end(); iter++)
	{
		TreeItem* newDir = treeItem.addChild(*iter);
		listFilesToTreeItem(path, *newDir, extension, listDirs);
	}

	list<string> files = listFilesByExt(path + "/" + treeItem.path(), extension, listDirs);
	for (list<string>::iterator iter = files.begin(); iter != files.end(); iter++)
	{
		if (treeItem.findChildByName(*iter) == NULL)
		{
			treeItem.addChild(*iter);
		}
	}

	// Removing empty directories which are not listed in files
	for (list<string>::iterator iter = directories.begin(); iter != directories.end(); iter++)
	{
		TreeItem* theChild = treeItem.findChildByName(*iter);
		if (theChild->firstChild() == NULL &&									// It's empty
			std::find(files.begin(), files.end(), *iter) == files.end()			// It's not presented in files array
		)
		{
			treeItem.removeChild(*iter);
		}
	}
}

list<string> listFilesByExtRecursively(const string& path, const string& extension, ListDirectories listDirs)
{
	// Building the tree of files
	TreeItem root(NULL, "");
	listFilesToTreeItem(path, root, extension, listDirs);

	// Now we walk thru the tree and linearize it
	list<string> pathList;
	TreeItem* cur = &root;
	bool out2;
	do
	{
		bool out1;
		do
		{
			while (cur->firstChild() != NULL)
			{
				cur = cur->firstChild();
			}

			pathList.push_back(path + "/" + cur->path());

			if (cur->nextSibling() != NULL)
			{
				cur = cur->nextSibling();
				out1 = false;
			}
			else
			{
				out1 = true;
			}
		}
		while (!out1);

		if (cur->getParent()->nextSibling() != NULL)
		{
			cur = cur->getParent()->nextSibling();
			out2 = false;
		}
		else
		{
			out2 = true;
		}
	}
	while (!out2);

	return pathList;
}

bool pathBeginsWith(string path, string beginning)
{
	return (path.length() >= beginning.length()) && path.substr(0, beginning.length()) == beginning;
}

bool pathEndsWith(string path, string ending)
{
	return (path.length() >= ending.length()) && (path.substr(path.length() - ending.length()) == ending);
}

string replaceRelativePath(string path, string oldBeginning, string newBeginning)
{
	string rpath = realPath(path);
	string rold = realPath(oldBeginning + "/");
	string rnew = realPath(newBeginning + "/");

	return replaceBeginning(rpath, rold, rnew);

}

string replaceBeginning(string path, string oldBeginning, string newBeginning)
{
	if (pathBeginsWith(path, oldBeginning))
	{
		return path.replace(0, oldBeginning.length(), newBeginning);
	}
	else
		return path;
}
