/**
 * JustBuild is a simple build tool for projects made with Java, C and C++
 * It is intended to be simple, stupid, doesn't need any hardcore configuration.
 *
 * It Just Builds.
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

#include "rapidxml-1.13/rapidxml.hpp"

#include "tools.h"
#include "gcc_deps_parser.h"
#include "gcc_compiler.h"
#include "tools_test.h"
#include "dep_tree.h"

static const int CANT_LIST_DEPENDENCIES_ERROR = 1;
static const int CANT_COMPILE_ERROR = 2;
static const int CANT_LINK_ERROR = 3;

static const int CONFIG_ERROR = 4;

using namespace std;
using namespace rapidxml;

class XMLFile
{
private:
	char* fileContents;
	xml_document<> document;
public:
	XMLFile(const string& fileName)
	{
		FILE* xmlFile = fopen(fileName.c_str(), "r");
		char s[65536];
		string xmlString = "";
		while (!feof(xmlFile))
		{
			if (fgets(s, 128, xmlFile) != NULL)
			{
				xmlString += s;
			}
		}
		fileContents = new char[xmlString.length()];
		strcpy(fileContents, xmlString.c_str());

		document.parse<0>(fileContents);    // 0 means default parse flags

	}

	xml_document<>* getDocument() { return &document; }

	~XMLFile()
	{
		delete [] fileContents;
	}
};


time_t latestModificationTime(const list<string>& files)
{
	time_t latest = 0;
	for (list<string>::const_iterator iter = files.begin(); iter != files.end(); iter++)
	{
		time_t mod = fileModifiedTime(*iter);
		if (mod > latest) latest = mod;
	}
	return latest;
}

int main(int argc, char** argv)
{
	// Running tests
	test_listFilesByExt();
	test_listFilesByExtRecursively();

	bool showDebugInfo = false;

	// Reading just.xml
	XMLFile justXMLFile("just.xml");

	xml_document<>* justXMLDoc = justXMLFile.getDocument();
	xml_node<> *justUnitNode = justXMLDoc->first_node();
	if ((string)(justUnitNode->name()) != string("just-unit"))
	{
		printf("The root node of the configuration file should be a <just-unit> node");
		return CONFIG_ERROR;
	}
	else if (justUnitNode->next_sibling() != NULL)
	{
		printf("There should be only one root node in configuration file");
		return CONFIG_ERROR;
	}

	string target, sourcePath, objectsPath, binary, targetBinaryName;
	list<string> includes, linkerOptions;

	for (xml_node<>* buildNode = justUnitNode->first_node(); buildNode != NULL; buildNode = buildNode->next_sibling())
	{
		if ((string)(buildNode->name()) == string("build"))
		{
			for (xml_node<>* cppNode = buildNode->first_node(); cppNode != NULL; cppNode = cppNode->next_sibling())
			{
				if ((string)(cppNode->name()) == string("cpp"))
				{

					// Reading attributes
					for (xml_attribute<>* attr = cppNode->first_attribute(); attr != NULL; attr = attr->next_attribute())
					{
						if ((string)(attr->name()) == string("target"))
						{
							target = attr->value();
						}
						else
						{
							printf("Incorrect attribute in <cpp> node. Only target=\"...\" allowed");
							return CONFIG_ERROR;
						}
					}

					for (xml_node<>* compilerLinkerNode = cppNode->first_node(); compilerLinkerNode != NULL; compilerLinkerNode = compilerLinkerNode->next_sibling())
					{
						if ((string)(compilerLinkerNode->name()) == string("compiler"))
						{
							// Reading attributes
							for (xml_attribute<>* attr = compilerLinkerNode->first_attribute(); attr != NULL; attr = attr->next_attribute())
							{
								if ((string)(attr->name()) == string("source"))
								{
									sourcePath = attr->value();
								}
								else if ((string)(attr->name()) == string("objects"))
								{
									objectsPath = attr->value();
								}
								else
								{
									printf("Incorrect attribute in <compiler> node. Only source=\"...\", objects=\"...\",  allowed");
									return CONFIG_ERROR;
								}
							}

							for (xml_node<>* includesNode = compilerLinkerNode->first_node(); includesNode != NULL; includesNode = includesNode->next_sibling())
							{
								if ((string)(includesNode->name()) == string("includes"))
								{
									for (xml_node<>* includeNode = includesNode->first_node(); includeNode != NULL; includeNode = includeNode->next_sibling())
									{
										if ((string)(includeNode->name()) == string("include"))
										{
											includes.push_back(includeNode->value());
										}
										else
										{
											printf("Incorrect node inside <includes> node. Only <include> nodes allowed");
											return CONFIG_ERROR;
										}
									}
								}
							}
						}
						else if ((string)(compilerLinkerNode->name()) == string("linker"))
						{
							// Reading attributes
							for (xml_attribute<>* attr = compilerLinkerNode->first_attribute(); attr != NULL; attr = attr->next_attribute())
							{
								if ((string)(attr->name()) == string("binary"))
								{
									binary = attr->value();
								}
								else if ((string)(attr->name()) == string("objects"))
								{
									objectsPath = attr->value();
								}
								else if ((string)(attr->name()) == string("target-binary-name"))
								{
									targetBinaryName = attr->value();
								}
								else
								{
									printf("Incorrect attribute in <compiler> node. Only source=\"...\", objects=\"...\",  allowed");
									return CONFIG_ERROR;
								}
							}

							for (xml_node<>* optionsNode = compilerLinkerNode->first_node(); optionsNode != NULL; optionsNode = optionsNode->next_sibling())
							{
								if ((string)(optionsNode->name()) == string("options"))
								{
									for (xml_node<>* optionNode = optionsNode->first_node(); optionNode != NULL; optionNode = optionNode->next_sibling())
									{
										if ((string)(optionNode->name()) == string("option"))
										{
											linkerOptions.push_back(optionNode->value());
										}
										else
										{
											printf("Incorrect node inside <options> node. Only <option> nodes allowed");
											return CONFIG_ERROR;
										}
									}
								}
							}
						}
						else
						{
							printf("Incorrect node inside <cpp> node. Only <compiler> and <linker> nodes allowed");
							return CONFIG_ERROR;
						}
					}
				}
				else
				{
					printf("Incorrect node inside <build> node. Only <cpp> nodes allowed");
					return CONFIG_ERROR;
				}
			}
		}
		else
		{
			printf("Incorrect node inside <just-unit> node. Only <build> nodes allowed");
			return CONFIG_ERROR;
		}
	}


	string localPathAbsolute = realPath("./");
	if (showDebugInfo) printf("Current directory: %s\n", localPathAbsolute.c_str());

	// ** Building C++ files **

	string targetObjectPath = target + "/" + objectsPath;// "target/obj";
	string targetBinaryPath = target + "/" + binary;// "target/bin";
	//string targetBinaryName = "justbuild";

	//list<string> linkerOptions = ;
	//linkerOptions.push_back("-static-libgcc");
	//linkerOptions.push_back("-static-libstdc++");
	//linkerOptions.push_back("-static");

	// Building the dependency tree

	DependencyTree depTree;
	list<string> sourceFiles = convertToRealPaths(listFilesByExtRecursively(sourcePath, "cpp", ldOnlyFiles));
	for (list<string>::iterator iter = sourceFiles.begin(); iter != sourceFiles.end(); iter++)
	{
		DependencyTreeItem* item = depTree.addItem(*iter);
		// Adding dependencies
		list<string> dependencyFiles = listDependencies(*iter);
		if (dependencyFiles.size() == 0)
		{
			// Just exit
			return CANT_LIST_DEPENDENCIES_ERROR;
		}

		for (list<string>::iterator iter = dependencyFiles.begin(); iter != dependencyFiles.end(); iter++)
		{
			// Checking if the dependency is local
			if (pathBeginsWith(*iter, localPathAbsolute))
			{
				item->addDependency(*iter);
			}
		}
	}
	//depTree.printTree();

	// Compiling

	list<string> objectFiles;

	if (!makeDirectoryTree(targetObjectPath))
	{
		fprintf(stderr, "Can't make the directory %s\n", targetObjectPath.c_str());
	}
	if (!makeDirectoryTree(targetBinaryPath))
	{
		fprintf(stderr, "Can't make the directory %s\n", targetBinaryPath.c_str());
	}

	for (list<DependencyTreeItem>::const_iterator iter = depTree.listFiles().begin(); iter != depTree.listFiles().end(); iter++)
	{
		string currentFile = (*iter).getFileName();
		// For each source file we analyze its dependencies
		if (pathEndsWith(currentFile, ".cpp"))
		{
			string objectFile = replaceRelativePath(currentFile, sourcePath, targetObjectPath) + ".o";
			objectFiles.push_back(objectFile);

			if (!fileExists(objectFile) ||
				fileModifiedTime(objectFile) < latestModificationTime((*iter).listDependencyFileNames()) ||
				fileModifiedTime(objectFile) < fileModifiedTime(currentFile))
			{
				printf("Compiling %s...\n", replaceBeginning(currentFile, realPath(localPathAbsolute + "/"), "").c_str());

				int errorCode = compile(currentFile, objectFile);
				if (errorCode != 0)
				{
					printf("Compilation errors. Compiler returned %d\n", errorCode);
					return CANT_COMPILE_ERROR;
				}
			}
			else
			{
				if (showDebugInfo)
				{
					printf("Leaving %s unchanged...\n", replaceBeginning(currentFile, realPath(localPathAbsolute + "/"), "").c_str());
				}
			}
		}
	}

	string targetFile = realPath(targetBinaryPath + "/" + targetBinaryName);

#ifdef __MINGW32__
	// For Windows we add .exe to the binary
	targetFile += ".exe";
#endif

	if (!fileExists(targetFile) ||
	    fileModifiedTime(targetFile) < latestModificationTime(objectFiles))
	{
		printf("Linking %s...\n", replaceBeginning(targetFile, realPath(localPathAbsolute + "/"), "").c_str());

		int errorCode = link(objectFiles, targetFile, linkerOptions);
		if (errorCode != 0)
		{
			printf("Linking error. Linker returned %d\n", errorCode);
			return CANT_LINK_ERROR;
		}
	}
	else
	{
		if (showDebugInfo)
		{
			printf("Leaving %s unchanged...\n", replaceBeginning(targetFile, realPath(localPathAbsolute + "/"), "").c_str());
		}
	}

	printf("Everything has been done\n");
	return 0;
}
