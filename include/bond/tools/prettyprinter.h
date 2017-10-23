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
	enum Verbosity
	{
		VERBOSITY_NORMAL,
		VERBOSITY_MINIMAL
	};

	enum ConstantFolding
	{
		CONSTANT_FOLDING_OFF,
		CONSTANT_FOLDING_ON
	};

	void Print(const ParseNode *parseNode, OutputStream &stream, Verbosity verbosity = VERBOSITY_NORMAL,
		ConstantFolding folding = CONSTANT_FOLDING_OFF) const;

	void PrintList(const ListParseNode *listNode, OutputStream &stream, Verbosity verbosity = VERBOSITY_NORMAL,
		ConstantFolding folding = CONSTANT_FOLDING_OFF) const;
};

}

#endif
