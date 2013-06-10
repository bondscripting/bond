#ifndef BOND_COMPILER_FRONTEND_H
#define BOND_COMPILER_FRONTEND_H

#include "bond/stl/list.h"
#include "bond/types/hashedstring.h"

namespace Bond
{

class Allocator;
class FileLoader;
class Lexer;
class Parser;
class SemanticAnalyzer;

class FrontEnd
{
public:
	FrontEnd(
			Allocator &allocator,
			Lexer &lexer,
			Parser &parser,
			SemanticAnalyzer &semanticAnalyzer,
			FileLoader &fileLoader):
		mInputFileNameList(StringList::Allocator(&allocator)),
		mLexer(lexer),
		mParser(parser),
		mSemanticAnalyzer(semanticAnalyzer),
		mFileLoader(fileLoader)
	{}

	// Note that a copy of the queued file name is not made.
	void AddInputFile(const HashedString &inputFileName);
	bool ContainsInputFile(const HashedString &inputFileName);

	void Analyze();

	bool HasErrors() const;

private:
	typedef List<HashedString> StringList;

	StringList::Type mInputFileNameList;
	Lexer &mLexer;
	Parser &mParser;
	SemanticAnalyzer &mSemanticAnalyzer;
	FileLoader &mFileLoader;
};

}

#endif
