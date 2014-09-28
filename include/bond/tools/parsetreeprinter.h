#ifndef BOND_TOOLS_PARSETREEPRINTER_H
#define BOND_TOOLS_PARSETREEPRINTER_H

#include "bond/conf.h"

namespace Bond
{

class ParseNode;
class ListParseNode;
class OutputStream;
class Token;

class ParseTreePrinter
{
public:
	void Print(const ParseNode *parseNode, OutputStream &stream);
	void PrintList(const ListParseNode *listNode, OutputStream &stream);
};

}

#endif
