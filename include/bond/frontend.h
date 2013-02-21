#ifndef BOND_FRONTEND_H
#define BOND_FRONTEND_H

#include "bond/list.h"
#include "bond/hashedstring.h"

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
			FileLoader &fileLoader,
			bool recursiveCompileEnabled = false):
		mInputFileNameList(StringList::Allocator(&allocator)),
		mLexer(lexer),
		mParser(parser),
		mSemanticAnalyzer(semanticAnalyzer),
		mFileLoader(fileLoader),
		mRecursiveCompileEnabled(recursiveCompileEnabled)
	{}

	void AddInputFile(const HashedString &inputFileName);
	bool ContainsInputFile(const HashedString &inputFileName);

	bool GetRecursiveCompileEnabled() const { return mRecursiveCompileEnabled; }
	void SetRecursiveCompileEnabled(bool enabled) { mRecursiveCompileEnabled = enabled; }

	void Analyze();

	bool HasErrors() const;

private:
	typedef List<HashedString> StringList;

	StringList::Type mInputFileNameList;
	Lexer &mLexer;
	Parser &mParser;
	SemanticAnalyzer &mSemanticAnalyzer;
	FileLoader &mFileLoader;
	bool mRecursiveCompileEnabled;
};

}

#endif
