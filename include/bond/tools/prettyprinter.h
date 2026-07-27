#ifndef BOND_TOOLS_PRETTYPRINTER_H
#define BOND_TOOLS_PRETTYPRINTER_H

#include "bond/conf.h"

namespace Bond
{

class ParseNode;
class ListParseNode;
class OutputStream;
class Token;

/// \brief A formatter for Bond parse trees.
///
/// PrettyPrinter converts parse trees back into Bond source code using consistent formatting rules.
/// It can optionally emit minimal formatting and fold constant expressions.
///
/// \sa ParseNode, ListParseNode, OutputStream
/// \ingroup tools
class PrettyPrinter
{
public:
	/// \brief Controls whitespace and layout verbosity in pretty-printed output.
	enum Verbosity
	{
		/// \brief Full formatting with standard whitespace and line breaks.
		VERBOSITY_NORMAL,

		/// \brief Compact formatting with minimal whitespace.
		VERBOSITY_MINIMAL
	};

	/// \brief Controls whether constant expressions are folded before printing.
	enum ConstantFolding
	{
		/// \brief Print expressions without constant folding.
		CONSTANT_FOLDING_OFF,

		/// \brief Fold constant expressions and print their literal values.
		CONSTANT_FOLDING_ON
	};

	/// \brief Prints Bond source code for a parse tree rooted at a single parse node.
	/// \param parseNode Root parse node to print.
	/// \param stream Output stream receiving the formatted Bond source.
	/// \param verbosity Controls output spacing and compactness.
	/// \param folding Controls whether constant expressions are folded before printing.
	void Print(const ParseNode *parseNode, OutputStream &stream, Verbosity verbosity = VERBOSITY_NORMAL,
		ConstantFolding folding = CONSTANT_FOLDING_OFF) const;

	/// \brief Prints Bond source code for a parse tree represented as a list of parse nodes.
	/// \param listNode Head of the parse node list to print.
	/// \param stream Output stream receiving the formatted Bond source.
	/// \param verbosity Controls output spacing and compactness.
	/// \param folding Controls whether constant expressions are folded before printing.
	void PrintList(const ListParseNode *listNode, OutputStream &stream, Verbosity verbosity = VERBOSITY_NORMAL,
		ConstantFolding folding = CONSTANT_FOLDING_OFF) const;
};

}

#endif
