#ifndef BOND_TOOLS_PRETTYPRINTER_H
#define BOND_TOOLS_PRETTYPRINTER_H

#include "bond/conf.h"

namespace Bond
{

class ParseNode;
class ListParseNode;
class OutputStream;
class Token;

class PrettyPrinter
{
public:
	void Print(const ParseNode *parseNode, OutputStream &stream, bool printFoldedConstants = false);
	void PrintList(const ListParseNode *listNode, OutputStream &stream, bool printFoldedConstants = false);
};

}

#endif
