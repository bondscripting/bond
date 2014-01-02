#ifndef BOND_TOOLS_PARSETREEPRINTER_H
#define BOND_TOOLS_PARSETREEPRINTER_H

#include "bond/conf.h"

namespace Bond
{

class ParseNode;
class ListParseNode;
class TextWriter;
class Token;

class ParseTreePrinter
{
public:
	void Print(const ParseNode *parseNode, TextWriter &writer);
	void PrintList(const ListParseNode *listNode, TextWriter &writer);
};

}

#endif
