#ifndef BOND_COMPILER_SEMANTICANALYZER_H
#define BOND_COMPILER_SEMANTICANALYZER_H

#include "bond/compiler/symboltable.h"

namespace Bond
{

class CompilerErrorBuffer;
class SymbolTable;

/// \brief A semantic analyzer for a parse tree generated from the Bond scripting language.
///
/// The SemanticAnalyzer traverses the given parse tree to populate a symbol table, resolve
/// symbol names (e.g. type names, variable names, function names and so forth), evaluate the
/// value of compile-time constants (e.g. constant expressions, type sizes and array lengths),
/// and report semantic errors (e.g. unresolved symbol names).
///
/// A single SemanticAnalyzer can analyze the parse trees generated from several translation units.
/// All translation units are assumed to be part of a common project, therefore all of the symbols
/// are added to a common symbol table.
///
/// When writing a tool that requires a compiler front end, the interactions between the front end
/// components, namely a Lexer, a Parser and a SemanticAnalyzer, can be managed by a FrontEnd.
///
/// \sa CompilerErrorBuffer, FrontEnd, Lexer, Parser, SymbolTable, TranslationUnit
/// \ingroup compiler
class SemanticAnalyzer
{
public:
	/// \brief Constructs a SemanticAnalyzer object.
	/// \param errorBuffer Buffer where error messages are pushed when semantic errors are
	///        encountered.
	/// \param pointerSize Size of pointers used by the target virtual machine that will execute the
	///        Bond code. It is required to calculate the size of types.
	SemanticAnalyzer(CompilerErrorBuffer &errorBuffer, PointerSize pointerSize = BOND_NATIVE_POINTER_SIZE):
		mErrorBuffer(errorBuffer),
		mPointerSize(pointerSize)
	{}

	~SemanticAnalyzer() {}

	/// \brief Performs semantic analysis on a list of parse trees.
	/// \param translationUnitList A linked list of parse trees to be analyzed.
	void Analyze(TranslationUnit *translationUnitList);

	/// \brief Returns the symbol table to which symbols are added during semantic analysis.
	const SymbolTable &GetSymbolTable() const { return mSymbolTable; }

	/// \brief Returns the buffer where error messages are pushed.
	const CompilerErrorBuffer &GetErrorBuffer() const { return mErrorBuffer; }

private:
	SemanticAnalyzer(const SemanticAnalyzer &other) = delete;
	SemanticAnalyzer &operator=(const SemanticAnalyzer &other) = delete;

	SymbolTable mSymbolTable;
	CompilerErrorBuffer &mErrorBuffer;
	PointerSize mPointerSize;
};

}

#endif
