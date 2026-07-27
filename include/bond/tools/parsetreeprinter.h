#ifndef BOND_TOOLS_PARSETREEPRINTER_H
#define BOND_TOOLS_PARSETREEPRINTER_H

#include "bond/conf.h"

namespace Bond
{

class ParseNode;
class ListParseNode;
class OutputStream;
class Token;

/// \brief A printer for Bond parse trees.
///
/// ParseTreePrinter writes a structural representation of parsed Bond source code to an
/// OutputStream.
///
/// \sa ParseNode, ListParseNode, OutputStream
/// \ingroup tools
class ParseTreePrinter
{
public:
	/// \brief Prints a parse tree rooted at a single parse node.
	/// \param parseNode Root parse node to print.
	/// \param stream Output stream receiving the parse tree text.
	void Print(const ParseNode *parseNode, OutputStream &stream);

	/// \brief Prints a parse tree represented as a list of parse nodes.
	/// \param listNode Head of the parse node list to print.
	/// \param stream Output stream receiving the parse tree text.
	void PrintList(const ListParseNode *listNode, OutputStream &stream);
};

}

#endif
