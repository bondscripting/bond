#include "bond/algorithm.h"
#include "bond/compilererror.h"
#include "bond/fileloader.h"
#include "bond/frontend.h"
#include "bond/lexer.h"
#include "bond/parser.h"
#include "bond/semanticanalyzer.h"

namespace Bond
{

void FrontEnd::AddInputFile(const HashedString &inputFileName)
{
	if (Find(mInputFileNameList.begin(), mInputFileNameList.end(), inputFileName) == mInputFileNameList.end())
	{
		mInputFileNameList.push_back(inputFileName);
	}
}


void FrontEnd::Analyze()
{
	StringList::Type::const_iterator it = mInputFileNameList.begin();
	while ((it != mInputFileNameList.end()) && !HasErrors())
	{
		Bond::FileLoader::Handle scriptHandle = mFileLoader.LoadFile(it->GetString());
		const char *script = reinterpret_cast<const char *>(scriptHandle.Get().mData);
		const size_t length = scriptHandle.Get().mLength;
		Bond::TokenStream stream = mLexer.Lex(script, length)->GetTokenStream();

		if (!HasErrors())
		{
			mParser.Parse(stream);
		}
		++it;
	}

	if (!HasErrors())
	{
		mSemanticAnalyzer.Analyze(mParser.GetTranslationUnitList());
	}
}


bool FrontEnd::HasErrors() const
{
	return
		mLexer.GetErrorBuffer().HasErrors() ||
		mParser.GetErrorBuffer().HasErrors() ||
		mSemanticAnalyzer.GetErrorBuffer().HasErrors();
}

}
