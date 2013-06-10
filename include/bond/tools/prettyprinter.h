#ifndef BOND_TOOLS_PRETTYPRINTER_H
#define BOND_TOOLS_PRETTYPRINTER_H

#include "bond/conf.h"

namespace Bond
{

class ParseNode;
class ListParseNode;
class TextWriter;
class Token;

class PrettyPrinter
{
public:
	void Print(const ParseNode *parseNode, TextWriter &writer, bool printFoldedConstants = false);
	void PrintList(const ListParseNode *listNode, TextWriter &writer, bool printFoldedConstants = false);
};

}

#endif
