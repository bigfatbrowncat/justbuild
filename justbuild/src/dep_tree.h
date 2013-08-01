/*
 * dep_tree.h
 *
 *  Created on: Jul 22, 2013
 *      Author: imizus
 */

#ifndef DEP_TREE_H_
#define DEP_TREE_H_

#include <string>
#include <list>

using namespace std;

class DependencyTreeItem;
class DependencyTreeItemComparator;

class DependencyTree
{
private:
	list<DependencyTreeItem> items;
public:
	DependencyTreeItem* addItem(const string& fileName);
	int getCount() { return items.size(); }
	DependencyTreeItem* findFile(const string& fileName);
	bool removeItem(const string& fileName);
	const list<DependencyTreeItem>& listFiles();
	void printTree();
};

class DependencyTreeItem
{
private:
	DependencyTree* owner;
	string fileName;
	list<DependencyTreeItem*> dependencies;
protected:

	friend class DependencyTree;
	bool removeDependency(DependencyTreeItem* item);
	void printTreeItem(int indent);

public:
	DependencyTreeItem(DependencyTree* owner, const string& fileName);

	/**
	 * true means "added", false means "already exists"
	 */
	bool addDependency(const string& fileName);

	const string& getFileName() const { return fileName; }
	DependencyTreeItem* findDependencyFile(const string& fileName);
	const list<DependencyTreeItem*>& listDependencyFiles() const;
	const list<string> listDependencyFileNames() const;
};

#endif /* DEP_TREE_H_ */
