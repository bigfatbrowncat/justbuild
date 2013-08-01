/*
 * dep_tree.cpp
 *
 *  Created on: Jul 22, 2013
 *      Author: imizus
 */

#include <algorithm>

#include "dep_tree.h"

DependencyTreeItem* DependencyTree::addItem(const string& fileName)
{
	DependencyTreeItem* found = findFile(fileName);
	if (found != NULL)
	{
		return found;
	}
	else
	{
		items.push_back(DependencyTreeItem(this, fileName));
		return &(items.back());
	}
}

DependencyTreeItem* DependencyTree::findFile(const string& fileName)
{
	for (list<DependencyTreeItem>::iterator iter = items.begin(); iter != items.end(); iter++)
	{
		if ((*iter).getFileName() == fileName) return &(*iter);
	}
	return NULL;
}

bool DependencyTree::removeItem(const string& fileName)
{
	DependencyTreeItem* item = findFile(fileName);

	if (this->findFile(fileName) == NULL)
	{
		return false;
	}
	else
	{
		bool res = false;
		for (list<DependencyTreeItem>::iterator iter = items.begin(); iter != items.end(); iter++)
		{
			if ((*iter).getFileName() == (*item).getFileName())
			{
				iter = items.erase(iter);
				if (iter == items.end()) break;
			}

			if ((*iter).removeDependency(item))
			{
				// Something's removed
				res = true;
			}
		}

		return res;
	}
}

const list<DependencyTreeItem>& DependencyTree::listFiles()
{
	return items;
}

void DependencyTree::printTree()
{
	for (list<DependencyTreeItem>::iterator iter = items.begin(); iter != items.end(); iter++)
	{
		(*iter).printTreeItem(0);
	}
}

bool DependencyTreeItem::removeDependency(DependencyTreeItem* item)
{
	list<DependencyTreeItem*>::iterator iter = std::find(dependencies.begin(), dependencies.end(), item);
	if (iter == dependencies.end()) return false;
	dependencies.erase(iter);
	return true;
}

DependencyTreeItem::DependencyTreeItem(DependencyTree* owner, const string& fileName) : owner(owner), fileName(fileName)
{

}

DependencyTreeItem* DependencyTreeItem::findDependencyFile(const string& fileName)
{
	for (list<DependencyTreeItem*>::iterator iter = dependencies.begin(); iter != dependencies.end(); iter++)
	{
		if ((*iter)->getFileName() == fileName) return *iter;
	}
	return NULL;
}

bool DependencyTreeItem::addDependency(const string& fileName)
{
	if (findDependencyFile(fileName) != NULL)
	{
		// This dependency already exists
		return false;
	}
	else
	{
		DependencyTreeItem* dti = owner->findFile(fileName);
		if (dti == NULL)
		{
			// This dependency file isn't found among our files
			dti = owner->addItem(fileName);
		}

		// Adding dependency
		dependencies.push_back(dti);

		return true;
	}
}

const list<DependencyTreeItem*>& DependencyTreeItem::listDependencyFiles() const
{
	return dependencies;
}

void DependencyTreeItem::printTreeItem(int indent)
{
	for (int i = 0; i < indent; i++) printf("\t");
	printf("%s\n", fileName.c_str());

	for (list<DependencyTreeItem*>::iterator iter = dependencies.begin(); iter != dependencies.end(); iter++)
	{
		(*iter)->printTreeItem(indent + 1);
	}
}
