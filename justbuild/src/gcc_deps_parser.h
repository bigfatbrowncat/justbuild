/*
 * c_deps_parser.h
 *
 *  Created on: Jul 17, 2013
 *      Author: imizus
 */

#ifndef C_DEPS_PARSER_H_
#define C_DEPS_PARSER_H_

#include <string>
#include <list>
using namespace std;

list<string> listDependencies(const string& sourceName, const list<string>& includes);

#endif /* C_DEPS_PARSER_H_ */
